CREATE TABLE IF NOT EXISTS `User` (
    `id` INT NOT NULL AUTO_INCREMENT,
    `first_name` VARCHAR(256) NOT NULL,
    `last_name` VARCHAR(256) NOT NULL,
    `login` VARCHAR(256) UNIQUE NOT NULL,
    `password` VARCHAR(256) NOT NULL,
    `addres` VARCHAR(256) NULL,
    PRIMARY KEY (`id`),KEY `fn` (`first_name`),KEY `ln` (`last_name`)
);