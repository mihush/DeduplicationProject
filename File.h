//
// Created by Polina on 11-Dec-17.
//

#ifndef DEDUPLICATION_PROJECT_FILE_H
#define DEDUPLICATION_PROJECT_FILE_H

#include "List.h"
#include "Utilities.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* **************** START **************** block_info struct **************** START **************** */
struct block_info{ //helper struct
    int size;
    char* id; // block id
};
typedef struct block_info* Block_Info;

static ListElement copy_block_info(ListElement block_info){
    assert(block_info);
    Block_Info bi = (Block_Info)(block_info);
    Block_Info bi_copy = malloc(sizeof(*bi_copy));
    if(bi_copy == NULL){
        return NULL;
    }

    bi_copy->size = bi->size;
    bi_copy->id = malloc(sizeof(char)*(strlen(bi->id) +1));
    if(bi_copy->id == NULL){
        free(bi_copy);
        return NULL;
    }
    strcpy(bi_copy->id , bi->id);
    return bi_copy;

}

static void free_block_info(ListElement block_info){
    free(((Block_Info)(block_info))->id);
    free(block_info);
}

/* ***************** END ***************** block_info struct ***************** END ***************** */
/* ************************************************************************************************* */
/* ************************************************************************************************* */
/* *************** START ************** File STRUCT Definition *************** START *************** */
struct file_t{
    unsigned long file_sn; // file serial number
    char* file_id; // file id
    unsigned int file_depth; // the high in depot tree
    unsigned int dir_sn; //Serial number of the directory containing this file
    int num_blocks; //number of blocks contained in this file
    List blocks_list; // list of block_info objects contained in this file
};
typedef struct file_t *File;

/* **************** END *************** File STRUCT Definition **************** END **************** */


/*
 *
 */
File file_create(char* file_id , unsigned int depth , unsigned long file_sn , unsigned int dir_sn){
    assert(file_sn > 0);
    File file = malloc(sizeof(*file));
    if(file == NULL){
        printf("--> allocation Failed 1\n");
        return NULL;
    }

    file->file_id = malloc(sizeof(char)* (FILE_ID_LEN + 1));
    if(file->file_id == NULL){
        printf("--> allocation Failed 2\n");
        free(file);
        return NULL;
    }

    file->file_id = strcpy(file->file_id , file_id);
    file->file_sn = file_sn;
    file->dir_sn = dir_sn;
    file->num_blocks = 0;
    file->file_depth = depth;

    file->blocks_list = listCreate(copy_block_info , free_block_info);
    if(file->blocks_list == NULL){
        printf("--> allocation Failed 3\n");
        free(file->file_id);
        free(file);
        return NULL;
    }
    return file;
}


/*
 *
 */
void file_destroy(File file){
    assert(file);

    free(file->file_id);
    listDestroy(file->blocks_list);
    free(file);
}

/*
 *
 */
unsigned long file_get_SN(File file){
    assert(file);
    return file->file_sn;
}

char* file_get_ID(File file){
    assert(file);
    return file->file_id;
}

/*
 *
 */
unsigned int file_get_depth(File file){
    assert(file);
    return file->file_depth;
}
/*
 *
 */
ErrorCode file_add_block(File file , char* block_id , int block_size){
    if(file == NULL || block_id == NULL || block_size < 0){
        return INVALID_INPUT;
    }

    Block_Info bi = malloc(sizeof(*bi));
    if(bi == NULL){
        return OUT_OF_MEMORY;
    }
    bi->id =  malloc(sizeof(char)*(strlen(block_id) +1));
    if(bi->id == NULL){
        free(bi);
        return OUT_OF_MEMORY;
    }
    strcpy(bi->id , block_id);
    bi->size = block_size;

    ListResult res = listInsertLast(file->blocks_list , bi);
    printf("added block to file \n");
    if(res != LIST_SUCCESS){
        free(bi->id);
        free(bi);
        printf("--> Failed in block info allocation in list \n");
        return OUT_OF_MEMORY;
    }
    file->num_blocks += 1;
    free(bi->id);
    free(bi);
    return SUCCESS;
}

#endif //DEDUPLICATION_PROJECT_FILE_H
