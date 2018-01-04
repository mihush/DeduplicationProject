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
/*
 * Definition of a block info structure:
 *                  - block_sn -> a running index on all blocks read from the file system
 *                  - block_id -> a hushed id as appears in the input file
 *                  - block_size -> the size of a block
 *                  - shared_by_num_files -> number of files sharing this block
 *                  - files_list -> list of hashed file ids containing this block
 */
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
/*
 * Definition of a File structure:
 *                  - file_sn -> a running index on all files read from the file system
 *                  - file_id -> a hushed id as appears in the input file
 *                  - file_depth -> the depth of the file in the hierarchical tree
 *                  - dir_sn -> Serial number of the directory containing this file
 *                  - num_blocks -> number of blocks the file consists of
 *                  - file_size -> the size of the file
 *                  - blocks_list -> List of Block_info elements of blocks contained in the file
 */
struct file_t{
    unsigned long file_sn;
    char* file_id;
    unsigned int file_depth;
    unsigned long dir_sn;
    int num_blocks;
    unsigned int file_size;
    List blocks_list;
};
typedef struct file_t *File;

/* **************** END *************** File STRUCT Definition **************** END **************** */
/* ************************************************************************************************* */
/* ************************************************************************************************* */
/* *************** START ************** File STRUCT Functions *************** START **************** */
/*
 *  file_create - Creates a new file object with:
 *                      - file id - a hashed id as appears in the input file
 *                      - depth
 *                      -file sn - running index on all files in the filesystem
 *                      - dir sn
 *
 */
File file_create(char* file_id , unsigned int depth , unsigned long file_sn , unsigned int size){
    assert(file_sn > 0);
    File file = malloc(sizeof(*file));
    if(file == NULL){
        printf("(File)--> Creating block to file - Allocation Error (1) \n");
        return NULL;
    }

    file->file_id = malloc(sizeof(char)* (FILE_ID_LEN + 1));
    if(file->file_id == NULL){
        printf("(File)--> Adding block to file - Allocation Error (2) \n");
        free(file);
        return NULL;
    }

    file->file_id = strcpy(file->file_id , file_id);
    file->file_sn = file_sn;
    file->dir_sn = 0; //not known in the time of creation
    file->num_blocks = 0;
    file->file_depth = depth;
    file->file_size = size;

    file->blocks_list = listCreate(copy_block_info , free_block_info);
    if(file->blocks_list == NULL){
        printf("(File)--> Adding block to file - Allocation Error (3) \n");
        free(file->file_id);
        free(file);
        return NULL;
    }
    printf("(File)--> Created File Sucessfully:\n");
    printf("              - SN    : %lu \n" , file->file_sn);
    printf("              - ID    : %s \n" , file->file_id);
    printf("              - Size  : %d \n" , file->file_size);
    printf("              - Depth : %d \n" , file->file_depth);
    return file;
}


/*
 *  file_destroy - Destroys and frees space of a file structure
 */
void file_destroy(File file){
    assert(file);
    free(file->file_id);
    listDestroy(file->blocks_list);
    free(file);
    printf("(File)--> Destroyed File Sucessfully \n");
}

/*
 *  file_get_SN - returns the SN of the file
 */
unsigned long file_get_SN(File file){
    assert(file);
    return file->file_sn;
}

/*
 * file_get_ID - returns the hashed ID of the file
 */
char* file_get_ID(File file){
    assert(file);
    return file->file_id;
}

/*
 *  file_get_depth - returns the depth of the file in the hierarchy
 */
unsigned int file_get_depth(File file){
    assert(file);
    return file->file_depth;
}

ErrorCode file_set_parent_dir_sn(File file , unsigned long dir_sn){
    assert(file);
    file->dir_sn = dir_sn;
    return SUCCESS;
}
/*
 *
 */
ErrorCode file_add_block(File file , char* block_id , int block_size){
    if(file == NULL || block_id == NULL || block_size < 0){
        printf("(File)--> Adding block to file - Invalid Input Error\n");
        return INVALID_INPUT;
    }

    Block_Info bi = malloc(sizeof(*bi));
    if(bi == NULL){
        printf("(File)--> Adding block to file - Allocation Error (1) \n");
        return OUT_OF_MEMORY;
    }
    bi->id =  malloc(sizeof(char)*(strlen(block_id) +1));
    if(bi->id == NULL){
        printf("(File)--> Adding block to file - Allocation Error (2) \n");
        free(bi);
        return OUT_OF_MEMORY;
    }
    strcpy(bi->id , block_id);
    bi->size = block_size;

    ListResult res = listInsertLast(file->blocks_list , bi);
    if(res != LIST_SUCCESS){
        printf("(File)--> Adding block to file - List of files containing block allocation Error (3) \n");
        free(bi->id);
        free(bi);
        return OUT_OF_MEMORY;
    }
    file->num_blocks += 1;
    free(bi->id);
    free(bi);

    printf("(File)--> Contained Block was added to File Sucessfully:\n");
    printf("            - File  SN     : %lu \n" , file->file_sn);
    printf("            - Block ID     : %s \n" , block_id);
    printf("            - Block size   : %d \n" , block_size);
    return SUCCESS;
}

/* **************** END *************** File STRUCT Functions **************** END ***************** */


#endif //DEDUPLICATION_PROJECT_FILE_H
