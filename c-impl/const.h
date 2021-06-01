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
#define COLUMN_USERNAME_SIZE  32

// Email字段的大小[byte]
#define COLUMN_EMAIL_SIZE 255

// 表能容纳的页数
// 之后使用树结构将不再设限[仅受限于文件的最大大小]
// 但是依然会限制内存中使用的最大页数
#define TABLE_MAX_PAGES 100


#include <stdint.h>

#define COLUMN_USERNAME_SIZE  32

#define COLUMN_EMAIL_SIZE 255

#define TABLE_MAX_PAGES 100

#if !defined(SQLITE_STRUCT_ROW)
#define SQLITE_STRUCT_ROW
typedef struct
{
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE+1];
    char email[COLUMN_EMAIL_SIZE+1];
} Row;
#endif

#define ID_SIZE sizeof(uint32_t)

// Username字段大小[byte]
#define USERNAME_SIZE (COLUMN_USERNAME_SIZE + 1)

// Email字段大小[byte]
#define EMAIL_SIZE (COLUMN_EMAIL_SIZE + 1)

// ID的偏移量
#define ID_OFFSET 0

// Username的偏移量
#define USERNAME_OFFSET (ID_OFFSET + ID_SIZE)

// Email的偏移量
#define EMAIL_OFFSET (USERNAME_OFFSET + USERNAME_SIZE)

// 一条行记录的大小
#define ROW_SIZE (ID_SIZE + USERNAME_SIZE + EMAIL_SIZE)

// 一页的大小[byte]
// 4096字节和大部分操作系统的虚拟内存页大小一致, 在内存页扇入扇出时，不会产生数据分裂
#define PAGE_SIZE 4096

// 一页能存放多少条记录
// 改用btree存储page，不再需要
// #define ROWS_PER_PAGE (PAGE_SIZE / ROW_SIZE)

// 一张表能存放的最大记录
// 改用btree存储page，不再需要
// #define TABLE_MAX_ROWS (ROWS_PER_PAGE * TABLE_MAX_PAGES)

#endif
