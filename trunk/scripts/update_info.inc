<?php

/**
 * @file
 *  用户数据同步，将不在 users 和 user_ext 表中的关注用户信息导入，主要用于解决以下三个问题：
 *    1、在未启用模块前，公众号已有用户关注。
 *    2、在用户关注时，接口出现问题，导致用户数据未能写入 users 和 user_ext 表
 *    3、用户关注时，为加快响应速度，未调用接口获取用户昵称等信息，导致用户用户不完整
 */


global $_weixin;

if (!$token = weixin_get_access_token()) {
  message('无法获取到 access token');
  return false;
}


$next_openid = '';

// 一次最多获取 10000 个关注者，暂时设置为最多获 10 次，以免访问超限
for ($i = 0; $i < 10; $i++) {
  $url = 'https://api.weixin.qq.com/cgi-bin/user/get?access_token=' . $token;
  if (!empty($next_openid)) {
    $url . = 'next_openid=' . $next_openid;
  }

  $result = dd_http_request($url, array(), 'GET');

  if ($data = json_decode($result->data)) {
    if (!empty($data->count)) {
      foreach ($data->data->openid as $openid) {
        $is_update = false;
        // 查询该用户是否为系统默认名称，若不是默认名称，则跳过
        if ($ac = db_exec('SELECT * FROM {user_ext} WHERE type = ? AND ext_type = ?',
        array('weixin', $openid), array('return' => 'one'))) {
          if (substr($ac->ext_name, 0, 3) != 'wx_') {
            message($ac->ext_name . ' 的用户信息已更新过。');
            continue;
          }
          $is_update = true;
        }
        // 本地数据库中还没有用户信息

        // 获取用户微信基本信息
        if (!$weixin = weixin_get_user_info($openid)) {
          continue;
        }

        // 更新昵称和基本信息
        if (!empty($is_update)) {
          db_exec('UPDATE {user_ext} SET ext_name = ?, data = ? WHERE aid = ?',
          array($weixin->nickname, serialize($weixin), $ac->aid));
          db_exec('UPDATE {users} SET name = ? WHERE uid = ?',
          array($weixin->nickname, $ac->uid));
          dd_log('weixin_user', $weixin->nickname . ' 更新成功');
        } else {
          $ac = new stdClass;
          
          $ac->name = $weixin->nickname;

          // 检查用户名是否存在，若存在，则增加编号，产生一个新的用户名。
          $i = 0;
          while ($uid = db_query('SELECT uid FROM {users} WHERE name = ?',
          array($ac->name), array('return' => 'column'))) {
            $ac->name = 'wx_' . $i . '_' . $ac->name;
            ++$i;
          }
        
          // 生成随机密码 
          $ac->pass = dd_range_str(). dd_range_str();

          // 生成一个邮箱
          $ac->mail = $ac->name . '@test.weixin.com';

          $user_ext = new stdClass();
          // 设为默认帐号
          $user_ext->ext_id = 1;
          $user_ext->ext_type = $openid; 
          $user_ext->type = 'weixin';
          $user_ext->ext_name = $ac->name;
          $user_ext->timestamp = time();
          $user_ext->data = $weixin;
          
          // 用户创建成功后的状态
          $ac->status = var_get_key('weixin', 'user_status', 1) ? 1 : (var_get_key('user', 'status', 1) == 1 ? 1 : 0);
          $error = '';
          
          if ($user = user_save($ac, NULL, $error)) {

            // 写入微信帐号扩展信息，以绑定到用户
            if (!user_ext_save($user, array($user_ext))) {
              message($user->name . ' 绑定失败');
              dd_log('weixin_user', $weixin->nickname . ' 绑定成功');
            } else {
              message($user->name . ' 绑定成功');
              dd_log('wexin_user', $weixin->nickname . ' 绑定失败');
            }

            // 将微信头像写入本地
            if (!empty($weixin->headimgurl) && $field = field_get_cache_field_key('user', 'field', 'avatar')) {
              if ($_image = file_get_contents($weixin->headimgurl)) {
                file_save_data($_image, array(
                  'uid' => $user->uid,
                  'ext_id' => $field->field_id,
                  'ext_id2' => $user->uid,
                  'ext_type' => 'field_file',
                  'filesize' => 4096,
                  'filename' => $user->uid,
                  'title' => $user->name . '的头像',
                  'extension' => 'jpg',
                  'savepath' => 'avatar',
                ), 0);
              }
            }
          }
        }
      }
    }

    if (!empty($data->next_openid)) {
      $next_openid = $data->next_openid;
    }

    if (round($data->total/10000) > $i) {
      break;
    }

    if ($data->count < 10000) {
      break;
    }
  } else {
    break;
  }
}

