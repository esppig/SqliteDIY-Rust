#include <stdlib.h>
#include <stdio.h>
#include "cursor.h"
#include "btree.h"

// 在表的起始位置创建光标
// Cursor* table_start(Table* table) {
//     Cursor* cursor = malloc(sizeof(Cursor));
//     cursor->table = table;
//     // cursor->row_num = 0;
//     // cursor->end_of_table = (table->num_rows == 0);
//     cursor->page_no = table->root_page_no;
//     cursor->cell_no = 0;

//     void* root_node = get_page(table->pager, table->root_page_no);
//     uint32_t num_cells = *leaf_node_num_cells(root_node);
//     cursor->end_of_table = (num_cells == 0);

//     return cursor;
// }

// 由于现在page的起始是root节点，(可能)并不是叶节点，并不包含数据
Cursor* table_start(Table* table) {
    Cursor* cursor = table_find(table, 0);

    void* node = get_page(table->pager, cursor->page_no);
    uint32_t cells_count = *leaf_node_num_cells(node);
    cursor->end_of_table = (cells_count = 0);
    return cursor;
}

// 在表的结束位置创建光标
Cursor* table_end(Table* table) {
    Cursor* cursor = malloc(sizeof(Cursor));
    cursor->table = table;
    // cursor->row_num = table->num_rows;
    cursor->page_no = table->root_page_no;
    
    void* root_node = get_page(table->pager, table->root_page_no);
    uint32_t num_cells = *leaf_node_num_cells(root_node);
    cursor->cell_no = num_cells;

    cursor->end_of_table = true;
    return cursor;
}

// 返回给定cell键的位置光标, 如果未找到, 则返回应该插入的位置
Cursor* table_find(Table* table, uint32_t key) {
    uint32_t root_page_no = table->root_page_no;
    void* root_node = get_page(table->pager, root_page_no);

    if (get_node_type(root_node) == NODE_LEAF) {
        return leaf_node_find(table, root_page_no, key);
    }
    // printf("Need to implement searching an internal node\n");
    // exit(EXIT_FAILURE);
    return internal_node_find(table, root_page_no, key);
}

// 取代 row_slot 函数 获取page
void* cursor_value(Cursor* cursor) {
    // uint32_t row_num = cursor->row_num;
    // uint32_t page_num = row_num / ROWS_PER_PAGE;
    uint32_t page_num = cursor->page_no;
    void* page = get_page(cursor->table->pager, page_num);
    // uint32_t row_offset = row_num % ROWS_PER_PAGE;
    // uint32_t byte_offset = row_offset * ROW_SIZE;
    // return page+byte_offset;
    return leaf_node_value(page, cursor->cell_no);
}

// 移动光标到下一条记录
void cursor_advance(Cursor* cursor) {
    uint32_t page_num = cursor->page_no;
    void* node = get_page(cursor->table->pager, page_num);

    cursor->cell_no += 1;
    if (cursor->cell_no >= (*leaf_node_num_cells(node))) {
        // cursor->end_of_table = true;
        uint32_t next_page_no = *leaf_node_next_leaf(node);
        if (next_page_no == 0) {
            // 最右侧叶节点
            cursor->end_of_table = true;
        } else {
            cursor->page_no = next_page_no;
            cursor->cell_no = 0;
        }
    }
}