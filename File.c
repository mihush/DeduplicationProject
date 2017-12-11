//
// Created by Polina on 11-Dec-17.
//

#include "File.h"
#include <assert.h>

/* **************** START **************** block_info struct **************** START **************** */
typedef struct block_info* Block_Info;
struct block_info{ //helper struct
    int size;
    char* id; // block id
};

static ListElement copy_block_info(Block_Info block_info){ //TODO check if correct input
    assert(block_info);
    Block_Info bi_copy = malloc(sizeof(*bi_copy));
    if(bi_copy == NULL){
        return NULL;
    }

    bi_copy -> size = block_info->size;
    bi_copy->id = malloc(BLOCK_ID_LEN +1);
    if(bi_copy->id == NULL){
        free(bi_copy);
        return NULL;
    }
    strcpy(bi_copy->id , block_info->id);
    return bi_copy;
}

static void free_block_info(Block_Info block_info){
    free(block_info->id);
    free(block_info);
}

/* ***************** END ***************** block_info struct ***************** END ***************** */
/* ************************************************************************************************* */
/* ************************************************************************************************* */
/* *************** START ************** File STRUCT Definition *************** START *************** */
struct file_t{
    long file_sn; // file serial number
    char* file_id; // file id
    int num_blocks; //number of blocks contained in this file
    List contained_blocks; // list of block_info objects contained in this file
};

/* **************** END *************** File STRUCT Definition **************** END **************** */
/* ************************************************************************************************* */
/* ************************************************************************************************* */
/* ************** START ************* File Function Implementation ************* START ************* */


/* *************** END ************** File Function Implementation ************** END ************** */