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
#include <assert.h>


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
    char flag; // L - logical_file , P - physical_file
    unsigned long file_sn;
    char* file_id;
    unsigned int file_depth;
    unsigned long dir_sn;
    int num_blocks;
    unsigned int file_size;
    List blocks_list;
    HashTableF files_ht; // should be use only for flag = 'P'
    unsigned int num_files; // should be use only for flag = 'P'
    unsigned long physical_sn;

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
File file_create(char* file_id , unsigned int depth , unsigned long file_sn , unsigned int size ,
                 unsigned long physical_sn){
    File file = malloc(sizeof(*file));
    if(file == NULL){
        printf("(File)--> Creating file - Allocation Error (1) \n");
        return NULL;
    }

    file->file_id = malloc(sizeof(char)* (FILE_ID_LEN + 1));
    if(file->file_id == NULL){
        printf("(File)--> Creating file - Allocation Error (2) \n");
        free(file);
        return NULL;
    }

    file->file_id = strcpy(file->file_id , file_id);
    file->file_sn = file_sn;
    file->dir_sn = 0; //not known in the time of creation
    file->num_blocks = 0;
    file->file_depth = depth;
    file->file_size = size;
    file->num_files = 1;
    file->flag = 'P';
    file->physical_sn = physical_sn; // will be updated from file_compare

    file->blocks_list = listCreate(copy_block_info , free_block_info);
    if(file->blocks_list == NULL){
        printf("(File)--> Adding block to file - Allocation Error (3) \n");
        free(file->file_id);
        free(file);
        return NULL;
    }

    file->files_ht = ht_createF('N');
    if(file->files_ht == NULL){
        free(file->file_id);
        listDestroy(file->blocks_list);
        free(file);
        return NULL;
    }

    ht_setF(file->files_ht, file_id);
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

/*
 *  file_get_num_blocks - returns the number of blocks the file contains
 */
int file_get_num_blocks(File file){
    assert(file);
    return file->num_blocks;
}

ErrorCode file_set_parent_dir_sn(File file , unsigned long dir_sn){
    assert(file);
    file->dir_sn = dir_sn;
    return SUCCESS;
}

ErrorCode file_set_physical_sn(File file , unsigned long physical_file_sn){
    assert(file);
    file->physical_sn = physical_file_sn;
    return SUCCESS;
}

/*
 *
 */
ErrorCode file_set_logical_flag(File file){
    file->flag = 'L';
    return SUCCESS;
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

    (file->num_blocks)++;
    free(bi->id);
    free(bi);
    return SUCCESS;
}


/* **************** END *************** File STRUCT Functions **************** END ***************** */


#endif //DEDUPLICATION_PROJECT_FILE_H
