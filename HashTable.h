//
// Created by mihush on 11/12/2017.
//

#ifndef DEDUPLICATION_PROJ_HASHTABLE_H
#define DEDUPLICATION_PROJ_HASHTABLE_H
/* **************************************************** INCLUDES **************************************************** */
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>

#include "Block.h"
#include "File.h"
#include "List.h"
#include "Directory.h"
/* **************************************************** INCLUDES **************************************************** */
/* ****************************************************************************************************************** */
/* **************************************************** DEFINES ***************************************************** */
#define INIT_SIZE 5007
#define BLOCKS_INIT_SIZE 20000
#define FILES_INIT_SIZE 10000
#define DIRS_INIT_SIZE 6000
#define PHYSICAL_FILES_SIZE 10000
typedef void* Data;
/* **************************************************** DEFINES ***************************************************** */
/* ****************************************************************************************************************** */
/* ******************* START ******************** HashTable Definition ******************** START ******************* */

struct entry_t {
    char *key;
    Data data;
    struct entry_t *next;//Chain-hashing solution. ptr to the next element
};
typedef struct entry_t *Entry;

struct hashtable_t {
    long size_table;
    long num_of_elements;
    Entry *table; // array of pointers to Entries
};
typedef struct hashtable_t *HashTable;

/* ********************* END ******************** HashTable Definition********************* END ********************* */
/* ****************************************************************************************************************** */
/* ****************************************************************************************************************** */
/* ******************** START ******************** HashTable Functions ******************** START ******************* */
/* Create a new HashTable */
/*
 * ht_create - creates a hashtable for the requested type of objects (which determines its size)
 *
 * @type - can be one of 3 : 'B' for blocks , 'F' for files and 'D' for directories
 */
HashTable ht_create(char type) {
    HashTable ht = NULL;

    ht = malloc(sizeof(*ht)); //Allocate the table
    if(!ht){ //check allocation was successful
        return NULL;
    }
    switch(type){
        case 'B': //Blocks
            ht->size_table = BLOCKS_INIT_SIZE;
            break;
        case 'F': //Logical Files
            ht->size_table = FILES_INIT_SIZE;
            break;
        case 'D': //Directories
            ht->size_table = DIRS_INIT_SIZE;
            break;
        default:
            ht->size_table = INIT_SIZE; //Shouldn't really get here
            break;
    }
    ht->num_of_elements = 0;

    /* Allocate pointers to the head nodes */
    ht -> table = malloc(sizeof(Entry) * (ht->size_table));
    if(!ht -> table ){ //check array od pointers was allocated successfully
        free(ht);
        return NULL;
    }

    for(int i = 0; i < (ht->size_table) ; i++ ){
        ht->table[i] = NULL;
    }
    return ht;
}

/*
 * ht_hash - Given a key (string) Generates a Hash Value by which it will be stored in the table
 *
 * @ht  - the hashtable in which the key will be saved
 * @key - the key for which we want to get the hashed value in the hashtable
 */
long int ht_hash( HashTable ht, char *key ) {
    unsigned long int hashval = 0;
    int i = 0;
    /* Convert our string to an integer */
    while((hashval < ULONG_MAX) && (i < strlen(key))){
        hashval = hashval << 8;
        hashval += key[i];
        i++;
    }

    return (hashval % (ht->size_table));
}

/*
 * ht_newpair - Creates a key-value pair
 *               - For block - size parameter will contain the block size
 *               - For File - size parameter will be -1
 */
Entry ht_newpair(char *key, unsigned int depth , unsigned long sn , unsigned int size , char flag ,
                 unsigned long physical_sn){
    Entry newpair  = malloc(sizeof(*newpair));
    if(newpair == NULL){
        return NULL;
    }

    newpair->key = malloc(sizeof(char)*(strlen(key)+1));
    if(newpair->key == NULL){
        free(newpair);
        return NULL;
    }
    newpair->key = strcpy(newpair->key , key);

    if(flag == 'B'){ // save the data object
        newpair->data = block_create(key , sn, size);
    }else if( flag == 'D'){
        newpair->data = dir_create(key , depth , sn);
    } else if(flag == 'F'){ //This is a file object
        newpair->data = file_create(key , depth , sn , size , physical_sn);
    }

    if(newpair->data == NULL) {
        free(newpair->key);
        free(newpair);
        return NULL;
    }

    newpair->next = NULL;
    return newpair;
}

/*
 * ht_set - Insert a key-value pair into a hash table (General function thus
 *
 * @ht            - the hashtable to which the object will be added
 * @key           - the hashed id of the object
 * @depth         - the depth of the object in the filesystem hierarchy
 * @sn            - the serial number of the object
 * @size          - the size of the object
 * @flag          - a flag that signifies the object: 'B' for block , 'F' for File and 'D' for directory
 * @object_exists - pointer to variable that will be an output of the function - True if the block or file already exist in the hashtable
 * @physical_sn   - the serial number of a physical file
 */
