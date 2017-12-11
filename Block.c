//
// Created by Polina on 11-Dec-17.
//

#include "Block.h"
#include <assert.h>


//TODO it is 10 in one example i saw - need to check length of hashed id in average
static ListElement copyString(ListElement str){
    assert(str);
    char* copy = malloc(strlen(str)+1);
    return copy!= NULL ? strcpy(copy , str) : NULL;
}

static void freeString(ListElement str){
    free(str);
}

struct block_t{
    long block_sn; // block serial number
    char* block_id; // block id
    short block_size;
    int shared_by_num_files; // #num of files containing this block
    List containing_file_ids; // list of file ids containing this block
};

Block block_create(long block_sn , char* block_id , short block_size , ErrorCode *status){
    if(block_sn < 0){ //check invalid input
        *status = INVALID_INPUT;
        return NULL;
    }

    Block block = malloc(sizeof(*block)); //create a block
    if(block == NULL){ //Check memory allocation was successful
        *status = OUT_OF_MEMORY;
        return NULL;
    }


    block->block_id = malloc(BLOCK_ID_LEN + 1); //allocate string for block_id
    block->block_id = strcpy(block->block_id , block_id);
    if(block->block_id == NULL){ //check successful allocation
        *status = OUT_OF_MEMORY;
        free(block);
        return NULL;
    }

    block->block_sn = block_sn;
    block->shared_by_num_files = 0;
    block->block_size = block_size;

    block->containing_file_ids = listCreate(copyString , freeString);
    if(block->containing_file_ids == NULL){ //check allocation - if failed free everything else
        *status = OUT_OF_MEMORY;
        free(block->block_id);
        free(block);
        return NULL;
    }

    return block;
}

void block_destroy(Block block){
    if(block == NULL){ //Check invalid input
        return;
    }

    free(block->block_id);
    listDestroy(block->containing_file_ids);
    free(block);
}

long block_get_SN(Block block){
    assert(block);
    return block->block_sn;
}

char* block_get_ID(Block block){
    assert(block);
    return block->block_id; //TODO choose whether to return a copy or leave it like this
}

ErrorCode block_add_file(Block block , char* file_id){
    if(file_id == NULL || block == NULL){ //Check input is valid
        return INVALID_INPUT;
    }

    char* copy_fID = malloc(FILE_ID_LEN + 1); //allocate string for block_id
    strcpy(block->block_id , copy_fID);
    if(copy_fID == NULL){ //check successful allocation
        return OUT_OF_MEMORY;
    }
    listInsertFirst(block->containing_file_ids , copy_fID);
    block->shared_by_num_files += 1;
    return SUCCESS;
}