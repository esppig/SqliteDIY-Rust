#ifndef __BTREE_H_
#define __BTREE_H_

#include <stdint.h>
#include "const.h"

typedef enum {
    NODE_INTERNAL, // 非叶子节点
    NODE_LEAF // 叶子节点
} NodeType;

// 返回叶节点的cell数量
uint32_t* leaf_node_num_cells(void* node);

// 返回叶节点的cells
void* leaf_node_cell(void* node, uint32_t cell_num);

#endif