//
// Created by Polina on 27-Jan-18.
//

#ifndef DEDUPLICATIONPROJECT_PHYSICALFILE_H
#define DEDUPLICATIONPROJECT_PHYSICALFILE_H

#include "List.h"
#include "HashTableF.h"

/*
 * Definition of a Physical File structure:
 */
struct physical_file_t{
    unsigned long physical_file_sn;
    char* file_id; // Set to be the id of the first file that created it
    int num_blocks;
    unsigned int file_size;
    unsigned int shared_by_num_files;
    HashTableF files_ht;
    List blocks_list;
};
typedef struct physical_file_t *PhysicalFile;



#endif //DEDUPLICATIONPROJECT_PHYSICALFILE_H
