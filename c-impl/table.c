#include <stdlib.h>
#include <string.h>
#include "const.h"
#include "table.h"

// #define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0) -> Attribute)
// const uint32_t ID_SIZE = size_of_attribute(Row, id);
// const uint32_t USERNAME_SIZE = size_of_attribute(Row, username);
// const uint32_t EMAIL_SIZE = size_of_attribute(Row, email);

// ID字段大小[byte] 复合字面量
const uint32_t ID_SIZE = sizeof((Row){.id=0, .username={}, .email={}}.id);

// Username字段大小[byte]
const uint32_t USERNAME_SIZE = sizeof((Row){0, {}, {}}.username);

// Email字段大小[byte]
const uint32_t EMAIL_SIZE = sizeof((Row){0, {}, {}}.email);

// ID的偏移量
const uint32_t ID_OFFSET = 0;

// Username的偏移量
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;

// Email的偏移量
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + EMAIL_SIZE;

// 一行的大小
const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;

// 一页的大小[byte]
// 4096字节和大部分操作系统的虚拟内存页大小一致, 在内存页扇入扇出时，不会产生数据分裂
const uint32_t PAGE_SIZE = 4096;

// 一页能存放多少条记录
const uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;

// 一张表能存放的最大记录
const uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;
// 将 Row数据 序列化
void serialize_row(Row* row, void* dst) {
    // 从src 复制 N 个字节 到 dst
    memcpy(dst+ID_OFFSET, &(row->id), ID_SIZE);
    memcpy(dst+USERNAME_OFFSET, &(row->username), USERNAME_SIZE);
    memcpy(dst+EMAIL_OFFSET, &(row->email), EMAIL_SIZE);
}

// 将目标地址的数据，反序列化到 Row 结构
void deserialize_row(void* src, Row* row) {
    memcpy(&(row->id), src+ID_OFFSET, ID_SIZE);
    memcpy(&(row->username), src+USERNAME_OFFSET, USERNAME_SIZE);
    memcpy(&(row->email), src+EMAIL_OFFSET, EMAIL_SIZE);
}

// 
void* row_slot(Table* table, uint32_t row_num) {
    // 找到行记录所在的页号[行记录数 / 每页存放的行记录数]
    uint32_t page_num = row_num / ROWS_PER_PAGE;
    // 查看表中对应页号的指针
    void* page = table->pages[page_num];
    if (page == NULL) {
        // 如果为NULL，则为对应页号分配内存
        page = table->pages[page_num] = malloc(PAGE_SIZE);
    }
    // 找到行记录的偏移位置 [行记录数 % 每页存放的记录数]
    uint32_t row_offset = row_num % ROWS_PER_PAGE;
    // 字节偏移量 [行记录的偏移位置 * 行记录的大小]
    uint32_t byte_offset = row_offset * ROW_SIZE;
    return page+byte_offset;
}
