#ifndef __BTREE_H_
#define __BTREE_H_

#include <stdint.h>

extern const uint32_t ROW_SIZE;
extern const uint32_t PAGE_SIZE;

typedef enum {
    NODE_INTERNAL, // 非叶子节点
    NODE_LEAF // 叶子节点
} NodeType;

#endif