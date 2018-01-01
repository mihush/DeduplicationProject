/***************************************************** INCLUDES *******************************************************/

#include "HashTable.h"
#include <windows.h>
#include "Utilities.h"


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
        printf("---> string allocation Failure\n ");
        return NULL;
    }
    strncpy(dir_name_hash , buff , DIR_NAME_HASH);
    dir_name_hash[DIR_NAME_HASH] = '\0';
    fprintf(res_file , "--> Dir name is: %s \n" , dir_name_hash);
    printf("(Parser) --> Dir name is: %s \n" , dir_name_hash);
    return dir_name_hash;
}

/* NAMESPACE DEPTH */
unsigned short case_4_get_depth(FILE *res_file , char buff[BUFFER_SIZE]){
    unsigned short namespace_depth = (unsigned short)strtol(buff,(char**)NULL, 10);
    fprintf(res_file , "--> Namespace depth is : %d \n" , namespace_depth);
    printf("(Parser) --> Namespace depth is : %d \n" , namespace_depth);
    return namespace_depth ;
}

/* FILE SIZE */
unsigned int case_5_file_size(FILE *res_file , char buff[BUFFER_SIZE]){
    unsigned int file_size = (unsigned int)strtol(buff,(char **)NULL, 10);
    fprintf(res_file , "--> File size is : %d \n" , file_size);
    printf("(Parser) --> File size is : %d \n" , file_size);
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
    fprintf(res_file , "--> File attribute is : %d \n" , file_attribute);
    printf("(Parser) --> File attribute is (HEX): %X \n" , file_attribute);
    char res;

    //Check for a directory, otherwise it is a file
    //The fifth bit should be set if this is a directory
    if((FILE_ATTRIBUTE_DIRECTORY & file_attribute) == FILE_ATTRIBUTE_DIRECTORY){
        fprintf(res_file , "---> The object is a Directory\n");
        res = 'D';
    } else{
        fprintf(res_file, "---> The object is a File \n");
        res = 'F';
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
    file_id[(FILE_ID_LEN - 2)] = (LETTERS_CHAR + ind_num_of_file); //TODO Check this conversion
    file_id[(FILE_ID_LEN - 1)] = '\0';
    fprintf(res_file , "(Parser) --> File id is: %s \n" , file_id);
    return file_id;
}

/* Line 13 is SV */
void case_13_VS(File file_obj , FILE* res_file , FILE *input_file , char buff[BUFFER_SIZE] , int* block_line_count , bool* read_empty_line_chucnks) {
    /* Params initialization */
    *read_empty_line_chucnks = false;
    char block_id[BLOCK_ID_LEN];
    unsigned int block_size = 0;
    //Block insert_block;

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
            ht_set(ht_blocks , block_id , 1 , blocks_sn , block_size , 'B');
            blocks_sn++;
            fprintf(res_file, "--> Block  - %s - %d \n", block_id, block_size);
            fgets(buff, BUFFER_SIZE, input_file);
            (*block_line_count)++;
        } while (strlen(buff) > 1);
    }
    return;
}

/*
 * update_parent_dir_sn
 */
void update_parent_dir_sn(List previous , List current , int global_depth){
    printf("(update_parent_dir_sn) -->  Updating Parent directory serial numbers ..... \n");
    if(global_depth == 0){ //We are at root Level directory just set everyone to be the children of root
        unsigned long root_sn = root_directory->dir_sn;
        //TODO iterate over the entire list of current (prev list is NULL) and update

    }else{ //Go over both lists and update accordingly
        //Create Previous Depth CSV File
        printf("(update_parent_dir_sn) -->  Creating Previous Depth CSV file for depth %d  ..... \n" , global_depth);
        FILE *prev_depth_fp;
        char* fileName = malloc(sizeof(char)*30);
        sprintf(fileName , "Prev_depth_%d.csv" , global_depth);
        prev_depth_fp = fopen(fileName , "w+");
        fprintf(prev_depth_fp ,"Object ID, Object SN, Parent Dir ID, Object Type" );
        LIST_FOREACH(Object_Info , iter ,previous){
            fprintf(prev_depth_fp ,"\n%s, %d , %s, %c" ,
                    iter->object_id , iter->object_sn , iter->parent_dir_id , iter->object_type);
        }
        fclose(prev_depth_fp);


        //Create Current Depth CSV File
        printf("(update_parent_dir_sn) -->  Creating Current Depth CSV file for depth %d  ..... \n" , global_depth);
        FILE *curr_depth_fp;
        sprintf(fileName , "Curr_depth_%d.csv" , global_depth);
        curr_depth_fp = fopen(fileName , "w+");
        fprintf(curr_depth_fp ,"Object ID, Object SN, Parent Dir ID, Object Type" );
        LIST_FOREACH(Object_Info , iter ,current){
            fprintf(curr_depth_fp ,"\n%s, %d , %s, %c" ,
                    iter->object_id , iter->object_sn , iter->parent_dir_id , iter->object_type);
        }
        fclose(prev_depth_fp);

        free(fileName);
    }
}

