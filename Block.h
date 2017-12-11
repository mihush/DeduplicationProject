//
// Created by Polina on 11-Dec-17.
//

#ifndef DEDUPLICATION_PROJECT_BLOCK_H
#define DEDUPLICATION_PROJECT_BLOCK_H

#include "List.h"
#include "Utilities.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


typedef struct block_t *Block;
/*
 *  blockCreate - Creates a new Block with:
 *                      - a given serial number
 *                      - a hashed id
 *                      - creates an empty files list
 *                      - zeros the counter that contains the amount of files sharing this block
 */
Block block_create(long block_sn , char* block_id , short block_size , ErrorCode *status);


/*
 *
 */
void block_destroy(Block block);

/*
 *
 */
long block_get_SN(Block block);
char* block_get_ID(Block block);

/*
 *
 */
ErrorCode block_add_file(Block block , char* file_id);

#endif //DEDUPLICATION_PROJECT_BLOCK_H
