#ifndef __CONST_H_
#define __CONST_H_

#include <stdint.h>

// 硬编码表

// id   username     email
// int  varchar(32)  varchar(255)

// #define COLUMN_USERNAME_SIZE 32
// #define COLUMN_EMAIL_SIZE 255


// | column   | size (byte) | offset |
// | id       | 4           | 0      |
// | username | 32          | 4      |
// | email    | 255         | 36     |
// | total    | 291         |        |


// Username字段的大小[byte]
const int COLUMN_USERNAME_SIZE = 32;

// Email字段的大小[byte]
const int COLUMN_EMAIL_SIZE = 255;

// 表能容纳的页数
const int TABLE_MAX_PAGES = 100;


#endif