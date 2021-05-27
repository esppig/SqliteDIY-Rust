#ifndef __TABLE_H_
#define __TABLE_H_

#include <stdint.h>
#include "const.h"

// Table结构
typedef struct
{
    uint32_t num_rows; // 表中的记录条数
    void* pages[TABLE_MAX_PAGES]; // 页数组
} Table;

// 行记录结构
typedef struct
{
    uint32_t id;
    // C字符串以 '\0' 结尾 需要额外一个字节
    char username[COLUMN_USERNAME_SIZE+1];
    char email[COLUMN_EMAIL_SIZE+1];
} Row;

// 将 Row数据 序列化
void serialize_row(Row* row, void* dst);

// 将目标地址的数据，反序列化到 Row 结构
void deserialize_row(void* src, Row* row);

// 找到存放行记录的页中的slot位置
void* row_slot(Table* table, uint32_t row_num);

// 创建新表
Table* new_table();

// 释放表占用的内存
void free_table(Table* table);

// 打印行记录
void print_row(Row* row);

#endif
