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

/* *************** START ************** Directory STRUCT Definition *************** START *************** */
/*
 * Definition of a Directory structure:
 *                  - dir_sn -> a running index on all directories read from the file system
 *                  - dir_id -> a hushed id as appears in the input file
 *                  - parent_dir_sn -> the sn of the parent directory in the hierarchy
 *                  - dir_depth -> the depth of the directory in the hierarchical tree
 *                  - num_of_subdirs -> number of sub directories
 *                  - num_of_files -> number of files contained in the directory
 *                  - dirs_list -> list of directory sn contained in this directory
 *                  - files_list -> list of file sn contained in this directory
 */
struct dir_t{
    unsigned long dir_sn;
    char* dir_id;
    unsigned long parent_dir_sn;
    int dir_depth;
    unsigned short num_of_subdirs;
    unsigned short num_of_files;
    List dirs_list; // list of serial numbers
    List files_list; //list of serial numbers
};
typedef struct dir_t *Dir;


static ListElement copy_directory_info(ListElement directory_info){
    assert(directory_info);
    unsigned long* sn = (unsigned long*)(directory_info);
    unsigned long* sn_copy = malloc(sizeof(*sn_copy));
    if(sn_copy == NULL){
        printf("---> allocation failed at list_element copy_func\n");
        return NULL;
    }
    *sn_copy = *sn;
    return sn_copy;
}

static  void free_dir_info(ListElement dir_info){
    free(dir_info);
}


/* **************** END *************** Directory STRUCT Definition **************** END **************** */
/* ****************************************************************************************************** */
/* ****************************************************************************************************** */
/* *************** START *************** Directory STRUCT Functions *************** START *************** */

/*
 * dir_create - Creates a new Directory object with:
 *                      - dir_id
 *                      - dir_sn
 *                      - depth
 */
Dir dir_create(char* dir_id , unsigned int depth , unsigned long dir_sn){
    assert(dir_sn >= 0);
    Dir dir = malloc(sizeof(*dir));
    if(dir == NULL){
        printf("(Directory)--> Creating Directory - Allocation Error (1) \n");
        return NULL;
    }
    dir->dir_id = malloc((sizeof(char)*DIR_NAME_LEN));
    if(!(dir->dir_id)){
        printf("(Directory)--> Creating Directory - Allocation Error (2) \n");
        free(dir);
        return NULL;
    }
    dir->dir_id = strcpy(dir->dir_id , dir_id);
    dir->dir_depth = depth;
    dir->dir_sn = dir_sn;
    dir->num_of_files = 0;
    dir->num_of_subdirs = 0;
    dir->parent_dir_sn = 0; //  not known in the time of creation
    dir->dirs_list = listCreate(copy_directory_info , free_dir_info);
    dir->files_list = listCreate(copy_directory_info , free_dir_info);

    if((!dir->files_list) || (!dir->dirs_list)){
        printf("(Directory)--> Creating Directory - Allocation Error (3) \n");
        free(dir->dir_id);
        free(dir);
        return NULL;
    }

//    printf("(Directory)--> Created Directory Successfully:\n");
//    printf("              - SN    : %lu \n" , dir->dir_sn);
//    printf("              - ID    : %s \n" , dir->dir_id);
//    printf("              - Depth : %d \n" , dir->dir_depth);
    return dir;
}
ErrorCode dir_set_parent_dir_sn(Dir dir , unsigned long sn){
    assert(dir);
    dir->parent_dir_sn = sn;
    return SUCCESS;
}
/*
 * dir_destroy - Destroy struct of Directory
 */
void dir_destroy(Dir dir){
    assert(dir);
    free(dir->dir_id);
    listDestroy(dir->dirs_list);
    listDestroy(dir->files_list);
    free(dir);
}

/*
 * dir_get_SN - Return the sn of directory
 */
unsigned long dir_get_SN(Dir dir){
    assert(dir);
    return dir->dir_sn;
}

/*
 * dir_get_ID - Return the ID of directory
 */
char* dir_get_ID(Dir dir){
    assert(dir);
    return dir->dir_id;
}

/*
 * dir_get_depth - Return the depth of the directory
 */
unsigned int dir_get_depth(Dir dir){
    assert(dir);
    return dir->dir_depth;
}

/* Adding file into the directory */
ErrorCode dir_add_file(Dir dir , unsigned long file_sn){
    if(dir == NULL || file_sn < 0){
        return INVALID_INPUT;
    }
    unsigned long* temp = malloc(sizeof(*temp));
    if(temp == NULL){
        printf("(Directory)--> Adding file to Directory - Allocation Error (1) \n");
        return OUT_OF_MEMORY;
    }
    *temp = file_sn;
    ListResult res = listInsertFirst(dir->files_list , temp);
    if(res != LIST_SUCCESS){
        free(temp);
        printf("(Directory)--> Adding file to Directory - Allocation Error (2) \n");
        return OUT_OF_MEMORY;

    }
    (dir->num_of_files)++;
//    printf("(Directory)--> File was added to Directory Successfully:\n");
//    printf("            - File  SN     : %lu \n" , file_sn);
//    printf("            - Directory SN : %lu \n" , dir->dir_sn);
    return SUCCESS;
}

/* Adding sub_dir into the directory */
ErrorCode dir_add_sub_dir(Dir dir , unsigned long dir_sn){
    if(dir == NULL || dir_sn < 0){
        return INVALID_INPUT;
    }
    unsigned long* temp = malloc(sizeof(*temp));
    if(temp == NULL){
        printf("(Directory)--> Adding sub directory to Directory - Allocation Error (1) \n");
        return OUT_OF_MEMORY;
    }
    *temp = dir_sn;
    ListResult res = listInsertFirst(dir->dirs_list, temp);
    if(res != LIST_SUCCESS){
        free(temp);
        printf("(Directory)--> Adding sub directory to Directory - Allocation Error (2) \n");
        return OUT_OF_MEMORY;
    }
    (dir->num_of_subdirs)++;
//    printf("(Directory)--> Directory was added to Parent Directory Successfully:\n");
//    printf("            - Directory  SN : %lu \n" , dir_sn);
//    printf("            - Parent Dir SN : %lu \n" , dir->dir_sn);
    return SUCCESS;
}
/* **************** END **************** Directory STRUCT Functions **************** END **************** */

#endif //DEDUPLICATIONPROJECT_DIRECTORY_H