Data ht_set(HashTable ht, char *key, unsigned int depth , unsigned long sn , unsigned int size , char flag,
            bool* object_exists , unsigned long physical_sn, char dedup_type) {
    Entry newpair = NULL;
    Entry next = NULL;
    Entry last = NULL;

    long int hash_key = ht_hash( ht , key );
    next = ht->table[hash_key];

    if(dedup_type == 'B' && flag == 'F'){// We are using Block Level Deduplication and we are working on File object
        newpair = ht_newpair(key, depth , sn, size, flag , physical_sn);
        newpair->next = next;
        ht->table[hash_key] = newpair;
        return newpair->data;
    }

    // The code gets here only in 2 cases:
    //              - Block Level Deduplication - Block object or Directory Object
    //              - File Level Deduplication - Block Object or Directory Object Since files are added differently without using ht_set
    /* Advance until get the end of the list OR first matching key */
    while( next != NULL && next->key != NULL && strcmp( key, next->key ) != 0 ) {
        last = next;
        next = next->next;
    }

    /* There's already a pair. Let's replace that string. */
    if( next != NULL && next->key != NULL && strcmp( key, next->key ) == 0 ) {
        //Return the pointer to the Block/File that already exists in the hash
        *object_exists = true;
        return next->data;
    } else { /* Nope, could't find it.  Time to grow a pair. */
        newpair = ht_newpair(key, depth , sn, size, flag , physical_sn); //allocate new pair
        if(newpair == NULL){
            return NULL;
        }
        /* We're at the start of the linked list in this hash_key. */
        if( next == ht->table[hash_key] ){ // If we in an empty list
            newpair->next = next;
            ht->table[hash_key] = newpair;

            /* We're at the end of the linked list in this hash_key. */
        } else if ( next == NULL ) {
            last->next = newpair;

        } else  { /* We're in the middle of the list. */
            newpair->next = next;
            last->next = newpair;
        }
        return newpair->data;
    }
}

/*
 * ht_get - Retrieve pointer for block/file element with corresponding key in hash table
 *
 * @ht  - the hashtable to which the object will be added
 * @key - the hashed id of the object
 */
Data ht_get( HashTable ht, char *key ) {
    long int hash_key = ht_hash(ht, key);
    Entry pair = ht->table[hash_key];

    /* Step through the hash_key, looking for our value. */
    while( pair != NULL && pair->key != NULL && strcmp( key, pair->key ) != 0 ) {
        pair = pair->next;
    }

    /* Did we actually find anything? */
    if( pair == NULL || pair->key == NULL || strcmp( key, pair->key ) != 0 ) {
        //didn't find anything
        return NULL;

    }
    //found the key - return the data
    return pair->data;
}

/*
 * data_destroy - Destroy the data object
 *
 * @data - pointer to the data object to be destroyed
 * @flag - flag that signifies if the object is flag ('F') or block 'B' or directory 'D'
 */
void data_destroy(Data data, char flag){
    switch (flag){
        case 'F':
            file_destroy((File)data);
            break;
        case 'D':
            dir_destroy((Dir)data);
            break;
        case 'B':
            block_destroy((Block)data);
            break;
        //TODO add case of Physical file - do we really need this ?
    }
}

/*
 * hashTable_destroy - Freeing all allocations of HashTable
 *
 * @ht - hashtable to destroy
 * @flag - flag that signifies if the object is flag ('F') or block 'B' or directory 'D'
 */
void hashTable_destroy(HashTable ht , char flag){
    long size_table = ht->size_table;
    //long size_of_lists = 0;
    struct entry_t* temp_to_free;
    // Remove lists elements of each HashTable cell
    for(int i = 0 ; i < size_table ; i++){
        while(ht->table[i]) {         // free each list element of cell i
            temp_to_free = ht->table[i];
            ht->table[i] = temp_to_free->next;
            // Destroy elements fields
            data_destroy(temp_to_free->data , flag);
            free(temp_to_free->key);
            free(temp_to_free);
        }
        assert(ht->table[i] == NULL);
    }
    free(ht->table);
    free(ht);
}

/*
 * file_compare_to_File - files are considered identical if have the same blocks
 *                          1 - check sizes
 *                          2 - check amount of blocks
 *                          3 - check first block , second block, etc ....
 *                         returns false if physical file already exists
 *                         returns true if no physical file exists
 */
