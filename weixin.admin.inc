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
 * 绑定了新浪微信帐号或通过新浪微信帐号注册的用户
 */
function weixin_admin_users() {
   
  dd_set_tabs(array(
    l(t('weixin', '微信设置'), 'admin/weixin/settings'),
    l(t('weixin', '微信用户'), 'admin/weixin/users')
  ));
  
  if ($fetch = pager_query('SELECT s.*, u.name FROM {user_ext} s INNER JOIN {users} u 
  ON s.uid = u.uid WHERE s.type = ? ORDER BY u.uid DESC', 50, 0, NULL, array('weixin'))) {
    foreach ($fetch as $o) {
      $table[] = array(
        $o->name,
        $o->ext_name,
        format_date($o->timestamp),
        //l(t('weixin', '解除绑定'), 'admin/weixin/users', array(
        //  'query' => 'op=remove&uid='.$o->uid,
        //  'attributes' => array('class' => 'confirmajax')
        //)),
      );
    }
    $output = theme('table', array('用户', '微信帐号', '绑定时间', '操作'), $table, array('class' => 'center_table'));
    $output .= pager_view();
  } else {
    $output = system_no_content();
  }
  
  return $output;
}
