<?php
// $Id$

/**
 * 微信设置
 */
function weixin_admin_settings(){
  
  dd_set_tabs(array(
    l(t('weixin', '微信设置'), 'admin/weixin/settings'),
    l(t('weixin', '微信用户'), 'admin/weixin/users')
  ));
  
  $form['fields']['weixin']['user_max'] = array(
    '#type' => 'textfield',
    '#title' => t('weixin', '绑定数量'),
    '#default_value' => var_get_key('weixin', 'user_max', 1),
    '#required' => 1,
    '#description' => t('weixin', '默认情况下，一个用户只允许绑定一个微信帐号')
  );
   
  $form['fields']['weixin']['user_status'] = array(
    '#title' => t('weixin', '用户状态'),
    '#default_value' => var_get_key('weixin', 'user_status', 1),
    '#type' => 'radio',
    '#required' => 1,
    '#options' => array(t('weixin', '默认'), t('weixin', '激活')),
    '#description' => t('weixin', '通过微信帐号注册成功后的用户状态，默认则使用网站全局设置')
  );

  $form['fields']['weixin']['app_id'] = array(
    '#type' => 'textfield',
    '#title' => t('weixin', '开发者ID'),
    '#required' => 1,
    '#default_value' => var_get_key('weixin', 'app_id', NULL),
    '#description' => t('weixin', '该 ID 在微信公众号管理平台的基本配置中')
  );
  
  $form['fields']['weixin']['app_secret'] = array(
    '#type' => 'textfield',
    '#title' => t('weixin', '开发者密码'),
    '#required' => 1,
    '#default_value' => var_get_key('weixin', 'app_secret', NULL),
    '#description' => t('weixin', '该密码在微信公众号管理平台的基本配置中')
  );
 
  $form['fields']['weixin']['app_token'] = array(
    '#type' => 'textfield',
    '#title' => t('weixin', '通信Token'),
    '#required' => 1,
    '#default_value' => var_get_key('weixin', 'app_token', NULL),
    '#description' => t('weixin', '必须与基本配置中的服务器设置一致')
  );
  
  $form['fields']['weixin']['app_aes_key'] = array(
    '#type' => 'textfield',
    '#title' => t('weixin', '加密密钥'),
    '#required' => 1,
    '#default_value' => var_get_key('weixin', 'app_aes_key', NULL),
    '#description' => t('weixin', '必须与基本配置中的服务器设置一致')
  );
    
  $form['fields']['url'] = array(
    '#type' => 'value',
    '#title' => t('weixin', '服务器URL'),
    '#default_value' => url('ajax', array('absolute' => 1, 'query' => array(
      'module' => 'weixin',
      'op' => 'default'
    ))),
  );
 
  return system_setting_form($form);
}

/**
 * 绑定了微信帐号或通过微信帐号注册的用户
 */
function weixin_admin_users() {
  // 同步修改昵称
  if (!empty($_GET['aid']) && $_GET['op'] == 'update') {
    if ($ac = db_query('SELECT * FROM {user_ext} WHERE type = ? AND aid = ?',
    array('weixin', $_GET['aid']), array('return' => 'one'))) {
      // 获取用户微信基本信息
      if ($weixin = weixin_get_user_info($ac->ext_type)) {
        if ($ac->ext_name != $weixin->nickname) {
          $ac->name = $weixin->nickname;
          db_exec('UPDATE {user_ext} SET ext_name = ?, data = ? WHERE aid = ?',
          array($ac->name, serialize($weixin), $ac->aid));

          // 检查用户名是否存在，若存在，则增加编号，产生一个新的用户名。
          $i = 0;
          while ($uid = db_query('SELECT uid FROM {users} WHERE name = ?',
          array($ac->name), array('return' => 'column'))) {
            $ac->name = 'wx_' . $i . '_' . $ac->name;
            ++$i;
          }

          db_exec('UPDATE {users} SET name = ? WHERE uid = ?',
          array($ac->name, $ac->uid));
        } else {
          dd_set_message($ac->name . ' 昵称一致，不用更新');
        }
      }
    }
    dd_goto('admin/weixin/users');
  }
   
  dd_set_tabs(array(
    l(t('weixin', '微信设置'), 'admin/weixin/settings'),
    l(t('weixin', '微信用户'), 'admin/weixin/users')
  ));

  $output = '<p><a href="">导入用户</a></p>';
  
  if ($fetch = pager_query('SELECT s.*, u.name FROM {user_ext} s INNER JOIN {users} u 
  ON s.uid = u.uid WHERE s.type = ? ORDER BY u.uid DESC', 50, 0, NULL, array('weixin'))) {
    foreach ($fetch as $o) {
      $table[] = array(
        $o->name,
        $o->ext_name,
        format_date($o->timestamp),
        l(t('weixin', '更新昵称'), 'admin/weixin/users', array(
          'query' => 'op=update&aid=' . $o->aid,
          'attributes' => array('class' => 'confirm')
        )),
      );
    }
    $output .= theme('table', array('用户', '微信帐号', '绑定时间', '操作'), $table, array('class' => 'center_table'));
    $output .= pager_view();
  } else {
    $output = system_no_content();
  }
  
  return $output;
}
