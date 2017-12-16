
/***************************************************** INCLUDES *******************************************************/
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
#define FILE_ID_LEN 18
#define BUFFER_SIZE 255
#define LETTERS_CHAR 48
#define LINE_SPACE 10
#define CHUNKE_ID_LEN 10
#define CHUNKE_SIZE_LEN 6

/************************************************** Helper Functions **************************************************/
/* Compare between current buffer and string of "Z"*/
bool check_12_z(char buff[STR_OF_Z]){
    for (int i = 0; i < STR_OF_Z ; ++i) {
        if(buff[i] != 'z'){
            return false;
        }
    }
    return true;
}

/********************************* Parsing Functions ************************************/
/*
 *  Extract fields from input file.
 *     - Get value from each type needed:
 *      --> Create correspond structure
 *      --> Insert value into the struct
 *
 *  @ return final struct for line was parsered
 *
 * */

/* DIRECTORY NAME */
void case_1_directory_name(FILE *res_file , char buff[BUFFER_SIZE]){
    char dir_name_hash[DIR_NAME_LEN];
    strncpy(dir_name_hash , buff , DIR_NAME_HASH);
    dir_name_hash[DIR_NAME_HASH] = '\0';
    fprintf(res_file , "-->File name is: %s \n" , dir_name_hash);
    return;
}

/* NAMESPACE DEPTH */
void case_4_get_depth(FILE *res_file , char buff[BUFFER_SIZE]){
    int namespace_depth = (int)strtol(buff,(char **)NULL, 10);
    fprintf(res_file , "--> Namespace depth is : %d \n" , namespace_depth);
    return;
}

/* FILE SIZE */
void case_5_file_size(FILE *res_file , char buff[BUFFER_SIZE]){
    int file_size = (int)strtol(buff,(char **)NULL, 10);
    fprintf(res_file , "--> File size is : %d \n" , file_size);
    return;
}

/* FILE ATTRIBUTES VALUE */
void case_6_file_attribute(FILE *res_file , char buff[BUFFER_SIZE]){
    int file_attribute = (int)strtol(buff,(char **)NULL, 10);
    fprintf(res_file , "--> File attribute is : %d \n" , file_attribute);
    return;
}

/* FILE ID */
void case_7_hash_file_id(FILE* res_file , char buff[BUFFER_SIZE], int ind_num_of_file){
    char file_id[FILE_ID_LEN]; // The value is 15 chars + 2 chars for index +1 for eol (end of line)
    //only first 15 digits depict the hashed directory name
    strncpy(file_id , buff , (FILE_ID_LEN - 3));
    file_id[(FILE_ID_LEN - 3)] = '_';
    file_id[(FILE_ID_LEN - 2)] = (LETTERS_CHAR + ind_num_of_file);
    file_id[(FILE_ID_LEN - 1)] = '\0';
    fprintf(res_file , "-->File id is: %s \n" , file_id);
    return;
}

/* Line 13 is SV */
void case_13_VS(FILE* res_file , FILE *input_file , char buff[BUFFER_SIZE] , int* block_line_count , bool* read_empty_line_chucnks) {
    /* Params initialization */
    *read_empty_line_chucnks = false;
    char block_id[BLOCK_ID_LEN];
    int block_size = 0;

    fgets(buff, BUFFER_SIZE, input_file);
    (*block_line_count)++;
    //check next line
    switch ((int) (buff[0])) {
        case 'V':
            fgets(buff, BUFFER_SIZE, input_file);
            (*block_line_count)++;
            /* Check if this line is empty or data chunk */
            if ((int) (buff[0]) == LINE_SPACE) {
                *read_empty_line_chucnks = true;
            }
            break;
            /* Empty line */
        case LINE_SPACE :
            *read_empty_line_chucnks = true;
            break;
            /* Data chunk */
        default:
            break;
    }
    if (*read_empty_line_chucnks == true) {
        return;
    }
    /* Read all data chunks */
    if ((int) (buff[0]) != CHUNKE_ID_LEN) {
        //we already have one chunk in the buffer
        do {
            char size[CHUNKE_SIZE_LEN];
            fputs(buff, res_file);

            if (check_12_z(buff) == true) {
                //only first 12 digits are block_id
                strncpy(block_id, buff, STR_OF_Z);
                block_id[STR_OF_Z] = '\0';

                strncpy(size, &buff[(STR_OF_Z + 1)], CHUNKE_SIZE_LEN);
            } else {
                //only first 10 digits are block_id
                strncpy(block_id, buff, 10);
                block_id[CHUNKE_ID_LEN] = '\0';

                strncpy(size, &buff[(CHUNKE_ID_LEN + 1)], CHUNKE_SIZE_LEN);
            }

            block_size = (int)strtol(size,(char **)NULL, 10);
            fprintf(res_file, "--> Block  - %s - %d \n", block_id, block_size);
            fgets(buff, BUFFER_SIZE, input_file);
            (*block_line_count)++;
        } while (strlen(buff) > 1);
    }
    return;
}

