/***************************************************** INCLUDES *******************************************************/
#include "HashTable.h"
#include "Utilities.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


/************************************************** Global Params *****************************************************/
/* Serial number for counting the elements which insert to the system */
unsigned long blocks_sn = 0 , files_sn = 0 , dir_sn = 0;

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

/************************************************** Parsing Functions **************************************************/
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
        //printf("---> string allocation Failure\n ");
        return NULL;
    }
    strncpy(dir_name_hash , buff , DIR_NAME_HASH);
    dir_name_hash[DIR_NAME_HASH] = '\0';
    return dir_name_hash;
}

/* NAMESPACE DEPTH */
unsigned short case_4_get_depth(FILE *res_file , char buff[BUFFER_SIZE]){
    unsigned short namespace_depth = (unsigned short)strtol(buff,(char**)NULL, 10);
    return namespace_depth ;
}

/* FILE SIZE */
unsigned int case_5_file_size(FILE *res_file , char buff[BUFFER_SIZE]){
    unsigned int file_size = (unsigned int)strtol(buff,(char **)NULL, 10);
    return file_size;
}


/* FILE ATTRIBUTES VALUE */
/*
 * Returns one from { 'D' , 'F'}
 *  @ 'D' - for directory
 *  @ 'F' - for file
 */
char case_6_file_attribute(FILE *res_file , char buff[BUFFER_SIZE]){
    unsigned int file_attribute = (unsigned int)strtol(buff,(char **)NULL, 16);
    //printf("(Parser) --> File attribute is (HEX): %X \n" , file_attribute);
    //fprintf(res_file , "(Parser) --> File attribute is (HEX): %X \n" , file_attribute);
    char res;

    //Check for a directory, otherwise it is a file
    //The fifth bit should be set if this is a directory
    if((FILE_ATTRIBUTE_DIRECTORY & file_attribute) == FILE_ATTRIBUTE_DIRECTORY){
        //fprintf(res_file , "(Parser) --> The object is a Directory\n");
        res = 'D';
    } else{
        //fprintf(res_file, "(Parser) --> The object is a File \n");
        res = 'F';
    }
    return res;
}

/* FILE ID */
char* case_7_hash_file_id(FILE* res_file , char buff[BUFFER_SIZE], int ind_num_of_file){
    char* file_id = malloc(sizeof(char)*FILE_ID_LEN); // The value is 15 chars + 2 chars for index +1 for eol (end of line)
    if(file_id == NULL){
        return NULL;
    }
    //only first 15 digits depict the hashed directory name
    file_id[0] = (LETTERS_CHAR + ind_num_of_file);
    file_id[1] = '_';
    strncpy((file_id + 2) , buff , strlen(buff) - 1);
    file_id[strlen(buff) + 2] = '\0';
    //fprintf(res_file , "(Parser) --> BUFFER LEN IS : %d \n" , (int)strlen(buff));
    //printf("(Parser) --> BUFFER LEN IS : %d \n" , (int)strlen(buff));
    //fprintf(res_file , "(Parser) --> File id is: %s \n" , file_id);
    return file_id;
}

/* Line 13 is SV */
void case_13_VS(File file_obj , FILE* res_file , FILE *input_file , char buff[BUFFER_SIZE] , int* block_line_count , bool* read_empty_line_chucnks) {
    /* Params initialization */
    *read_empty_line_chucnks = false;
    char block_id[BLOCK_ID_LEN];
    unsigned int block_size = 0;

    fgets(buff, BUFFER_SIZE, input_file);
    (*block_line_count)++;

    switch ((int) (buff[0])) { //check next line
        case 'V':
            fgets(buff, BUFFER_SIZE, input_file);
            (*block_line_count)++;
            /* Check if this line is empty or data chunk */
            if ((int) (buff[0]) == LINE_SPACE) {
                *read_empty_line_chucnks = true;
            }
            //TODO Read all V lines
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
            Block new_block = ht_set(ht_blocks , block_id , 1 , blocks_sn , block_size , 'B');
            block_add_file(new_block , file_obj->file_id);
            blocks_sn++;
            //fprintf(res_file, "--> Block  - %s - %d \n", block_id, block_size);
            fgets(buff, BUFFER_SIZE, input_file);
            (*block_line_count)++;
        } while (strlen(buff) > 1);
    }
    return;
}

/*
 * update_parent_dir_sn
 */
