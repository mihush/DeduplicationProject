
/***************************************************** INCLUDES *******************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "HashTable.h"
#include <windows.h>

/************************************************** Global Params**************************************************/
/* Serial number for counting the elements which insert to the system */
unsigned long blocks_sn = 1 , files_sn = 1 , dir_sn = 1;

/* Hash-Tables for blocks, files , directories */
HashTable ht_files , ht_blocks , ht_dirs;

/* Root Directory */
Dir root_directory;

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
char* case_1_directory_name(FILE *res_file , char buff[BUFFER_SIZE]){
    char* dir_name_hash = malloc(sizeof(char)*DIR_NAME_LEN);
    if(!dir_name_hash){
        printf("---> string allocation Failure\n ");
        return NULL;
    }
    strncpy(dir_name_hash , buff , DIR_NAME_HASH);
    dir_name_hash[DIR_NAME_HASH] = '\0';
    fprintf(res_file , "--> Dir name is: %s \n" , dir_name_hash);
    return dir_name_hash;
}

/* NAMESPACE DEPTH */
unsigned short case_4_get_depth(FILE *res_file , char buff[BUFFER_SIZE]){
    unsigned short namespace_depth = (unsigned short)strtol(buff,(char**)NULL, 10);
    fprintf(res_file , "--> Namespace depth is : %d \n" , namespace_depth);
    return namespace_depth ;
}

/* FILE SIZE */
unsigned int case_5_file_size(FILE *res_file , char buff[BUFFER_SIZE]){
    unsigned int file_size = (unsigned int)strtol(buff,(char **)NULL, 10);
    fprintf(res_file , "--> File size is : %d \n" , file_size);
    return file_size;
}


/* FILE ATTRIBUTES VALUE */
/*
 * Returns one from { 'D' , 'F'}
 *  @ 'D' - for directory
 *  @ 'F' - for file
 *
 */
char case_6_file_attribute(FILE *res_file , char buff[BUFFER_SIZE]){
    int file_attribute = (int)strtol(buff,(char **)NULL, 10);
    fprintf(res_file , "--> File attribute is : %d \n" , file_attribute);

    char res;
    if ( FILE_ATTRIBUTE_ARCHIVE & file_attribute){
        fprintf(res_file , "---> The object is a File \n"  );
        res = 'F';
    }else if((FILE_ATTRIBUTE_DIRECTORY & file_attribute)){
        fprintf(res_file , "---> The object is a Directory\n");
        res = 'D';
    }
    return res;
}

/* FILE ID */
char* case_7_hash_file_id(FILE* res_file , char buff[BUFFER_SIZE], int ind_num_of_file){
    char* file_id = malloc(sizeof(char)*FILE_ID_LEN); // The value is 15 chars + 2 chars for index +1 for eol (end of line)
    if(!file_id){
        printf("---> string allocation Failure\n ");
        return NULL;
    }
    //only first 15 digits depict the hashed directory name
    strncpy(file_id , buff , (FILE_ID_LEN - 3));
    file_id[(FILE_ID_LEN - 3)] = '_';
    file_id[(FILE_ID_LEN - 2)] = (LETTERS_CHAR + ind_num_of_file);
    file_id[(FILE_ID_LEN - 1)] = '\0';
    fprintf(res_file , "-->File id is: %s \n" , file_id);
    return file_id;
}

