<?php
// $Id$

/**
 * @file 自定义计划任务
 */

/**
 * @Implement of hook_cronjobs()
 * @param array $timestamp
 *  所定义的每个任务上次执行时间
 */
function weixin_cronjobs($timestamp) {
  
  $cronjobs = array();

  $cronjobs['update_info'] = array(
    'name' => t('weixin', '用户信息同步'),
    'script' => 'update_info.inc',
    'year' => '*',
    'month' => '*',
    'day' => '*',
    'hour' => '*',
    'minute' => '10', 
  );

  return $cronjobs;
}
