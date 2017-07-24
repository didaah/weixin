<?php

function weixin_system_tools_ajax($v, $g, $type) {
  global $_weixin;

  switch ($type) {
    case 'default':
      $api = new WXBizMsgCrypt($_weixin->app_token, $_weixin->app_aes_key, $_weixin->app_id);
      $data = file_get_contents('php://input');
      $request = ''; 
      $errCode = $api->DecryptMsg(!empty($g['msg_signature']) ? $g['msg_signature'] : $g['signature'], $g['timestamp'], $g['nonce'], $data, $request);

      text_log('weixin_data', $data);

      if ($errCode == 0) {
        // 首次接入验证
        //$errCode = $api->VerifyURL($g['msg_signature'], $g['timestamp'], $g['nonce'], $g['echostr'], $output);

        if (!empty($g['echostr'])) {
          echo $g['echostr'];
          exit;
        }

        $data = (array) simplexml_load_string(str_replace('<![CDATA[]]>', '0', $request), 'SimpleXMLElement', LIBXML_NOCDATA);

        $output = array();
        $value = array('message' => $data, 'post' => $v, 'get' => $g);

        if (!empty($data['MsgType'])) {
          module_alter_all('weixin_message_filter_' . $data['MsgType'], $value, $output);
        }

        if (empty($output)) {
          weixin_message_filter_default($value, $output);
        }

        $default = array(
          'ToUserName' => $data['FromUserName'],
          'FromUserName' => $data['ToUserName'],
          'CreateTime' => time(),
          'MsgType' => 'text',
        );

        echo weixin_message_reply(array_merge($values, $output));
      } else {
        text_log('weixin_data', $errCode);
        dd_log('weixin_error', $errCode);
      }
    break;
  }
}

