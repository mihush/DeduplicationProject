//
// Created by Polina on 21-Dec-17.
//

#ifndef DEDUPLICATIONPROJECT_DIRECTORY_H
#define DEDUPLICATIONPROJECT_DIRECTORY_H

#include "List.h"
#include "Utilities.h"

/* ****************** START ****************** Directory STRUCT Definition ****************** START ***************** */
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

/*
 * copy_directory_info - returns pointer to a copy of the serial number of the directory received as input
 *
 * @directory_info - pointer to the serial number of the directory to be copied
 */
static ListElement copy_directory_info(ListElement directory_info){
    assert(directory_info);
    unsigned long* sn = (unsigned long*)(directory_info);
    unsigned long* sn_copy = malloc(sizeof(*sn_copy));
    if(sn_copy == NULL){
        return NULL;
    }
    *sn_copy = *sn;
    return sn_copy;
}

/*
 * free_dir_info - frees the allocated space to the serial number of a directory
 *
 * @directory_info - pointer to the serial number that should be freed
 */
static  void free_dir_info(ListElement directory_info){
    free(directory_info);
}


/* ******************* END ****************** Directory STRUCT Definition ****************** END ******************** */
/* ****************************************************************************************************************** */
/* ****************************************************************************************************************** */
/* ****************** START ****************** Directory STRUCT Functions ****************** START ****************** */

/*
 * dir_create - Creates a new Directory object with the parameters received as input
 *              Returns pointer to the new directory object created
 *
 *
 * @dir_id - hashed id of the directory
 * @depth  - the depth of the directory in the file system (Root is in depth 0)
 * @dir_sn - serial number of the directory
 */
Dir dir_create(char* dir_id , unsigned int depth , unsigned long dir_sn){
    Dir dir = malloc(sizeof(*dir));
    if(dir == NULL){
        return NULL;
    }
    dir->dir_id = malloc((sizeof(char)*(strlen(dir_id) + 1)));
    if(!(dir->dir_id)){
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
        free(dir->dir_id);
        free(dir);
        return NULL;
    }

    return dir;
}

/*
 * dir_set_parent_dir_sn - updates the parent serial number of the directory received as input
 *
 * @dir - the directory who's parent directory serial number we want to update
 * @sn  - the serial number of the parent directory
 */
ErrorCode dir_set_parent_dir_sn(Dir dir , unsigned long sn){
    assert(dir);
    dir->parent_dir_sn = sn;
    return SUCCESS;
}

/*
 * dir_destroy - Destroys struct of Directory
 *
 * @dir - Pointer to the directory structure that should be destroyed
 */
void dir_destroy(Dir dir){
    assert(dir);
    free(dir->dir_id);
    listDestroy(dir->dirs_list);
    listDestroy(dir->files_list);
    free(dir);
}

/*
 * dir_get_SN - Return the serial number of directory
 *
 * @dir - pointer to the directory
 */
unsigned long dir_get_SN(Dir dir){
    assert(dir);
    return dir->dir_sn;
}

/*
 * dir_get_ID - Return pointer to the ID of directory
 *
 * @dir - pointer to the directory
 */
char* dir_get_ID(Dir dir){
    assert(dir);
    return dir->dir_id;
}

/*
 * dir_get_depth - Return the depth of the directory
 *
 * @dir - pointer to the directory
 */
unsigned int dir_get_depth(Dir dir){
    assert(dir);
    return dir->dir_depth;
}

/*
 * dir_add_file - Adds file to a directory object by saving its' ID in the files list of the directory
 *                and updates the files counter of the folder
 *
 *  @dir     - pointer to the directory
 *  @file_sn - the serial number of the file that should be added to the directory
 */
ErrorCode dir_add_file(Dir dir , unsigned long file_sn){
    if(dir == NULL || file_sn < 0){
        return INVALID_INPUT;
    }
    unsigned long* temp = malloc(sizeof(*temp));
    if(temp == NULL){
        return OUT_OF_MEMORY;
    }

    *temp = file_sn;
    ListResult res = listInsertFirst(dir->files_list , temp);
    if(res != LIST_SUCCESS){
        free(temp);
        return OUT_OF_MEMORY;

    }
    (dir->num_of_files)++;
    free(temp); //TODO Check everything is OK
    return SUCCESS;
}

/* Adding sub_dir into the directory */
/*
 * dir_add_sub_dir - Adds a sub-directory to a directory object by saving its' ID in the sub-directories list of
 *                   the directory and updates the files counter of the folder
 *
 *  @dir    - pointer to the directory
 *  @dir_sn - the serial number of the sub-directory that should be added to the directory
 */
ErrorCode dir_add_sub_dir(Dir dir , unsigned long dir_sn){
    if(dir == NULL || dir_sn < 0){
        return INVALID_INPUT;
    }
    unsigned long* temp = malloc(sizeof(*temp));
    if(temp == NULL){
        return OUT_OF_MEMORY;
    }
    *temp = dir_sn;
    ListResult res = listInsertFirst(dir->dirs_list, temp);
    if(res != LIST_SUCCESS){
        free(temp);
        return OUT_OF_MEMORY;
    }
    (dir->num_of_subdirs)++;
    free(temp);
    return SUCCESS;
}

/* ******************* END ******************* Directory STRUCT Functions ******************* END ******************* */

#endif //DEDUPLICATIONPROJECT_DIRECTORY_H
