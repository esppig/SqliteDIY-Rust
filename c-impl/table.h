#ifndef __TABLE_H_
#define __TABLE_H_

#include "const.h"

// 页结构 抽象一个Pager的概念，管理所有的页[内存/磁盘]
typedef struct
{
    int fd; // 文件描述符
    uint32_t file_length; // 文件长度
    void* pages[TABLE_MAX_PAGES]; // 页数组
} Pager;
 
// Table结构
typedef struct
{
    uint32_t num_rows; // 表中的记录条数
    Pager* pager;  // 页结构指针
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
// Table* new_table();

// 打开DB文件
Table* db_open(const char* filename);

// 释放表占用的内存
// void free_table(Table* table);

// 打印行记录
void print_row(Row* row);

// 关闭数据库
void db_close(Table* table);

// 页缓存未命中时，申请内存
// 当flush时，讲数据刷到磁盘文件中
void* get_page(Pager* pager, uint32_t page_num);

#endif
