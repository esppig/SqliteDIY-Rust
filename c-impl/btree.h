#ifndef __BTREE_H_
#define __BTREE_H_

#include <stdint.h>
#include "const.h"
#include "cursor.h"

typedef enum {
    NODE_INTERNAL, // 非叶子节点
    NODE_LEAF // 叶子节点
} NodeType;


// 标识节点类型
extern const uint32_t NODE_TYPE_SIZE;

// 节点类型的偏移量
extern const uint32_t NODE_TYPE_OFFSET;

// 标识是否为根节点
extern const uint32_t IS_ROOT_SIZE;

// 标识是否为根节点的偏移量
extern const uint32_t IS_ROOT_OFFSET;

// 父节点指针
extern const uint32_t PARENT_POINT_SIZE;

// 父节点指针偏移量
extern const uint32_t PARENT_POINT_OFFSET;

// 通用节点头部大小
extern const uint32_t COMMON_NODE_HEADER_SIZE;

// * =====
// * Leaf Node Format
// * 除了 通用头节点信息, 叶节点还需要存储自身包含的 cell [ k/v pairs ] 数量
// * =====

// 标识叶节点cell数量
extern const uint32_t LEAF_NODE_NUMS_CELLS_SIZE;

// 叶节点cell数量的偏移量
extern const uint32_t LEAF_NODE_NUMS_CELLS_OFFSET;

// 叶节点头部大小
extern const uint32_t LEAF_NODE_HEADER_SIZE;

// * =====
// * Leaf Node Body Layout
// * 叶节点体布局, 是cell数组
// * =====

// 叶节点的键
extern const uint32_t LEAF_NODE_KEY_SIZE;

// 叶节点键的偏移量
extern const uint32_t LEAF_NODE_KEY_OFFSET;

// 叶节点的值[行记录]
extern const uint32_t LEAF_NODE_VALUE_SIZE;

// 叶节点的值偏移量
extern const uint32_t LEAF_NODE_VALUE_OFFSET;

// 叶节点cell的大小
extern const uint32_t LEAF_NODE_CELL_SIZE;

// 单页 叶节点cells的最大空间
// 当剩余空间无法放下完整cell的时候，我们放弃剩下的空间，避免将cell分裂到不同的节点
extern const uint32_t LEAF_NODE_SPACE_FOR_CELLS;

// 叶节点最大cells数量
extern const uint32_t LEAF_NODE_MAX_CELLS;

// 返回叶节点的cell数量
uint32_t* leaf_node_num_cells(void* node);

// 返回叶节点的cells
void* leaf_node_cell(void* node, uint32_t cell_num);

// 叶节点的键[cell以key开头]
uint32_t* leaf_node_key(void* node, uint32_t cell_num);

// 叶节点的cell值[键+偏移]
void* leaf_node_value(void* node, uint32_t cell_num);

// 初始化叶节点[将cell数量置为0]
void initialize_leaf_node(void* node);

// 插入cell到节点
void leaf_node_insert(Cursor* cursor, uint32_t key, Row* value);

// 因为目前没有实现内部节点，因此可以对叶节点进行二分查询
Cursor* leaf_node_find(Table* table, uint32_t page_num, uint32_t key);

// 获取节点类型
NodeType get_node_type(void* node);

// 更改节点类型
void set_node_type(void* node, NodeType type);

#endif