Data file_compare(HashTable ht_files , HashTable ht_physical_files ,
                  File file , File file_obj_p,
                  unsigned long* physical_files_sn){
    assert(file && file_obj_p);
    bool physical_file_exist = false , blocks_differ = false;
    Block_Info first_block = (Block_Info)listGetFirst(file->blocks_list);
    char* first_block_id = first_block->id;
    long int hash_key = ht_hash(ht_physical_files , first_block_id);

    /* ---------------------------------- Iterate over HT_PHYSICAL_FILES ---------------------------------- */
    File temp_file = NULL;
    Entry current = ht_physical_files->table[hash_key]; //get the cell int the hashtable for the possible file
    while(current != NULL && current->key != NULL){ //go over all files in the cell found above
        temp_file = ((File)(current->data));
        if(strcmp(file->file_id , temp_file->file_id) == 0){ //It's the same file
            current = current->next;
            continue;
        }
        if(file->file_size != temp_file->file_size){ //Compare by sizes
            current = current->next;
            continue;
        }
        if(file->num_blocks != temp_file->num_blocks){ //Compare by amount of blocks
            current = current->next;
            continue;
        }

        Object_Info temp_oi; //Compare each block
        Block_Info temp_file_blocks = listGetFirst(temp_file->blocks_list);
        LIST_FOREACH(Block_Info , iter ,file->blocks_list){
            if(strcmp(iter->id , temp_file_blocks->id) != 0){
                temp_oi = listGetFirst(temp_file->blocks_list);
                temp_oi = listGetFirst(file->blocks_list);
                blocks_differ = true; // the blocks aren't the same
                break;
            }
            temp_file_blocks = listGetNext(temp_file->blocks_list);
        }
        temp_oi = listGetFirst(temp_file->blocks_list);
        temp_oi = listGetFirst(file->blocks_list);
        if(blocks_differ == true){//advance to the next cell
            current = current->next;
            continue;
        } else { // We have found a match
            physical_file_exist = true;
            break;
        }
    } /* Finished searching for a physical file*/
    /* ---------------------------------- Iterate over HT_PHYSICAL_FILES ---------------------------------- */
    /* ---------------------------------------------------------------------------------------------------- */
    /* ---------------------------------- Adding the file to hash table ----------------------------------- */
    if(physical_file_exist == true) { // physical file already exits - add file to ht_files only
        file_set_logical_flag(file);
        ht_setF(temp_file->files_ht, file->file_id); // add logical file to the files ht of the physical we found
        (temp_file->num_files)++;
        file_set_physical_sn(file , temp_file->physical_sn); // set the physical sn of the logical file to be the one of the physical stored
        (*physical_files_sn)--;
        file_destroy(file_obj_p);
    } else { //add file only to ht_physical_files and to ht_files
        // hash by first block id
        hash_key = ht_hash(ht_physical_files , first_block_id);
        Entry ent = ht_physical_files->table[hash_key];
        Entry newpair  = malloc(sizeof(*newpair));
        assert(newpair);
        newpair->key = malloc(sizeof(char)*(strlen(first_block_id) + 1));
        assert(newpair->key);
        newpair->key = strcpy(newpair->key , first_block_id);
        newpair->data = file_obj_p;
        //Add the file in the head of the list
        newpair->next = ent;
        ht_physical_files->table[hash_key] = newpair;
    }
    /*-------------------- Adding the file to logical hash table anyway -----------------------------*/

    char* key = malloc(sizeof(char)*(strlen(file->file_id) + 1));
    strcpy(key , file->file_id);

    Entry newpair_l  = malloc(sizeof(*newpair_l));
    if(newpair_l == NULL){
        free(key);
        return NULL;
    }
    newpair_l->key = malloc(sizeof(char)*(strlen(file->file_id)+1));
    newpair_l->key = strcpy(newpair_l->key , file->file_id);
    newpair_l->data = file;

    Entry curr = NULL , last = NULL;
    long int hash_key_f = ht_hash( ht_files , file->file_id );
    curr = ht_files->table[hash_key_f];
    /* Advance until get the end of the list OR first matching key*/
    while( curr != NULL ) {
        if(curr->key != NULL){
            break;
        }
        if(strcmp( key, curr->key ) != 0){
            last = curr;
            curr = curr->next;
        } else{
            break;
        }
    }

    /* There's already a pair. Let's replace that string. */
    if( curr != NULL && curr->key != NULL && strcmp( key, curr->key ) == 0 ) {
        //Return the pointer to the Block/File that already exists in the hash
        free(key);
        return curr->data;
    } else { /* Nope, could't find it.  Time to grow a pair. */
        /* We're at the start of the linked list in this hash_key. */
        if( curr == ht_files->table[hash_key_f] ){ // If we in an empty list
            newpair_l->next = curr;
            ht_files->table[hash_key_f] = newpair_l;

            /* We're at the end of the linked list in this hash_key. */
        } else if ( curr == NULL ) {
            last->next = newpair_l;

        } else  { /* We're in the middle of the list. */
            newpair_l->next = curr;
            last->next = newpair_l;
        }
        free(key);
        return newpair_l->data;
    }
    free(key);
}

/* ********************* END ********************* HashTable Functions ********************* END ******************** */

#endif //DEDUPLICATION_PROJ_HASHTABLE_H
