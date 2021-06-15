-- Adminer 4.8.0 MySQL 8.0.24 dump

SET NAMES utf8;
SET time_zone = '+00:00';
SET foreign_key_checks = 0;
SET sql_mode = 'NO_AUTO_VALUE_ON_ZERO';

SET NAMES utf8mb4;

DROP TABLE IF EXISTS `network_clients`;
CREATE TABLE `network_clients` (
  `id` int NOT NULL AUTO_INCREMENT,
  `node_id` varchar(20) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL,
  `connected` bit(1) NOT NULL,
  `last_edit` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  UNIQUE KEY `mac` (`node_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;


DROP TABLE IF EXISTS `network_clients_modules`;
CREATE TABLE `network_clients_modules` (
  `id` int NOT NULL AUTO_INCREMENT,
  `client_id` int NOT NULL,
  `module` varchar(20) NOT NULL,
  `topic` varchar(100) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `topic` (`topic`),
  KEY `client_id` (`client_id`),
  CONSTRAINT `network_clients_modules_ibfk_2` FOREIGN KEY (`client_id`) REFERENCES `network_clients` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;


DROP TABLE IF EXISTS `sm_alerts`;
CREATE TABLE `sm_alerts` (
  `id` int NOT NULL AUTO_INCREMENT,
  `module_id` int NOT NULL,
  `value` varchar(20) NOT NULL,
  `date` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  KEY `module_id` (`module_id`),
  CONSTRAINT `sm_alerts_ibfk_2` FOREIGN KEY (`module_id`) REFERENCES `network_clients_modules` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;


DROP TABLE IF EXISTS `sm_config`;
CREATE TABLE `sm_config` (
  `id` int NOT NULL AUTO_INCREMENT,
  `key` varchar(50) NOT NULL,
  `value` varchar(50) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;


-- 2021-05-28 11:51:10
