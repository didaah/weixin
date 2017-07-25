微信昵称包含 utf8mb4 字符，目前核心数据库类尚不支持创建表时使用 utf8mb64编码
为防止微信用户信息写入出错，本模块安装完成后，请在数据库中执行以下语句：

ALTER TABLE `user_ext` 
  DEFAULT CHARACTER SET=utf8mb4,
  COLLATE=utf8mb4_general_ci,
  MODIFY COLUMN `ext_name` varchar(64) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL DEFAULT '' COMMENT '第三方用户名' AFTER `uid`,
  MODIFY COLUMN `data` longtext CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL COMMENT '其它数据' AFTER `ext_id`;


ALTER TABLE `users` 
  DEFAULT CHARACTER SET=utf8mb4,
  COLLATE=utf8mb4_general_ci,
  MODIFY COLUMN `name` varchar(64) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL DEFAULT '' COMMENT '昵称' AFTER `uid`,
  MODIFY COLUMN `data` longtext CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL COMMENT '其它数据' AFTER `session`;


并在 setting.php 文件中添加 set_names 参数，示例：

  'port' => '3306',
  'set_names' => 'utf8mb4',
  'prefix' => '',


