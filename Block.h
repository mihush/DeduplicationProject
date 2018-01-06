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
/* *************** START ************** Block STRUCT Definition *************** START *************** */
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
    List files_list; // list of file ids containing this block
};
typedef struct block_t *Block;


static ListElement copyString(ListElement str){
    assert(str);
    char* copy = malloc(strlen(str)+1);
    return copy!= NULL ? strcpy(copy , str) : NULL;
}

static void freeString(ListElement str){
    free(str);
}

/* **************** END *************** Block STRUCT Definition **************** END **************** */
/* ************************************************************************************************** */
/* ************************************************************************************************** */
/* *************** START ************** Block STRUCT Functions **************** START *************** */
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
        printf("(Block)--> Adding block to file - Allocation Error (1) \n");
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

    printf("(Block)--> Created Block Sucessfully:\n");
    printf("            - SN   : %lu \n" , block->block_sn);
    printf("            - ID   : %s \n" , block->block_id);
    printf("            - Size : %d \n" , block->block_size);
    return block;
}


/*
 *  block_destroy - Destroys and frees space of a block structure
 */
void block_destroy(Block block){
    assert(block);
    free(block->block_id);
    listDestroy(block->files_list);
    free(block);
}

/*
 *  block_get_SN - returns the SN of the block
 */
long block_get_SN(Block block){
    assert(block);
    return block->block_sn;
}

/*
 *  block_get_ID - Returns the hashed id of the block
 */
char* block_get_ID(Block block){
    assert(block);
    return block->block_id;
}

/*
 *  block_add_file - adds the file containing the block to the files list saved in the block
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

    printf("(Block)--> Containing file was added to block Successfully:\n");
    printf("            - Block SN   : %lu \n" , block->block_sn);
    printf("            - File  ID   : %s \n" , file_id);
    return SUCCESS;
}
/* **************** END *************** Block STRUCT Functions ***************** END **************** */

#endif //DEDUPLICATION_PROJECT_BLOCK_H
