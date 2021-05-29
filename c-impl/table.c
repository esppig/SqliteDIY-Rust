#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>  // open O_RDWR
// #include <sys/types.h>
#include <unistd.h> // lseek
#include <sys/stat.h>  // S_IWUSR
#include <errno.h>
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

// 创建新表
// Table* new_table() {
//     Table* table = malloc(sizeof(Table));
//     table->num_rows = 0;
//     for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
//         table->pages[i] = NULL;
//     }
//     return table;
// }

// 释放表占用的内存
// void free_table(Table* table) {
//     for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
//         free(table->pages[i]);
//     }
//     free(table);
// }

// 打印行记录
void print_row(Row* row) {
    printf("(%d, %s, %s)\n", row->id, row->username, row->email);
}

// 打开DB文件 构建Pager
Pager* pager_open(const char* filename) {
    int fd = open(filename,
                    O_RDWR |  // 读写模式
                        O_CREAT, // 创建文件如果文件不存在
                    S_IWUSR | // 用户写权限
                        S_IRUSR); // 用户读权限
    if (fd == -1) {
        printf("Unable to open file\n");
        exit(EXIT_FAILURE);
    }
    // 文件长度
    off_t file_length = lseek(fd, 0, SEEK_END);

    Pager* pager = malloc(sizeof(Pager));
    pager->fd = fd;
    pager->file_length = file_length;

    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        pager->pages[i] = NULL;
    }

    return pager;
}

// 初始化 pager 结构， 初始化 table 结构
Table* db_open(const char* filename) {
    Pager* pager = pager_open(filename);
    // 文件长度/每一条行记录的大小 = 行记录的总数
    uint32_t num_rows = pager->file_length /  ROW_SIZE;
    Table* table = malloc(sizeof(Table));
    table->pager = pager;
    table->num_rows = num_rows;
    return table;
}

// 页缓存未命中时，申请内存
// 当flush时，讲数据刷到磁盘文件中
void* get_page(Pager* pager, uint32_t page_num) {
    if (page_num > TABLE_MAX_PAGES) {
        printf("Tried to fetch page number out of bounds."
            "%d > %d\n", page_num, TABLE_MAX_PAGES);
        exit(EXIT_FAILURE);
    }
    // cache missing
    // 申请内存，并且从文件中加载数据
    if (pager->pages[page_num] == NULL) {
        void* page = malloc(PAGE_SIZE);
        // 文件长度 / 每页大小 = 页数[下阈]
        uint32_t num_pages = pager->file_length / PAGE_SIZE;
        // 所以也可能需要保存文件最后的部分页
        if (pager->file_length % PAGE_SIZE != 0) {
            num_pages += 1;
        }

        if (page_num <= num_pages) {
            // 调整文件当前指针到所需页的开头 [页数从0开始]
            lseek(pager->fd, page_num * PAGE_SIZE, SEEK_SET);
            // 读取一整页的内容到 page [刚刚申请好的内存]
            ssize_t bytes_read = read(pager->fd, page, PAGE_SIZE);
            if (bytes_read == -1) {
                printf("Error reading file: %d\n", errno);
                exit(EXIT_FAILURE);
            }
        }

        pager->pages[page_num] = page;
    }

    return pager->pages[page_num];
}

// 找到存放行记录的页中的slot位置
void* row_slot(Table* table, uint32_t row_num) {
    // 找到行记录所在的页号[行记录数 / 每页存放的行记录数]
    uint32_t page_num = row_num / ROWS_PER_PAGE;

    // 查看表中对应页号的指针
    // void* page = table->pages[page_num];
    // if (page == NULL) {
         // 如果为NULL，则为对应页号分配内存
    //     page = table->pages[page_num] = malloc(PAGE_SIZE);
    // }

    void* page = get_page(table->pager, page_num);

    // 找到行记录的偏移位置 [行记录数 % 每页存放的记录数]
    uint32_t row_offset = row_num % ROWS_PER_PAGE;
    // 字节偏移量 [行记录的偏移位置 * 行记录的大小]
    uint32_t byte_offset = row_offset * ROW_SIZE;
    return page+byte_offset;
}

// 将页内容刷到磁盘
// 目前的设计文件长度和行记录条数相关，因此需要计算文件最后的部分页
void pager_flush(Pager* pager, uint32_t page_num, uint32_t size) {
    if (pager->pages[page_num] == NULL) {
        printf("Tried to flush null page\n");
        exit(EXIT_FAILURE);
    }

    off_t offset = lseek(pager->fd, page_num * PAGE_SIZE, SEEK_SET);
    if (offset == -1) {
        printf("Error seeking: %d\n", errno);
        exit(EXIT_FAILURE);
    }

    ssize_t bytes_written = write(pager->fd, pager->pages[page_num], size);

    if (bytes_written == -1) {
        printf("Error writing: %d\n", errno);
        exit(EXIT_FAILURE);
    }
}

// 关闭数据库
// 1. 将页缓存中的内容刷磁盘
// 2. 关闭DB文件
// 3. 释放内存[Pager && Table]
void db_close(Table* table) {
    Pager* pager = table->pager;

    // 表中总的记录数 / 一页的记录数 = 表中总页数[下阈]
    uint32_t num_full_pages = table->num_rows / ROWS_PER_PAGE;

    for (uint32_t i = 0; i < num_full_pages; i++) {
        if (pager->pages[i] == NULL) {
            // 空page
            continue;
        }
        pager_flush(pager, i, PAGE_SIZE);
        free(pager->pages[i]);
        pager->pages[i] = NULL;
    }

    // 同样，可能余有部分页记录需要写到文件最后
    uint32_t num_additional_rows = table->num_rows % ROWS_PER_PAGE;
    if (num_additional_rows > 0) {
        uint32_t page_num = num_full_pages;
        if (pager->pages[page_num] != NULL) {
            pager_flush(pager, page_num, num_additional_rows * ROW_SIZE);
            free(pager->pages[page_num]);
            pager->pages[page_num] = NULL;
        }
    }

    int ret_code = close(pager->fd);
    if (ret_code == -1) {
        printf("Error cloing db file.\n");
        exit(EXIT_FAILURE);
    }
    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        void* page = pager->pages[i];
        if (page != NULL) {
            free(page);
            pager->pages[i] = NULL;
        }
    }
    free(pager);
    free(table);
}
