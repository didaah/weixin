<?php

function weixin_system_tools_ajax($v, $g, $type) {
  global $_weixin;

  switch ($type) {
    case 'default':
      $api = new WXBizMsgCrypt($_weixin->app_token, $_weixin->app_aes_key, $_weixin->app_id);
      $data = file_get_contents('php://input');
      $request = ''; 
      $errCode = $api->DecryptMsg($g['msg_signature'], $g['timestamp'], $g['nonce'], $data, $request);
      text_log('weixin', $data);
      text_log('weixin', serialize($_GET));
      if ($errCode == 0) {
        // 首次接入验证
        if (!empty($g['echostr'])) {
          echo $g['echostr'];
          exit;
        }

        $data = (array) simplexml_load_string(str_replace('<![CDATA[]]>', '0', $request), 'SimpleXMLElement', LIBXML_NOCDATA);
        // 已关联用户
        if (!empty($data['FromUserName'])) {
          $ac = db_query('SELECT * FROM {user_ext} WHERE type = ? AND ext_type = ?',
          array('weixin', $data['FromUserName']), array('return' => 'one'));
        }

        if ($values = _api_wx_spdb_hb_filter_data($data, $ac)) {
          // 回复消息
          if ($output = api_wx_reply($values)) {
            echo $output;
          }
        }
      } else {
        dd_log('weixin_error', $errCode);
      }
    break;
  }
}

