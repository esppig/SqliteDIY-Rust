#include <string.h>
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
    char username[COLUMN_USERNAME_SIZE];
    char email[COLUMN_EMAIL_SIZE];
} Row;

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

