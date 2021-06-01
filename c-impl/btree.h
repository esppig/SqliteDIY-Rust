#ifndef __BTREE_H_
#define __BTREE_H_

#include <stdint.h>
#include "const.h"
#include "cursor.h"

typedef enum {
    NODE_INTERNAL, // 非叶子节点
    NODE_LEAF // 叶子节点
} NodeType;

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

#endif