void update_parent_dir_sn(FILE* res_file, List previous , List current , int global_depth){
    File temp_file = NULL;
    Dir temp_dir = NULL;
    // Object_Info temp_oi;
    //printf("(update_parent_dir_sn) -->  Updating Parent directory serial numbers in depth %d ..... \n" , global_depth);
    fprintf(res_file , "(update_parent_dir_sn) -->  Updating Parent directory serial numbers in depth %d ..... \n" , global_depth);
    if(global_depth == 0){ //We are at root Level directory just set everyone to be the children of root
        unsigned long root_sn = root_directory->dir_sn;
        //Set root to be its own child
        dir_set_parent_dir_sn(root_directory , root_sn);
        dir_add_sub_dir(root_directory , root_sn);

        Dir temp_dir_root = (Dir)(ht_get(ht_dirs , root_directory->dir_id));
        //printf("(update_parent_dir_sn) --> First Level - Root is parent %lu ..... \n" , root_sn);
        //fprintf(res_file , "(update_parent_dir_sn) --> First Level - Root is parent %lu ..... \n" , root_sn);
        LIST_FOREACH(Object_Info , iter ,current){
            if(iter->object_type == 'F'){
                temp_file = (File)(ht_get(ht_files , iter->object_id));
                assert(temp_file);
                file_set_parent_dir_sn(temp_file ,root_sn);
                dir_add_file(temp_dir_root,temp_file->file_sn);
            } else{
                temp_dir = (Dir)(ht_get(ht_dirs , iter->object_id));
                assert(temp_dir);
                dir_set_parent_dir_sn(temp_dir , root_sn);
                dir_add_sub_dir(temp_dir_root,temp_dir->dir_sn);
            }
        }
        //temp_oi = listGetFirst(current);//Just in order to reset the iterator of the current list to the beginning
    }else{ //Go over both lists and update accordingly
        //NOW Lets Update the parent SNs!!!!!!!!!!!!!!!!!!!!!!!!!!!
        //printf("(update_parent_dir_sn) --> Depth %d ..... \n", global_depth);
        //fprintf(res_file , "(update_parent_dir_sn) --> Depth %d ..... \n", global_depth);

        Object_Info prev_list_iterator = listGetFirst(previous);
        Object_Info curr_list_iterator = listGetFirst(current);
        int curr_level_objects_count = 0;
        int prev_list_size = listGetSize(previous);
        int curr_list_size = listGetSize(current);

        for(int i = 0 ; i < prev_list_size ; i++){ //iterate over Previous level list
            unsigned long current_sn_to_set = prev_list_iterator->object_sn;
            Dir parent_dir_object = (Dir)(ht_get(ht_dirs , prev_list_iterator->object_id));

            if(prev_list_iterator->object_type == 'F'){ //A File cant be Parent directory for anyone
                prev_list_iterator = listGetNext(previous); //advance to the next object in the previous level
                continue;
            }else{ //The object is a directory it can be a parent
                if(curr_level_objects_count >= curr_list_size){
                    prev_list_iterator = listGetNext(previous); //advance to the next object in the previous level
                    continue;
                }
                char* parent_id = curr_list_iterator->parent_dir_id;
                //now lets iterate over the current list while we have objects with the same parent id
                while((curr_list_iterator != NULL)&&(strcmp(parent_id , curr_list_iterator->parent_dir_id) == 0)){
                    if(curr_list_iterator->object_type == 'F'){
                        temp_file = (File)(ht_get(ht_files , curr_list_iterator->object_id));
                        assert(temp_file);
                        file_set_parent_dir_sn(temp_file ,current_sn_to_set);
                        //add to the prevDir object - dir_add_file
                        dir_add_file(parent_dir_object ,curr_list_iterator->object_sn);
                    } else{
                        temp_dir = (Dir)(ht_get(ht_dirs , curr_list_iterator->object_id));
                        assert(temp_dir);
                        dir_set_parent_dir_sn(temp_dir , current_sn_to_set);
                        //add to the prevDir object - dir_add_sub_dir
                        dir_add_sub_dir(parent_dir_object , curr_list_iterator->object_sn);
                    }
                    curr_list_iterator = listGetNext(current);//advance to the next object in the current level
                    curr_level_objects_count++;
                }
            }
            prev_list_iterator = listGetNext(previous); //advance to the next object in the previous level
        }
    }
}

