//
// Created by Polina on 21-Dec-17.
//

#ifndef DEDUPLICATIONPROJECT_DIRECTORY_H
#define DEDUPLICATIONPROJECT_DIRECTORY_H

#include "List.h"
#include "Utilities.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* *************** START ************** File STRUCT Definition *************** START *************** */

struct dir_t{
    unsigned long dir_sn;
    char* dir_id;
    unsigned long parent_dir_sn;
    unsigned int dir_depth;
    unsigned short num_of_subdirs;
    unsigned short num_of_files;
    List dirs_list; // list of serial numbers
    List files_list; //list of serial numbers
};
typedef struct dir_t *Dir;

/********************************* List Functions ******************************** */
static ListElement copy_directory_info(ListElement directory_info){
    assert(directory_info);
    unsigned long* sn = (unsigned long*)(directory_info);
    unsigned long* sn_copy = malloc(sizeof(*sn_copy));
    if(!sn_copy){
        printf("---> allocation faild at list_element copy_func\n");
        return NULL;
    }
    *sn_copy = *sn;
    return sn_copy;
}

static  void free_dir_info(ListElement dir_info){
    free(dir_info);
}

/******************************* Function of Directory Struct *****************************/
/* Creating a Directory struct */
Dir dir_create(char* dir_id , unsigned int depth , unsigned long dir_sn , unsigned long parent_dir_sn){
    assert(dir_sn >= 0);
    Dir dir = malloc(sizeof(*dir));
    if(dir == NULL){
        printf(" ----> Failed allocating dir 1\n");
        return NULL;
    }
    dir->dir_id = malloc((sizeof(char)*DIR_NAME_LEN));
    if(!(dir->dir_id)){
        printf(" ----> Failed allocating dir_id\n");
        free(dir);
        return NULL;
    }
    dir->dir_id = strcpy(dir->dir_id , dir_id);
    dir->dir_depth = depth;
    dir->dir_sn = dir_sn;
    dir->num_of_files = 0;
    dir->num_of_subdirs = 0;
    dir->parent_dir_sn = parent_dir_sn;
    dir->dirs_list = listCreate(copy_directory_info , free_dir_info);
    dir->files_list = listCreate(copy_directory_info , free_dir_info);

    if((!dir->files_list) || (!dir->dirs_list)){
        printf(" ----> Failed allocating lists of directory\n");
        free(dir->dir_id);
        free(dir);
        return NULL;
    }
    return dir;
}

/* Destroy struct of Directory */
void dir_destroy(Dir dir){
    assert(dir);
    free(dir->dir_id);
    listDestroy(dir->dirs_list);
    listDestroy(dir->files_list);
    free(dir);
}

/* Return the sn of directory */
unsigned long dir_get_SN(Dir dir){
    assert(dir);
    return dir->dir_sn;
}

/* Return the ID of directory */
char* dir_get_ID(Dir dir){
    assert(dir);
    return dir->dir_id;
}

/* Return the depth of the directory*/
unsigned int dir_get_depth(Dir dir){
    assert(dir);
    return dir->dir_depth;
}

/* Adding file into the directory */
ErrorCode dir_add_file(Dir dir , unsigned int file_sn){
    if(dir != NULL && file_sn > 0){
        return INVALID_INPUT;
    }
    unsigned int* temp = malloc(sizeof(*temp));
    if(!temp){
        return OUT_OF_MEMORY;
    }
    *temp = file_sn;
    ListResult res = listInsertFirst(dir->files_list , temp);
    if(res != LIST_SUCCESS){
        free(temp);
        printf("--->Allocation error while insert a file into directory");
        return OUT_OF_MEMORY;

    }
    dir->num_of_files += 1;

    return SUCCESS;
}

/* Adding sub_dir into the directory */
ErrorCode dir_add_sub_dir(Dir dir , unsigned int dir_sn){
    if(dir != NULL && dir_sn > 0){
        return INVALID_INPUT;
    }
    unsigned int* temp = malloc(sizeof(*temp));
    if(!temp){
        return OUT_OF_MEMORY;
    }
    *temp = dir_sn;
    ListResult res = listInsertFirst(dir->files_list , temp);
    if(res != LIST_SUCCESS){
        free(temp);
        printf("--->Allocation error while insert a sub_sir into directory");
        return OUT_OF_MEMORY;
    }
    dir->num_of_subdirs += 1;

    return SUCCESS;
}
#endif //DEDUPLICATIONPROJECT_DIRECTORY_H
