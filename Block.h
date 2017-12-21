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
#include <assert.h>

struct block_t{
    unsigned long block_sn; // block serial number
    char* block_id; // block id
    unsigned int block_size;
    unsigned int shared_by_num_files; // #num of files containing this block
    List files_list; // list of file ids containing this block
};
typedef struct block_t *Block;

/*-------------------------------------------------------*/
//TODO it is 10 in one example i saw - need to check length of hashed id in average
static ListElement copyString(ListElement str){
    assert(str);
    char* copy = malloc(strlen(str)+1);
    return copy!= NULL ? strcpy(copy , str) : NULL;
}

static void freeString(ListElement str){
    free(str);
}
/*-------------------------------------------------------*/

/*
 *  blockCreate - Creates a new Block with:
 *                      - a given serial number
 *                      - a hashed id
 *                      - creates an empty files list
 *                      - zeros the counter that contains the amount of files sharing this block
 */
Block block_create(char* block_id , unsigned long block_sn , unsigned int block_size){
    assert(block_sn > 0); //check invalid input

    Block block = malloc(sizeof(*block)); //create a block
    if(block == NULL){ //Check memory allocation was successful
        return NULL;
    }

    block->block_id = malloc(sizeof(char)*(BLOCK_ID_LEN + 1)); //allocate string for block_id
    if(block->block_id == NULL){ //check successful allocation
        free(block);
        return NULL;
    }
    block->block_id = strcpy(block->block_id , block_id);
    block->block_sn = block_sn;
    block->shared_by_num_files = 0;
    block->block_size = block_size;

    block->files_list = listCreate(copyString , freeString);
    if(block->files_list == NULL){ //check allocation - if failed free everything else
        free(block->block_id);
        free(block);
        return NULL;
    }
    return block;
}


/*
 *
 */
void block_destroy(Block block){
    assert(block);

    free(block->block_id);
    listDestroy(block->files_list);
    free(block);
}

/*
 *
 */
long block_get_SN(Block block){
    assert(block);
    return block->block_sn;
}

/*
 *
 */
char* block_get_ID(Block block){
    assert(block);
    return block->block_id;
}

/*
 *
 */
ErrorCode block_add_file(Block block , char* file_id){
    if(file_id == NULL || block == NULL){ //Check input is valid
        return INVALID_INPUT;
    }

    char* copy_fID = malloc(sizeof(char)*(FILE_ID_LEN + 1)); //allocate string for block_id
    if(copy_fID == NULL){ //check successful allocation
        return OUT_OF_MEMORY;
    }
    strcpy(block->block_id , copy_fID);

    listInsertFirst(block->files_list , copy_fID);
    block->shared_by_num_files += 1;
    return SUCCESS;
}

#endif //DEDUPLICATION_PROJECT_BLOCK_H
