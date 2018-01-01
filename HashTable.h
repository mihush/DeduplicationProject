//
// Created by mihush on 11/12/2017.
//

#ifndef DEDUPLICATION_PROJ_HASHTABLE_H
#define DEDUPLICATION_PROJ_HASHTABLE_H

/* *************** START ************** HashTable Definition *************** START *************** */
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>

#include "Block.h"
#include "File.h"
#include "List.h"
#include "Directory.h"

#define GROWTH_FACTOR 2
#define INIT_SIZE 5007
//TODO Set Correct Sizes
#define BLOCKS_INIT_SIZE 5007
#define FILES_INIT_SIZE 5007
#define DIRS_INIT_SIZE 5007
typedef void* Data;

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

/* **************** END *************** HashTable Definition **************** END **************** */
/* *********************************************************************************************** */
/* *********************************************************************************************** */
/* *************** START ************** HashTable Functions *************** START **************** */
/* Create a new HashTable. */
HashTable ht_create(char type) {
    HashTable ht = NULL;

    /* Allocate the table itself */
    ht = malloc(sizeof(*ht));
    if(!ht){ //check allocation was successful
        printf("(HashTable)--> Creating HashTable - Allocation Error (1) \n");
        return NULL;
    }
    switch(type){
        case 'B':
            ht->size_table = BLOCKS_INIT_SIZE;
            break;
        case 'F':
            ht->size_table = FILES_INIT_SIZE;
            break;
        case 'D':
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
        printf("(HashTable)--> Creating HashTable - Allocation Error (2) \n");
        free(ht);
        return NULL;
    }

    for(int i = 0; i < (ht->size_table) ; i++ ){
        ht->table[i] = NULL;
    }
    printf("(HashTable)--> Created HashTable Sucessfully of size %d \n" , ht->size_table);
    return ht;
}

/*
 * ht_hash - Given a key (string) Generates a Hash Value by which it will be stored in the table
 */
long int ht_hash( HashTable ht, char *key ) {
    unsigned long int hashval;
    int i = 0;

    /* Convert our string to an integer */
    while((hashval < ULONG_MAX) && (i < strlen(key))){
        hashval = hashval << 8;
        hashval += key[i];
        i++;
    }

    return hashval % (ht->size_table);
}

/*
 * ht_newpair - Creates a key-value pair
 *                  - For block - size parameter will contain the block size
 *                  - For File - size parameter will be -1
 */
Entry ht_newpair(char *key, unsigned int depth , unsigned long sn , unsigned int size , char flag){
    Entry newpair  = malloc(sizeof(*newpair));
    if(newpair == NULL){
        printf("(HashTable)--> Creating new pair - Allocation Error (1) \n");
        return NULL;
    }

    newpair->key = strdup(key);
    if(newpair->key == NULL){
        printf("(HashTable)--> Creating new pair - Allocation Error (2) \n");
        free(newpair);
        return NULL;
    }

    if(flag == 'B'){ // save the data object
        printf("(HashTable)--> Creating new pair - BLOCK \n");
        newpair->data = block_create(key , sn, size);
    }else if( flag == 'D'){
        printf("(HashTable)--> Creating new pair - DIRECTORY \n");
        newpair->data = dir_create(key , depth , sn);
    }
    else{ //This is a file object
        printf("(HashTable)--> Creating new pair - FILE \n");
        newpair->data = file_create(key , depth , sn , size);
    }

    if(newpair->data == NULL) {
        printf("(HashTable)--> Failed to allocate new pair \n");
        free(newpair->key);
        free(newpair);
        return NULL;
    }
    newpair->next = NULL;
    return newpair;
}

/*
 * ht_set - Insert a key-value pair into a hash table.
 */
Data ht_set(HashTable ht, char *key, unsigned int depth , unsigned long sn , unsigned int size , char flag) {
    Entry newpair = NULL;
    Entry next = NULL;
    Entry last = NULL;

    long int hash_key = ht_hash( ht , key );
    next = ht->table[hash_key];

    /* Advance until get the end of the list OR first matching key*/
    while( next != NULL && next->key != NULL && strcmp( key, next->key ) != 0 ) {
        last = next;
        next = next->next;
    }

    /* There's already a pair. Let's replace that string. */
    if( next != NULL && next->key != NULL && strcmp( key, next->key ) == 0 ) {
        //Return the pointer to the Block/File that already exists in the hash
        return next->data;
    } else { /* Nope, could't find it.  Time to grow a pair. */
        newpair = ht_newpair(key, depth , sn, size, flag ); //allocate new pair
        if(newpair == NULL){
            printf("(HashTable)--> Adding Pair to HT - Allocation Error (1) \n");
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
 * ht_get - Retrieve pointer for block/file element with corresponding key in hash table.
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

void print_ht_File(HashTable ht){
    printf("Printing HashTable: \n");
    for(int i = 0; i < (ht->size_table) ; i++ ){
        Entry pair = ht->table[i];
        /* Step through the hash_key, looking for our value. */
        while( pair != NULL && pair->key != NULL) {
            printf("Key : %s \n SN : %lu \n" , pair->key , ((File)(pair->data))->file_sn);
            printf("Num of blocks: %d \n " , ((File)(pair->data))->num_blocks);
            printf("The file contains the following blocks:\n");
            Block_Info iter = listGetFirst(((File)(pair->data))->blocks_list);
            if(iter == NULL && (((File)(pair->data))->num_blocks > 0) ){
                printf(" This file has no blocks - ooooppppsss!\n");
            }

            LIST_FOREACH(Block_Info, iter, ((File)(pair->data))->blocks_list) {
                printf("%s -- %d \n", iter->id , iter->size);
            }
            pair = pair->next;
        }
    }
}

/* **************** END *************** HashTable Functions **************** END ***************** */

#endif //DEDUPLICATION_PROJ_HASHTABLE_H
