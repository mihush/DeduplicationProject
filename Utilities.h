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

/********************************************* Definitions& Magic Numbers *********************************************/
/* Magic Numbers */
#define STR_OF_Z 12
#define DIR_NAME_LEN 11
#define DIR_NAME_HASH 10
#define BLOCK_ID_LEN 13
#define FILE_ID_LEN 18
#define BUFFER_SIZE 255
#define LETTERS_CHAR 48
#define LINE_SPACE 10
#define CHUNKE_ID_LEN 10
#define CHUNKE_SIZE_LEN 6

#endif //DEDUPLICATIONPROJECT_UTILITIES_H