/******************************************************* MAIN *********************************************************/
int main(){
    /* Declerations & Initialization */
    int num_of_input_files = 1;
    FILE *input_file , *res_file_1;
    char buff[BUFFER_SIZE];
    bool read_empty_line_chucnks = false;
    int block_line_count = 0;

    /* Go Over each file, parsing the data into correspond structures */
    /* -------------------- Get File Names To Process -------------------- */
    /*printf("How many files would you like to process?\n");
    scanf("%d" , &num_of_input_files);
    if(num_of_input_files < 1){ //check that at least one file will be processed
        printf("You have requested an illegal number of files to process - The program will end now.");
        return 0;
    }
    printf("Please insert the path to the directory containing the imput files: \n");
    scanf("%s" , input_dir_start);

    printf("Now we will insert the file names to be processed:\n");
    for (int i = 0; i <num_of_input_files ; ++i) {
        printf("Please insert the name of file %d : \n",(i+1));
        scanf("%s" , file_names[i]);
    }*/

    /* -------------------- File Opening -------------------- */
    printf(" --- Opening File --- \n");
    // Line for CS server
    // Input_file = fopen("//home//mihuahams//project_files//input_example.txt" , "r");
    input_file = fopen("C:\\Users\\mihush\\Documents\\Technion\\Sem_7\\Gala - project\\input_example.txt" , "r");
    res_file_1 = fopen("C:\\Users\\mihush\\Documents\\GitHub\\DeduplicationProject\\DeduplicationProject\\res_file_1.txt" , "w");
    if(input_file == NULL){ //check the file was opened successfully - if not terminate
        printf(" --- Can't open input file --- \n");
        return 0;
    }
    printf("---------------------------------------------------------------------------------------------------------");

    /* -------------------- File Reading -------------------- */
    /* Go over all file systems */
    for (int i = 0; i < num_of_input_files ; ++i) {
        printf(" --- Start Reading the file --- \n");
        /* Skip till the first empty line - over the file system description */
        do{
            fgets(buff, BUFFER_SIZE , input_file);
        } while(strlen(buff) > 1);
        printf(" --- Skipped over the file-system data block - lets start reading the real data --- \n");

        /* Read File till the end - parse each block and add it to the corresponding structure */
        while(!feof(input_file)){
            fgets(buff, BUFFER_SIZE , input_file);
            block_line_count++;
            /* Check if we have reached the end of the file, nothing more to read */
            if(strcmp(buff , "LOGCOMPLETE\n") == 0){
                /* Read the last line before EOF */
                fgets(buff, BUFFER_SIZE , input_file);
            }
            /* We haven't seen the LOGCOMPLETE line yet */
            /* Check if we haven't reached the end of the current input block */
            while (strlen(buff) > 1 && !feof(input_file)){
                fputs(buff , res_file_1);
                switch(block_line_count){
                    /* DIRECTORY NAME */
                    case 1:
                        //only first 10 digits depict the hashed directory name
                        case_1_directory_name(res_file_1 , buff);
                        break;
                    /* NAMESPACE DEPTH */
                    case 4:
                        case_4_get_depth(res_file_1 , buff);
                        break;
                    /* FILE SIZE */
                    case 5:
                        case_5_file_size(res_file_1 , buff);
                        break;
                    /* FILE ATTRIBUTES VALUE */
                    case 6:
                        case_6_file_attribute(res_file_1 , buff);
                        break;
                    /* FILE ID */
                    case 7:
                        case_7_hash_file_id(res_file_1 , buff , i);
                        break;
                    /* Line 13 is SV */
                    case 13:
                        case_13_VS(res_file_1 , input_file , buff , &block_line_count , &read_empty_line_chucnks);
                        break;
                    default:
                        break;
                }
                /* Find data chunks */
                if(read_empty_line_chucnks == false){
                    fgets(buff, BUFFER_SIZE , input_file); //read next line in current block
                    block_line_count++;
                }
            }
            /*** WE HAVE REACHED THE END OF THE CURRENT INPUT BLOCK !!! ***/
            if(!feof(input_file)){
                fputs("-------> finished Reading a block\n\n" , res_file_1);
                /* Zero the line count for the next block */
                block_line_count = 0;
                read_empty_line_chucnks = false;
            }
        }
    }

    printf(" --- Finished reading the input file - Now lets start processing ---\n");

    /* -------------------- File Closing -------------------- */
    fclose(input_file);
    fclose(res_file_1);

    return 0;
}