/* Line 13 is SV */
void case_13_VS(File file_obj , FILE* res_file , FILE *input_file , char buff[BUFFER_SIZE] , int* block_line_count , bool* read_empty_line_chucnks) {
    /* Params initialization */
    *read_empty_line_chucnks = false;
    char block_id[BLOCK_ID_LEN];
    int block_size = 0;
    Block insert_block;

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

            file_add_block(file_obj , block_id , block_size);
            ht_set(ht_blocks , block_id , 1 , blocks_sn , block_size , 1, 'B');
            blocks_sn++;
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

    ht_files = ht_create();
    ht_blocks = ht_create();
    ht_dirs = ht_create();
    if(ht_files == NULL || ht_blocks == NULL || ht_dirs == NULL){
        printf(" ---> Failed Allocating Hashtables in parser =[ \n");
        return 0;
    }

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
    // Input_file = fopen("//home//mihuahams//project_files//input_example.txt" , "r");
    input_file = fopen("C:\\Polina\\Technion\\Semester7\\Dedup Project\\Project_Files\\DeduplicationProject\\input_example.txt" , "r");
    res_file_1 = fopen("C:\\Polina\\Technion\\Semester7\\Dedup Project\\Project_Files\\DeduplicationProject\\res_file_1.txt" , "w");
    if(input_file == NULL){ //check the file was opened successfully - if not terminate
        printf(" ---> Can't open input file/s =[ \n");
        return 0;
    }

    /* -------------------- File Reading -------------------- */
    /* Define params for reading data */
    char* parent_dir_id;
    unsigned short depth = 0;
    unsigned int file_size = 0;
    char obj_type;
    char* object_id;
    File file_obj;
    Dir dir_obj;

    /* Go over all file systems */
    for (int i = 0; i < num_of_input_files ; ++i) {
        printf(" --- Start Reading the file --- \n");
        /* Skip till the first empty line - over the file system description */
        do{
            fgets(buff, BUFFER_SIZE , input_file);
        } while(strlen(buff) > 1);
        printf(" --- Skipped over the file-system data block successfully--- \n");

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
                        parent_dir_id = case_1_directory_name(res_file_1 , buff);
                        break;
                    /* NAMESPACE DEPTH */
                    case 4:
                        depth = case_4_get_depth(res_file_1 , buff);
                        break;
                    /* FILE SIZE */
                    case 5:
                        file_size = case_5_file_size(res_file_1 , buff);
                        break;
                    /* FILE ATTRIBUTES VALUE */
                    case 6:
                        obj_type = case_6_file_attribute(res_file_1 , buff);
                        break;
                    /* FILE ID */

                    case 7:
                        object_id = case_7_hash_file_id(res_file_1 , buff , i);
                        //Case adding into Files- HashT
                        if (obj_type == 'F'){
                            file_obj = ht_set(ht_files , object_id , depth ,files_sn , 1, 507 ,'F');//TODO Find the current dir_sn from hash_t
                            printf("Created file with:\n");
                            printf("file id - %s\n" , file_obj->file_id);
                            printf("file sn - %lu\n" , file_obj->file_sn);
                            files_sn++;
                        }
                        //Case adding into Directory - HashT
                        else if(obj_type == 'D'){
                            if( dir_sn == 1){
                                root_directory = ht_set(ht_dirs , parent_dir_id, 0 , dir_sn , 1 , 0 , 'D');
                                printf("Root Dir created\n");
                                printf("dir id: %s\n" , root_directory->dir_id);
                                printf("dir sn: %lu\n", root_directory->dir_sn);
                                dir_sn++;
                             }
                            Dir temp_dir = (Dir)ht_get(ht_dirs, parent_dir_id);
                            if (temp_dir == NULL) {
                                printf("Parent_dir error !\n");
                            }
                            unsigned int parent_dir_sn = temp_dir->dir_sn;
                            dir_obj = ht_set(ht_dirs, object_id, depth, dir_sn, 1, parent_dir_sn, 'D');
                            printf("Created dir with:\n");
                            printf("dir id: %s\n", dir_obj->dir_id);
                            printf("parent dir_sn: %lu\n" , dir_obj->parent_dir_sn);
                            printf("dir sn: %lu\n", dir_obj->dir_sn);
                            dir_sn++;
                        }
                        break;
                    /* Line 13 is SV */
                    case 13:
                        case_13_VS(file_obj , res_file_1 , input_file , buff , &block_line_count , &read_empty_line_chucnks);
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
                file_obj = NULL;
            }
        }
    }

    printf(" --- Finished reading the input file - Now lets start processing ---\n");

    /* -------------------- File Closing -------------------- */
    fclose(input_file);
    fclose(res_file_1);

    printf(" -------------- Print File Hash Table :\n");
    print_ht_File(ht_files);

    return 0;
}
