//
// Created by Polina on 14-Dec-17.
//

#ifndef DEDUPLICATIONPROJECT_UTILITIES_H
#define DEDUPLICATIONPROJECT_UTILITIES_H

typedef enum{
    SUCCESS,
    INVALID_INPUT,
    OUT_OF_MEMORY
} ErrorCode;

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/********************************************* Definitions& Magic Numbers *********************************************/
/* Magic Numbers */
#define STR_OF_Z 12
#define DIR_NAME_LEN 11
#define DIR_NAME_HASH 10
#define BLOCK_ID_LEN 13
#define FILE_ID_LEN 22
#define BUFFER_SIZE 255
#define LETTERS_CHAR 48
#define LINE_SPACE 10
#define CHUNKE_ID_LEN 10
#define CHUNKE_SIZE_LEN 6
#define DIR_SIZE 0


/* **************** START **************** object_info struct **************** START **************** */
/*
 * Definition of a object_info structure:
 *
 *
 *
 *
 *
 *
 */
struct object_info{ //helper struct
    char* object_id;
    unsigned long object_sn;
    char* parent_dir_id;
    char object_type; //can be either 'F' or 'D'
};
typedef struct object_info* Object_Info;


Object_Info object_info_create(char* id , unsigned long sn , char* parent_id , char type){
    Object_Info oi = malloc(sizeof(*oi));
    if(oi == NULL){
        return NULL;
    }
    //Set numeric Values
    oi->object_sn = sn;
    oi->object_type = type;

    //Set object_id
    oi->object_id = malloc(sizeof(char)*(strlen(id) +1));
    if(oi->object_id == NULL){
        free(oi);
        return NULL;
    }
    strcpy(oi->object_id  , id);

    //Set parent_dir_id
    oi->parent_dir_id = malloc(sizeof(char)*(strlen(parent_id) +1));
    if(oi->object_id == NULL){
        free(oi->parent_dir_id);
        free(oi);
        return NULL;
    }
    strcpy(oi->parent_dir_id  , parent_id);

    return oi;
}


static ListElement object_info_copy(ListElement object_info){
    assert(object_info);
    Object_Info oi = (Object_Info)(object_info);
    Object_Info oi_copy = malloc(sizeof(*oi_copy));
    if(oi_copy == NULL){
        return NULL;
    }
    //Copy numeric Values
    oi_copy->object_sn = oi->object_sn;
    oi_copy->object_type = oi->object_type;

    //copy object_id
    oi_copy->object_id = malloc(sizeof(char)*(strlen(oi->object_id) +1));
    if(oi_copy->object_id == NULL){
        free(oi_copy);
        return NULL;
    }
    strcpy(oi_copy->object_id  , oi->object_id);

    //copy parent_dir_id
    oi_copy->parent_dir_id = malloc(sizeof(char)*(strlen(oi->parent_dir_id) +1));
    if(oi_copy->object_id == NULL){
        free(oi_copy->parent_dir_id);
        free(oi_copy);
        return NULL;
    }
    strcpy(oi_copy->parent_dir_id  , oi->parent_dir_id);

    return oi_copy;
}

static void object_info_destroy(ListElement object_info){
    free(((Object_Info)(object_info))->object_id);
    free(((Object_Info)(object_info))->parent_dir_id);
    free(object_info);
}


/* ***************** END ***************** object_info struct ***************** END ***************** */

#endif //DEDUPLICATIONPROJECT_UTILITIES_H
