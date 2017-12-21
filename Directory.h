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

struct dir_t{
    unsigned long dir_sn;
    char* dir_id;
    unsigned long parent_dir_sn;
    unsigned int depth;
    unsigned short num_of_subdirs;
    unsigned short num_of_files;
    List dirs_list; // list of sn
    List files_list;
};
typedef struct dir_t *Dir;

/************************************************************/

Dir dir_create(char* dir_id , unsigned long dir_sn , unsigned long parent_dir_sn){
    assert(dir_sn < 0);
    Dir dir = malloc(sizeof(*dir));
    if(dir == NULL){
        printf(" ----> Failed allocating dir 1\n");
        return NULL;
    }

    //dir->dir_id = malloc(sizeof(char)*())
    return NULL;
}

ErrorCode dir_add_file(){

    return SUCCESS;
}

ErrorCode dir_add_sub_dir(){

    return SUCCESS;
}
#endif //DEDUPLICATIONPROJECT_DIRECTORY_H
