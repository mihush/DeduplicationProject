//
// Created by mihush on 11/12/2017.
//

#ifndef DEDUPLICATION_PROJECT_AVLTREE_H
#define DEDUPLICATION_PROJECT_AVLTREE_H


#define _XOPEN_SOURCE 500 /* Enable certain library functions (strdup) on linux. See feature_test_macros(7) */

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <assert.h>



/****************************************************************/
struct avl_node_s {
    struct avl_node_s *left;
    struct avl_node_s *right;
    int value;
};

typedef struct avl_node_s avl_node_t;

struct avl_tree_s {
    struct avl_node_s *root;
};

typedef struct avl_tree_s avl_tree_t;





#endif //DEDUPLICATION_PROJECT_AVLTREE_H
