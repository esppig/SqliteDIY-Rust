#include "btree.h"

// * =====
// * Common Node Header Layout
// * =====

// 节点类型大小
const uint32_t NODE_TYPE_SIZE = sizeof(uint8_t);

// 节点类型的偏移量
const uint32_t NODE_TYPE_OFFSET = 0;

// 根节点大小
const uint32_t IS_ROOT_SIZE = sizeof(uint8_t);

// 根节点偏移量
const uint32_t IS_ROOT_OFFSET = NODE_TYPE_SIZE;

// 父节点指针大小
const uint32_t PARENT_POINT_SIZE = sizeof(uint32_t);

// 父节点指针偏移量
const uint32_t PARENT_POINT_OFFSET = IS_ROOT_OFFSET + IS_ROOT_SIZE;

// 通用节点头部大小
const uint32_t COMMON_NODE_HEADER_SIZE = NODE_TYPE_SIZE + IS_ROOT_SIZE + PARENT_POINT_SIZE;

// * =====
// * Leaf Node Format
// * 除了 通用头节点信息, 叶节点还需要存储自身包含的 cell [ k/v pairs ] 数量
// * =====

// 叶节点cell数量的大小
const uint32_t LEAF_NODE_NUMS_CELLS_SIZE = sizeof(uint32_t);

// 叶节点cell数量的偏移量
const uint32_t LEAF_NODE_NUMS_CELLS_OFFSET = COMMON_NODE_HEADER_SIZE;

// 叶节点头部大小
const uint32_t LEAF_NODE_HEADER_SIZE = COMMON_NODE_HEADER_SIZE + LEAF_NODE_NUMS_CELLS_SIZE;

// * =====
// * Leaf Node Body Layout
// * 叶节点体布局, 是cell数组
// * =====

// 叶节点的键大小
const uint32_t LEAF_NODE_KEY_SIZE = sizeof(uint32_t);

// 叶节点的键偏移量
const uint32_t LEAF_NODE_KEY_OFFSET = 0;

// 叶节点的值大小 [行记录大小]
const uint32_t LEAF_NODE_VALUE_SIZE = ROW_SIZE;

// 叶节点的值偏移量
const uint32_t LEAF_NODE_VALUE_OFFSET = LEAF_NODE_KEY_OFFSET + LEAF_NODE_KEY_SIZE;

// 叶节点cell的大小
const uint32_t LEAF_NODE_CELL_SIZE = LEAF_NODE_KEY_SIZE + LEAF_NODE_VALUE_SIZE;

// 叶节点cells的空间
const uint32_t LEAF_NODE_SPACE_FOR_CELLS = PAGE_SIZE - LEAF_NODE_HEADER_SIZE;

// 叶节点最大cells数量
const uint32_t LEAF_NODE_MAX_CELLS = LEAF_NODE_SPACE_FOR_CELLS / LEAF_NODE_CELL_SIZE;
