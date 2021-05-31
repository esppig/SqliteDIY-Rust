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
