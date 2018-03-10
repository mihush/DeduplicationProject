//
// Created by Polina on 11-Dec-17.
//

#ifndef DEDUPLICATION_PROJECT_BLOCK_H
#define DEDUPLICATION_PROJECT_BLOCK_H

#include "HashTableF.h"
#include "Utilities.h"

/* ******************* START ****************** Block STRUCT Definition ****************** START ******************** */
/*
 * Definition of a block structure:
 *                  - block_sn -> a running index on all blocks read from the file system
 *                  - block_id -> a hushed id as appears in the input file
 *                  - block_size -> the size of a block
 *                  - shared_by_num_files -> number of files sharing this block
 *                  - files_list -> list of hashed file ids containing this block
 */
struct block_t{
    unsigned long block_sn; // running index
    char* block_id; // Hashed
    unsigned int block_size;
    unsigned int shared_by_num_files;
    HashTableF files_ht;
};
typedef struct block_t *Block;

/* ******************** END ******************** Block STRUCT Definition ******************** END ******************* */
/* ****************************************************************************************************************** */
/* ****************************************************************************************************************** */
/* ******************* START ******************* Block STRUCT Functions ******************* START ******************* */
/*
 *  blockCreate - Creates a new Block with:
 *                      - a given serial number
 *                      - a hashed id
 *                      - creates an empty files list
 *                      - zeros the counter that contains the amount of files sharing this block
 *
 * @block_id   - the hashed id of the block
 * @block_sn   - serial number of the block
 * @block_size - the size of the block
 */
Block block_create(char* block_id , unsigned long block_sn , unsigned int block_size){
    Block block = malloc(sizeof(*block)); //create a block
    if(block == NULL){ //Check memory allocation was successful
        return NULL;
    }

    block->block_id = calloc((BLOCK_ID_LEN + 1) , sizeof(char)); //allocate string for block_id
    if(block->block_id == NULL){ //check successful allocation
        free(block);
        return NULL;
    }
    block->block_id = strcpy(block->block_id , block_id);
    block->block_sn = block_sn;
    block->shared_by_num_files = 0;
    block->block_size = block_size;

    block->files_ht = ht_createF('N');
    if(block->files_ht == NULL){
        free(block->block_id);
        free(block);
        return NULL;
    }
    return block;
}

/*
 *  block_destroy - Destroys and frees space of a block structure
 *
 *  @block - pointer to the block structure to be destroyed
 */
void block_destroy(Block block){
    assert(block);
    free(block->block_id);
    hashTableF_destroy(block->files_ht);
    free(block);
}

/*
 *  block_get_SN - returns the SN of the block
 *
 *  @block - pointer to the block structure
 */
long block_get_SN(Block block){
    assert(block);
    return block->block_sn;
}

/*
 *  block_get_ID - Returns the hashed id of the block
 *
 *  @block - pointer to the block structure
 */
char* block_get_ID(Block block){
    assert(block);
    return block->block_id;
}

/*
 *  block_add_file - adds the file containing the block to the files list saved in the block
 *
 *  @block   - pointer to the block structure to which we want to add the file
 *  @file_id - the id of the file that contains the block
 */
ErrorCode block_add_file(Block block , char* file_id){
    if(file_id == NULL || block == NULL){ //Check input is valid
        return INVALID_INPUT;
    }

    EntryF result = ht_setF(block->files_ht, file_id);
    if(result == NULL){ //Check for memory allocation
        return OUT_OF_MEMORY;
    }

    (block->shared_by_num_files)++;
    return SUCCESS;
}


/* ******************** END ******************** Block STRUCT Functions ******************** END ******************** */

#endif //DEDUPLICATION_PROJECT_BLOCK_H
