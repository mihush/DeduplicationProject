//
// Created by Polina on 11-Dec-17.
//

#ifndef DEDUPLICATION_PROJECT_FILE_H
#define DEDUPLICATION_PROJECT_FILE_H

#include "Utilities.h"

/* ****************************************************************************************************************** */
/* ****************************************************************************************************************** */
/* ******************* START ******************* File STRUCT Definition ******************* START ******************* */
/*
 * Definition of a File structure:
 *                  - file_sn     -> a running index on all files read from the file system
 *                  - file_id     -> a hushed id as appears in the input file
 *                  - file_depth  -> the depth of the file in the hierarchical tree
 *                  - dir_sn      -> Serial number of the directory containing this file
 *                  - num_blocks  -> number of blocks the file consists of
 *                  - file_size   -> the size of the file
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

/* ******************** END ******************** File STRUCT Definition ******************** END ******************** */
/* ****************************************************************************************************************** */
/* ****************************************************************************************************************** */
/* ******************* START ******************* File STRUCT Functions ******************* START ******************** */
/*
 *  file_create - Creates a new file object with the input parameters
 *                      - file id - a hashed id as appears in the input file
 *                      - depth
 *                      -file sn - running index on all files in the filesystem
 *                      - dir sn
 *
 *
 * @file_id     - hashed id of the file
 * @depth       - the depth of the file in the file system (Root directory starts at 0)
 * @file_sn     - serial number of the file object
 * @size        - the size of the file
 * @physical_sn - in case of file level deduplication, there are 2 types of files - physical and logical
 */
File file_create(char* file_id , unsigned int depth , unsigned long file_sn , unsigned int size ,
                 unsigned long physical_sn){
    File file = malloc(sizeof(*file));
    if(file == NULL){
        return NULL;
    }

    file->file_id = malloc(sizeof(char)* (FILE_ID_LEN + 1));
    if(file->file_id == NULL){
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
 *
 *  @file - Pointer to the file object to be destroyed
 */
void file_destroy(File file){
    assert(file);
    free(file->file_id);
    listDestroy(file->blocks_list);
    hashTableF_destroy(file->files_ht);
    free(file);
}

/*
 *  file_get_SN - Returns the SN of the file
 *
 *  @file - Pointer to the file object
 */
unsigned long file_get_SN(File file){
    assert(file);
    return file->file_sn;
}

/*
 * file_get_ID - Returns pointer to the hashed ID of the file
 *
 * @file - Pointer to the file object
 */
char* file_get_ID(File file){
    assert(file);
    return file->file_id;
}

/*
 *  file_get_depth - Returns the depth of the file in the hierarchy
 *
 *  @file - Pointer to the file object
 */
unsigned int file_get_depth(File file){
    assert(file);
    return file->file_depth;
}

/*
 *  file_get_num_blocks - returns the number of blocks the file contains
 *
 *  @file - Pointer to the file object
 */
int file_get_num_blocks(File file){
    assert(file);
    return file->num_blocks;
}

/*
 *  file_get_num_blocks - returns the number of blocks the file contains
 *
 *  @file - Pointer to the file object
 */
ErrorCode file_set_parent_dir_sn(File file , unsigned long dir_sn){
    assert(file);
    file->dir_sn = dir_sn;
    return SUCCESS;
}

/*
 *  file_set_physical_sn - Set the value of the physical serial number of the file object
 *
 *  @file             - Pointer to the file object
 *  @physical_file_sn - value of the serial number to be set
 */
ErrorCode file_set_physical_sn(File file , unsigned long physical_file_sn){
    assert(file);
    file->physical_sn = physical_file_sn;
    return SUCCESS;
}

/*
 *  file_set_logical_flag - Set the File object to be a logical file
 *
 *  @file - Pointer to the file object
 */
ErrorCode file_set_logical_flag(File file){
    file->flag = 'L';
    return SUCCESS;
}


/*
 *  file_add_block - Add block to the file object that contains it
 *
 *  @file       - Pointer to the file object
 *  @block_id   - hashed id of the block
 *  @block_size - size of the block
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

    if(res != LIST_SUCCESS){
        free(bi->id);
        free(bi);
        return OUT_OF_MEMORY;
    }

    (file->num_blocks)++;
    free(bi->id);
    free(bi);
    return SUCCESS;
}

/* ******************** END ******************** File STRUCT Functions ******************** END ********************* */
#endif //DEDUPLICATION_PROJECT_FILE_H