/* ****************************************************** MAIN ******************************************************** */
int main(){
    /* ----------------------- Parameters Declarations & Initialization ----------------------- */
    /// File  Manipulation Variables
    int num_of_input_files = 1;
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
    char* parent_dir_id; // Hashed ID of parent Directory
    unsigned short depth = 0; //Depth of current object in the hierarchy
    unsigned int file_size = 0; //File Size (if the object is a file)
    char obj_type; //Hexadecimal Value - Tells the type of the object ( A combination of binary flags set)
    char* object_id; //Hashed ID of the current object
    File file_obj; //Pointer to the FILE Object to be created from the current input
    Dir dir_obj; //Pointer to the DIRECTORY Object to be created from the current input
    bool is_zero_size_file = false;

    /* ----------------------- Parameters Declarations & Initialization ----------------------- */
    /* ---------------------------------------------------------------------------------------- */
    /* -------------------- File Manipulations - Getting Files to Process  -------------------- */
    /*printf("How many files would you like to process?\n");
    scanf("%d" , &num_of_input_files);
    if(num_of_input_files < 1){ //check that at least one file will be processed
        printf("You have requested an illegal number of files to process - The program will end now.");
        return 0;
    }
    printf("Please insert the path to the directory containing the input files: \n");
    scanf("%s" , input_dir_start);

    printf("Now we will insert the file names to be processed:\n");
    for (int i = 0; i <num_of_input_files ; ++i) {
        printf("Please insert the name of file %d : \n",(i+1));
        scanf("%s" , file_names[i]);
    }*/
    /* -------------------- File Manipulations - Getting Files to Process  -------------------- */
    /* ---------------------------------------------------------------------------------------- */
    /* ------------------------- File Manipulations  - opening files  ------------------------- */
    /* Go Over each file, parsing the data into correspond structures */
    /* -------------------- Get File Names To Process -------------------- */

    printf("(Parser)--> ----- Opening File ----- \n");
    /* michal files addresses */
    //input_file = fopen("C:\\Users\\mihush\\Documents\\GitHub\\DeduplicationProject\\DeduplicationProject\\input_example.txt" , "r");
    //res_file_1 = fopen("C:\\Users\\mihush\\Documents\\GitHub\\DeduplicationProject\\DeduplicationProject\\res_file_1.txt" , "w");
    /* Polina files addresses */
    input_file = fopen("C:\\Polina\\Technion\\Semester7\\Dedup Project\\Project_Files\\DeduplicationProject\\input_example.txt" , "r");
    res_file_1 = fopen("C:\\Polina\\Technion\\Semester7\\Dedup Project\\Project_Files\\DeduplicationProject\\res_file_1.txt" , "w");

    if(input_file == NULL){ //check the file was opened successfully - if not terminate
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
                fputs(buff , res_file_1);
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
                            update_parent_dir_sn(previous_depth_objects , curr_depth_objects , global_current_depth);
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
                        if ( ( obj_type == 'F' ) && ( is_zero_size_file == false )){ //Adding File Object to HashTable
                            file_obj = ht_set(ht_files , object_id , depth ,files_sn , file_size ,'F');
                            printf("(Parser)--> Created file with: \n");
                            printf("(Parser)-->         file id - %s\n" , file_obj->file_id);
                            printf("(Parser)-->         file sn - %lu\n" , file_obj->file_sn);

                            //add file to curr_depth_objects list in order to later find the parent directory
                            Object_Info oi_file = object_info_create(object_id , files_sn , parent_dir_id , 'F');
                            listInsertLast(curr_depth_objects , oi_file);
                            object_info_destroy(oi_file); //The list adds a copy of this object and it is no longer needed

                            files_sn++;
                        }
                        else if(obj_type == 'D'){ //Adding Directory Object to HashTable
                            if( dir_sn == 1){ //Creating Dummy Root Node using the Parent_dir_id of the first object in the input file
                                root_directory = ht_set(ht_dirs , parent_dir_id , depth , dir_sn ,DIR_SIZE , 'D' );
                                printf("(Parser)--> Root Directory created : \n");
                                printf("(Parser)-->      directory id: %s\n" , root_directory->dir_id);
                                printf("(Parser)-->      directory sn: %lu\n\n", root_directory->dir_sn);
                                dir_sn++;
                            }
                            printf("(Parser)--> Global Directory SN is: %lu\n", dir_sn);
                            //Create Directory Object with the retrieved data
                            dir_obj = ht_set(ht_dirs, object_id, depth, dir_sn, DIR_SIZE , 'D');
                            printf("(Parser)--> Created Directory with:\n");
                            printf("(Parser)-->         directory id: %s\n", dir_obj->dir_id);
                            printf("(Parser)-->         directory sn: %lu\n", dir_obj->dir_sn);

                            //add file to curr_depth_objects list in order to later find the parent directory
                            Object_Info oi_dir = object_info_create(object_id , files_sn , parent_dir_id , 'D');
                            listInsertLast(curr_depth_objects , oi_dir);
                            object_info_destroy(oi_dir); //The list adds a copy of this object and it is no longer needed

                            dir_sn++;
                        }
                        break;
                    case 13: /* Line 13 is SV */
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

            /***********************************************************************************************/
            /******************* WE HAVE REACHED THE END OF THE CURRENT INPUT OBJECT !!! ********************/
            if(!feof(input_file)){ //Update parameters that are relevant to a single object
                printf("(Parser) -------> finished Reading an Object \n\n");
                fputs("(Parser) -------> finished Reading an Object \n\n" , res_file_1);
                /* Zero the line count for the next block */
                block_line_count = 0;
                read_empty_line_chucnks = false;
                file_obj = NULL;
                dir_obj = NULL;
                is_zero_size_file = false;
            }
        }
    }

    printf("(Parser) --> --- Finished reading the input file - Now lets start processing ---\n");

    /* -------------------- File Closing -------------------- */
    fclose(input_file);
    fclose(res_file_1);

    //printf("(Parser) --> -------------- Print File Hash Table :\n");
    //print_ht_File(ht_files);

    return 0;
}
