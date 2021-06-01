#include <stdlib.h>  // EXIT_FAILURE
#include <stdio.h>
#include <string.h>
#include "btree.h"

// * =====
// * Common Node Header Layout
// * =====

// 标识节点类型
const uint32_t NODE_TYPE_SIZE = sizeof(uint8_t);

// 节点类型的偏移量
const uint32_t NODE_TYPE_OFFSET = 0;

// 标识是否为根节点
const uint32_t IS_ROOT_SIZE = sizeof(uint8_t);

// 标识是否为根节点的偏移量
const uint32_t IS_ROOT_OFFSET = NODE_TYPE_SIZE;

// 父节点指针
const uint32_t PARENT_POINT_SIZE = sizeof(uint32_t);

// 父节点指针偏移量
const uint32_t PARENT_POINT_OFFSET = IS_ROOT_OFFSET + IS_ROOT_SIZE;

// 通用节点头部大小
const uint32_t COMMON_NODE_HEADER_SIZE = NODE_TYPE_SIZE + IS_ROOT_SIZE + PARENT_POINT_SIZE;

// * =====
// * Leaf Node Format
// * 除了 通用头节点信息, 叶节点还需要存储自身包含的 cell [ k/v pairs ] 数量
// * =====

// 标识叶节点cell数量
const uint32_t LEAF_NODE_NUMS_CELLS_SIZE = sizeof(uint32_t);

// 叶节点cell数量的偏移量
const uint32_t LEAF_NODE_NUMS_CELLS_OFFSET = COMMON_NODE_HEADER_SIZE;

// 叶节点头部大小
const uint32_t LEAF_NODE_HEADER_SIZE = COMMON_NODE_HEADER_SIZE + LEAF_NODE_NUMS_CELLS_SIZE;

// * =====
// * Leaf Node Body Layout
// * 叶节点体布局, 是cell数组
// * =====

// 叶节点的键
const uint32_t LEAF_NODE_KEY_SIZE = sizeof(uint32_t);

// 叶节点键的偏移量
const uint32_t LEAF_NODE_KEY_OFFSET = 0;

// 叶节点的值[行记录]
const uint32_t LEAF_NODE_VALUE_SIZE = ROW_SIZE;

// 叶节点的值偏移量
const uint32_t LEAF_NODE_VALUE_OFFSET = LEAF_NODE_KEY_OFFSET + LEAF_NODE_KEY_SIZE;

// 叶节点cell的大小
const uint32_t LEAF_NODE_CELL_SIZE = LEAF_NODE_KEY_SIZE + LEAF_NODE_VALUE_SIZE;

// 单页 叶节点cells的最大空间
// 当剩余空间无法放下完整cell的时候，我们放弃剩下的空间，避免将cell分裂到不同的节点
const uint32_t LEAF_NODE_SPACE_FOR_CELLS = PAGE_SIZE - LEAF_NODE_HEADER_SIZE;

// 叶节点最大cells数量
const uint32_t LEAF_NODE_MAX_CELLS = LEAF_NODE_SPACE_FOR_CELLS / LEAF_NODE_CELL_SIZE;

// * =====
// * 访问叶节点字段/域
// * =====

// 返回叶节点的cell数量
uint32_t* leaf_node_num_cells(void* node) {
    return (uint32_t*)(node + LEAF_NODE_NUMS_CELLS_OFFSET);
}

// 返回叶节点的cells
void* leaf_node_cell(void* node, uint32_t cell_num) {
    return node + LEAF_NODE_HEADER_SIZE + cell_num * LEAF_NODE_CELL_SIZE;
}

// 叶节点的键[cell以key开头]
uint32_t* leaf_node_key(void* node, uint32_t cell_num) {
    return (uint32_t*)leaf_node_cell(node, cell_num);
}

// 叶节点的cell值[键+偏移]
void* leaf_node_value(void* node, uint32_t cell_num) {
    return leaf_node_cell(node, cell_num) + LEAF_NODE_KEY_SIZE;
}

// 初始化叶节点[将cell数量置为0]
void initialize_leaf_node(void* node) {
    // 初始化节点类型
    set_node_type(node, NODE_LEAF);
    // 初始cells数量为0
    *leaf_node_num_cells(node) = 0;
}


// 插入cell到节点
void leaf_node_insert(Cursor* cursor, uint32_t key, Row* value) {
    // 查找到游标指向 所在的节点
    void* node = get_page(cursor->table->pager, cursor->page_num);

    // 获取当前节点的cell数量
    uint32_t num_cells = *leaf_node_num_cells(node);

    // ! 目前还没有进行节点的拆分, 所以可能会超过cell最大数量
    if (num_cells >= LEAF_NODE_MAX_CELLS) {
        // cell数量超过 叶节点的最大单元数
        printf("Need to implement splitting a leaf node.\n");
        exit(EXIT_FAILURE);
    }

    if (cursor->cell_num < num_cells) {
        // 将大于光标指向的cell号的记录 向后平移
        // 为新的cell腾出空间
        for (uint32_t i = num_cells; i > cursor->cell_num; i--) {
            memcpy(leaf_node_cell(node, i), leaf_node_cell(node, i - 1), LEAF_NODE_CELL_SIZE);
        }
    }
    // cell数量+1
    *(leaf_node_num_cells(node)) += 1;

    // cell键 赋值
    *(leaf_node_key(node, cursor->cell_num)) = key;

    // 数据序列化到 cell值中
    serialize_row(value, leaf_node_value(node, cursor->cell_num));
}

// 因为目前没有实现内部节点，因此可以对叶节点进行二分查询
Cursor* leaf_node_find(Table* table, uint32_t page_num, uint32_t key) {
    void* node = get_page(table->pager, page_num);
    uint32_t num_cells = *leaf_node_num_cells(node);

    Cursor* cursor = malloc(sizeof(Cursor));
    cursor->table = table;
    cursor->page_num = page_num;

    // 二分查询
    uint32_t min_index = 0;
    uint32_t one_past_max_index = num_cells;

    while(one_past_max_index != min_index) {
        uint32_t index = (min_index + one_past_max_index) / 2;
        uint32_t key_at_index = *leaf_node_key(node, index);
        if (key == key_at_index) {
            cursor->cell_num = index;
            return cursor;
        }
        if (key < key_at_index) {
            one_past_max_index = index;
        } else {
            min_index = index + 1;
        }
    }

    cursor->cell_num = min_index;
    return cursor;
}

// 获取节点类型
NodeType get_node_type(void* node) {
    uint8_t value = *((uint8_t*)(node + NODE_TYPE_OFFSET));
    return (NodeType)value;
}

// 更改节点类型
void set_node_type(void* node, NodeType type) {
    uint8_t value = (uint8_t)type;
    *((uint8_t*)(node + NODE_TYPE_OFFSET)) = value;
}