#ifndef __CURSOR_H_
#define __CURSOR_H_

#include <stdbool.h>
#include "table.h"

// 创建表开始及结束位置的游标
// 通过游标访问当前行记录
// 移动游标到下一行记录
// 删除和修改光标指向的行记录
// 根据给定ID搜索表,并创建指向这条记录的光标

#if !defined(SQLITE_STRUCT_CURSOR)
#define SQLITE_STRUCT_CURSOR
// Page改用Btree结构后，光标不再是跟踪行记录，而需要跟踪节点的页码和cell单元号
typedef struct
{
    Table* table; // 表结构指针
    // uint32_t row_num; // 当前行记录数[编号]
    uint32_t page_num; // 节点页码
    uint32_t cell_num; // 节点cell号
    bool end_of_table; // 指示最后一个元素之后的位置[可插入新记录]
} Cursor;
#endif

// 在表的起始位置创建光标
Cursor* table_start(Table* table);

// 在表的结束位置创建光标
Cursor* table_end(Table* table);

// 取代 row_slot 函数 获取page
void* cursor_value(Cursor* cursor);

// 移动光标到下一条记录
void cursor_advance(Cursor* cursor);

// 返回给定cell键的位置光标, 如果未找到, 则返回应该插入的位置
Cursor* table_find(Table* table, uint32_t key);

#endif