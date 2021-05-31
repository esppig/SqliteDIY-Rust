#ifndef __BTREE_H_
#define __BTREE_H_

#include <stdint.h>
#include "const.h"

typedef enum {
    NODE_INTERNAL, // 非叶子节点
    NODE_LEAF // 叶子节点
} NodeType;

#endif