void print_ht_to_CSV(char dedup_type){
    Entry pair = NULL;
    FILE *results_file = NULL;
    char* fileName = malloc(sizeof(char)*21);
    sprintf(fileName , "Parsing_Results.csv");
    results_file = fopen(fileName , "w+");

    if(dedup_type == 'B'){
        fprintf(results_file ,"# Output type: , block-level\n");
    } else {
        fprintf(results_file ,"# Output type: , file-level\n");
    }
    fprintf(results_file ,"# Input files: , input_example.txt\n"); //TODO save names of input files
    fprintf(results_file ,"# Num files: , %lu\n" , (files_sn-1));
    fprintf(results_file ,"# Num directories: , %lu\n" , (dir_sn-1));
    if(dedup_type == 'B'){
        fprintf(results_file ,"# Num Blocks:, %lu\n", (blocks_sn - 1));
    } else {
        //TODO change this to physical files
        fprintf(results_file ,"# Num physical files: , %lu\n", (blocks_sn - 1));
    }

    if(dedup_type == 'B'){ //Block level deduplication
        //Print Files - Logical
        for(int i = 0 ; i < (ht_files->size_table) ;i++){
            pair = ht_files->table[i];
            while( pair != NULL && pair->key != NULL) {
                File temp_file = ((File)(pair->data));
                fprintf(results_file , "F, %lu, %s, %lu, %d,",
                        temp_file->file_sn, temp_file->file_id , temp_file->dir_sn,
                        temp_file->num_blocks);
                //Object_Info temp_oi;
                LIST_FOREACH(Block_Info , iter ,temp_file->blocks_list){
                    unsigned long block_sn = ((Block)(ht_get(ht_blocks , iter->id)))->block_sn;
                    fprintf(results_file ,"%lu, %d," , block_sn , iter->size);
                }
                //temp_oi = listGetFirst(temp_file->blocks_list);
                fprintf(results_file ,"\n");
                pair = pair->next;
            }
        }
        //Print Blocks
        for(int i = 0 ; i < (ht_blocks->size_table) ;i++){
            pair = ht_blocks->table[i];
            while( pair != NULL && pair->key != NULL) {
                Block temp_block = ((Block)(pair->data));
                fprintf(results_file , "B, %lu, %s, %d,",
                        temp_block->block_sn , temp_block->block_id,
                        temp_block->shared_by_num_files);

                for(int j = 0 ; j < (temp_block->files_ht->size_table) ; j++){
                    EntryF pair_file_id = temp_block->files_ht->table[j];
                    while( pair_file_id != NULL && pair_file_id->key != NULL) {
                        unsigned long file_sn = ((File)(ht_get(ht_files , pair_file_id->key)))->file_sn;
                        fprintf(results_file ,"%lu," , file_sn);
                        pair_file_id = pair_file_id->next;
                    }
                }
                fprintf(results_file ,"\n");
                pair = pair->next;
            }
        }
    }else{
        printf("File Level Dedup\n");
    }

    //Print Directories
    for(int i = 0 ; i < (ht_dirs->size_table) ;i++){
        pair = ht_dirs->table[i];
        while( pair != NULL && pair->key != NULL) {
            Dir temp_dir = ((Dir)(pair->data));

            if(temp_dir->dir_depth == -1){
                fprintf(results_file , "R,");
            }else {
                fprintf(results_file , "D,");
            }
            fprintf(results_file , "%lu, %s, %lu, %d, %d," ,
                    temp_dir->dir_sn, temp_dir->dir_id, temp_dir->parent_dir_sn,
                    temp_dir->num_of_subdirs, temp_dir->num_of_files);
            LIST_FOREACH(unsigned long* , iter , temp_dir->dirs_list){
                fprintf(results_file ,"%lu," , *(iter));
            }
            LIST_FOREACH(unsigned long* , iter , temp_dir->files_list){
                fprintf(results_file ,"%lu," , *(iter));
            }
            fprintf(results_file , "\n");
            pair = pair->next;
        }
    }
    fclose(results_file);
    free(fileName);
}
/* ****************************************************** MAIN ******************************************************** */
int main(){
    /* ----------------------- Parameters Declarations & Initialization ----------------------- */
    /// File  Manipulation Variables
    int num_of_input_files = 1;
    char input_dir_start[255];
    char* current_file_to_process = NULL;
    FILE *input_file , *res_file_1;
    char buff[BUFFER_SIZE];
    bool read_empty_line_chucnks = false;
    int block_line_count = 0;

    /// Initialize Global Variables
    ht_files = ht_create('F');
    ht_blocks = ht_create('B');
    ht_dirs = ht_create('D');
    if(ht_files == NULL || ht_blocks == NULL || ht_dirs == NULL){
        printf("(Parser)--> Failed Allocating Hash Tables in parser =[ \n");
        return 0;
    }
    printf("\n\n\n");
    /// Define parameters for global data Manipulation (over entire input file)
    List curr_depth_objects , previous_depth_objects;
    int global_current_depth = 0 ;
    curr_depth_objects = listCreate(object_info_copy , object_info_destroy);
    previous_depth_objects = listCreate(object_info_copy , object_info_destroy);

    /// Define parameters for reading data regarding SINGLE OBJECT
    char* parent_dir_id = NULL; // Hashed ID of parent Directory
    unsigned short depth = 0; //Depth of current object in the hierarchy
    unsigned int file_size = 0; //File Size (if the object is a file)
    char obj_type = 'Z'; //Hexadecimal Value - Tells the type of the object ( A combination of binary flags set)
    char* object_id = NULL; //Hashed ID of the current object
    File file_obj = NULL; //Pointer to the FILE Object to be created from the current input
    Dir dir_obj = NULL; //Pointer to the DIRECTORY Object to be created from the current input
    bool is_zero_size_file = false;

    /* ----------------------- Parameters Declarations & Initialization ----------------------- */
    /* ---------------------------------------------------------------------------------------- */
    /* -------------------- File Manipulations - Getting Files to Process  -------------------- */
    /*
    printf("Please insert the path to the directory containing the input files: \n");
    scanf("%[^\n]s" , input_dir_start);

    printf("How many files would you like to process?\n");
    scanf("%d" , &num_of_input_files);
    if(num_of_input_files < 1){ //check that at least one file will be processed
        printf("You have requested an illegal number of files to process - The program will end now.");
        return 0;
    }
    char input_file_names[num_of_input_files][17];

    printf("Now we will insert the file names to be processed:\n");
    for (int i = 0; i < num_of_input_files ; ++i) {
        printf("Please insert the name of file %d : \n",(i+1));
        scanf("%s" , input_file_names[i]);
    }*/
    /* -------------------- File Manipulations - Getting Files to Process  -------------------- */
    /* ---------------------------------------------------------------------------------------- */
    /* ------------------------- File Manipulations  - opening files  ------------------------- */
    /* Go Over each file, parsing the data into correspond structures */
    /* -------------------- Get File Names To Process -------------------- */
    printf("(Parser)--> ----- Opening File ----- \n");
    num_of_input_files = 1;
    /* michal files addresses */
    //input_file = fopen("C:\\Users\\mihush\\Documents\\GitHub\\DeduplicationProject\\DeduplicationProject\\input_example.txt" , "r");
    //res_file_1 = fopen("C:\\Users\\mihush\\Documents\\GitHub\\DeduplicationProject\\DeduplicationProject\\res_file_1.txt" , "w");
    /* Polina files addresses */
    input_file = fopen("C:\\Polina\\Technion\\Semester7\\Dedup Project\\Project_Files\\DeduplicationProject\\0119.txt" , "r");
    res_file_1 = fopen("C:\\Polina\\Technion\\Semester7\\Dedup Project\\Project_Files\\DeduplicationProject\\res_file_1.txt" , "w");
    /* Server files addresses */
    //input_file = fopen("/home/polinam/03_01/input_example.txt" , "r");
    //res_file_1 = fopen("/home/polinam/08_01_18/res_file_1.txt" , "w");
    if(input_file == NULL){ //check the file was opened successfully - if not terminate
        printf("%s\n",current_file_to_process);
        printf("(Parser)--> Can't open input file/s =[ \n");
        return 0;
    }

    /* ------------------------- File Manipulations  - opening files  ------------------------- */
    /* ---------------------------------------------------------------------------------------- */
    /* ------------------------------------- File Reading ------------------------------------- */

    /* Go over all file systems */
    for (int i = 0; i < num_of_input_files ; ++i) {
        printf("(Parser)-->  ----- Start Reading the file ----- \n");
        /* Skip till the first empty line - over the file system description */
        do{
            fgets(buff, BUFFER_SIZE , input_file);
        } while(strlen(buff) > 1);
        printf("(Parser)--> --- Skipped over the file-system data block successfully--- \n");

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
            /***********************************************************************************************/
            while (strlen(buff) > 1 && !feof(input_file)){
                switch(block_line_count){
                    case 1: /* DIRECTORY NAME */
                        //only first 10 digits depict the hashed directory name
                        parent_dir_id = case_1_directory_name(res_file_1 , buff);
                        break;
                    case 4: /* NAMESPACE DEPTH */
                        depth = case_4_get_depth(res_file_1 , buff);
                        //Check if current depth (in variable depth) is bigger than the one in global_current_depth
                        if(depth > global_current_depth){
                            //This means we have reached a new depth and can update parent_dir_sn for objects from previous levels
                            update_parent_dir_sn(res_file_1 , previous_depth_objects , curr_depth_objects , global_current_depth);
                            //Update Object lists
                            listDestroy(previous_depth_objects); //Empty the previous_depth_objects list
                            previous_depth_objects = listCopy(curr_depth_objects);//Copy the curr_depth_objects list to the previous_depth_objects
                            listClear(curr_depth_objects); //Empty the curr_depth_objects list
                            global_current_depth = depth;
                        }
                        break;
                    case 5: /* FILE SIZE */
                        file_size = case_5_file_size(res_file_1 , buff);
                        break;
                    case 6: /* FILE ATTRIBUTES VALUE */
                        obj_type = case_6_file_attribute(res_file_1 , buff);
                        if( ( file_size == 0 ) && ( obj_type == 'F' ) ){ // ignore zero size files
                            is_zero_size_file = true;
                        }
                        break;
                    case 7: /* FILE ID */
                        object_id = case_7_hash_file_id(res_file_1 , buff , i);
                        //Adding File Object to HashTable
                        if ((obj_type == 'F') && (is_zero_size_file == false)){
                            file_obj = ht_set(ht_files , object_id , depth ,files_sn , file_size ,'F');
                            //add file to curr_depth_objects list in order to later find the parent directory
                            Object_Info oi_file = object_info_create(object_id , files_sn , parent_dir_id , 'F');
                            listInsertLast(curr_depth_objects , oi_file);
                            object_info_destroy(oi_file); //The list adds a copy of this object and it is no longer needed
                            files_sn++;
                        } //Adding Directory Object to HashTable
                        else if(obj_type == 'D'){
                            if( dir_sn == 0){ //Creating Dummy Root Node using the Parent_dir_id of the first object in the input file
                                root_directory = ht_set(ht_dirs , parent_dir_id , -1 , dir_sn ,DIR_SIZE , 'D' );
                                dir_sn++;
                            }

                            //Create Directory Object with the retrieved data
                            ht_set(ht_dirs, object_id, depth, dir_sn, DIR_SIZE , 'D');
                            //add file to curr_depth_objects list in order to later find the parent directory
                            Object_Info oi_dir = object_info_create(object_id , dir_sn , parent_dir_id , 'D');
                            listInsertLast(curr_depth_objects , oi_dir);
                            object_info_destroy(oi_dir); //The list adds a copy of this object and it is no longer needed
                            dir_sn++;
                        }
                        break;
                    case 13: /* Line 13 is SV */
                        case_13_VS(file_obj , res_file_1 , input_file , buff , &block_line_count , &read_empty_line_chucnks);
                        if (obj_type == 'F'){ //If there are no blocks in a non-zero size file
                            //remove from hash table

                            //decrease the file_sn counter
                            files_sn--;
                        }
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

            /***********************************************************************************************/
            /******************* WE HAVE REACHED THE END OF THE CURRENT INPUT OBJECT !!! ********************/
            if(!feof(input_file)){ //Update parameters that are relevant to a single object
                /* Zero the line count for the next block */
                block_line_count = 0;
                read_empty_line_chucnks = false;
                file_obj = NULL;
                //dir_obj = NULL;
                is_zero_size_file = false;
                free(parent_dir_id);
                free(object_id);
            }
        }
        free(current_file_to_process);
        fclose(input_file);
    }

    printf("(Parser) --> --- Finished reading the input file - Now lets start processing ---\n");
    //This means we have reached a new depth and can update parent_dir_sn for objects from previous levels
    update_parent_dir_sn(res_file_1 , previous_depth_objects , curr_depth_objects , global_current_depth);

    /* -------------------- File Closing -------------------- */

    fclose(res_file_1);


    //TODO Create CSV File For Results
    printf("(Parser) --> Printing Results ................\n");
    print_ht_to_CSV('B');

    //TODO Free All Hash tables and Lists
    hashTable_destroy(ht_files , 'F');
    hashTable_destroy(ht_dirs , 'D');
    hashTable_destroy(ht_blocks , 'B');
    listDestroy(curr_depth_objects);
    listDestroy(previous_depth_objects);

    //TODO Remove all irrelevant Printfs
    return 0;
}
