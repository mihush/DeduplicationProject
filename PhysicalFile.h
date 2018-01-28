//
// Created by Polina on 27-Jan-18.
//

#ifndef DEDUPLICATIONPROJECT_PHYSICALFILE_H
#define DEDUPLICATIONPROJECT_PHYSICALFILE_H

#include "List.h"
#include "HashTableF.h"
#include "Utilities.h"
#include "File.h"
#include <stdbool.h>

/* ********************************************************************************************************** */
/* ********************************************************************************************************** */
/* *************** START ************** Physical File STRUCT Definition *************** START *************** */
/*
 * Definition of a Physical File structure:
 */
struct physical_file_t{
    unsigned long physical_file_sn;
    char* file_id; // Set to be the id of the first file that created it
    int num_blocks;
    unsigned int file_size;
    unsigned int shared_by_num_files;
    HashTableF files_ht; // hashtable of files that contain this physical file
    List blocks_list; // list of blocks that the file contains
};
typedef struct physical_file_t *PhysicalFile;

/* **************** END *************** Physical File STRUCT Definition **************** END **************** */
/* ********************************************************************************************************** */
/* ********************************************************************************************************** */
/* *************** START ************** Physical File STRUCT Functions *************** START **************** */

/*
 * pfile_create - .....
 */
PhysicalFile pfile_create(char* file_id , unsigned long physical_file_sn){
    PhysicalFile  pfile = malloc(sizeof(*pfile));
    if(pfile == NULL){
        printf("(PFile)--> Creating Physical File - Allocation Error (1) \n");
        return NULL;
    }

    pfile->file_id = malloc(sizeof(char)* (FILE_ID_LEN + 1));
    if(pfile->file_id == NULL){
        printf("(File)--> Creating Physical File - Allocation Error (2) \n");
        free(pfile);
        return NULL;
    }

    pfile->file_id = strcpy(pfile->file_id , file_id);
    pfile->physical_file_sn = physical_file_sn;
    pfile->num_blocks = 0;
    pfile->file_size = 0;
    pfile->shared_by_num_files = 0;

    pfile->blocks_list = listCreate(copy_block_info , free_block_info);
    if(pfile->blocks_list == NULL){
        printf("(PFile)--> Creating Physical File - Allocation Error (3) \n");
        free(pfile->file_id);
        free(pfile);
        return NULL;
    }

    pfile->files_ht = ht_createF('P');
    if(block->files_ht == NULL){
        free(pfile->file_id);
        listDestroy(pfile->blocks_list);
        free(pfile);
        return NULL;
    }

    return pfile;
}
/*
 * pfile_destroy - .......
 */
void pfile_destroy(PhysicalFile pfile){
    assert(pfile);
    free(pfile->file_id);
    listDestroy(pfile->blocks_list);
    hashTableF_destroy(pfile->files_ht);
    free(pfile);

}

/*
 * pfile_get_size - ......
 */
unsigned int pfile_get_size(PhysicalFile pfile){
    assert(pfile);
    return pfile->file_size;
}

/*
 * pfile_get_num_blocks - .....
 */
int pfile_get_num_blocks(PhysicalFile pfile){
    assert(pfile);
    return pfile->num_blocks;
}




#endif //DEDUPLICATIONPROJECT_PHYSICALFILE_H
