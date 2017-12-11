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

typedef struct file_t *File;

/*
 *
 */
File file_create(long file_sn , char* file_id , short block_size , ErrorCode *status);


/*
 *
 */
void file_destroy(File file);

/*
 *
 */
long file_get_SN(File file);
char* file_get_ID(File file);

/*
 *
 */
ErrorCode file_add_block(File file , char* block_id , int block_size);

#endif //DEDUPLICATION_PROJECT_FILE_H
