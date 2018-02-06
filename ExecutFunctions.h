//
// Created by mihush on 27/01/2018.
//

#ifndef DEDUPLICATIONPROJECT_EXECUTFUNCTIONS_H
#define DEDUPLICATIONPROJECT_EXECUTFUNCTIONS_H
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
    char* dir_name_hash = calloc(DIR_NAME_LEN,sizeof(char));
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
    char* file_id = calloc(FILE_ID_LEN , sizeof(char)); // The value is 15 chars + 2 chars for index +1 for eol (end of line)
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
// returns true if a file was created
void case_13_VS(FILE* res_file , FILE *input_file , char buff[BUFFER_SIZE] , int* block_line_count ,
                bool* read_empty_line_chucnks ,unsigned short depth, char* object_id, unsigned int file_size ,
                bool* file_was_created, bool* finished_process_blocks) {
    /* Params initialization */
    bool read_block = false;
    *read_empty_line_chucnks = false;
    char block_id[BLOCK_ID_LEN];
    unsigned int block_size = 0;

    while((read_block == false) && (*read_empty_line_chucnks == false)){
        printf("----- %d\n",*block_line_count);
        printf("--> %s \n", buff);
        switch ((int)(buff[0])) { //check next line
            case 'S':
                break;
            case 'V':
                break;
            case 'A':
                break;
                /* Empty line */
            case LINE_SPACE :
                *read_empty_line_chucnks = true;
                break;
                /* Data chunk */
            default:
                read_block = true;
                break;
        }
        if((*read_empty_line_chucnks != true)&&(read_block != true)){
            fgets(buff, BUFFER_SIZE, input_file);
            (*block_line_count)++;
        }
    }
    if (*read_empty_line_chucnks == true) {
        return;
    }
    // If we got here it means we have blocks to read
    // Add file to files hashtable
    File file_obj = ht_set(ht_files , object_id , depth ,files_sn , file_size ,'F');
    files_sn++;
    *file_was_created = true;
    printf("-> FILE : %s\n", object_id);

    /* Read all data chunks */
    if ((int)(buff[0]) != LINE_SPACE) {
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
            blocks_sn++; // TODO if block already exists do not increase !!!
            //fprintf(res_file, "--> Block  - %s - %d \n", block_id, block_size);
            fgets(buff, BUFFER_SIZE, input_file);
            (*block_line_count)++;
        } while (strlen(buff) > 1);
    }
    *finished_process_blocks = true;
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





#endif //DEDUPLICATIONPROJECT_EXECUTFUNCTIONS_H
