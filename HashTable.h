//
// Created by mihush on 11/12/2017.
//

#ifndef DEDUPLICATION_PROJ_HASHTABLE_H
#define DEDUPLICATION_PROJ_HASHTABLE_H

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>

#include "Block.h"
#include "File.h"

#define GROWTH_FACTOR 2
#define INIT_SIZE 5007
typedef void* Data;

/* ------------------------------------------------------------------------------- */
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
/* ------------------------------------------------------------------------------- */

/* Create a new HashTable. */
HashTable ht_create() {
    HashTable ht = NULL;

    /* Allocate the table itself */
    ht = malloc(sizeof(*ht));
    if(!ht){ //check allocation was successful
        printf("Allocation of ht object FAILED !!\n");
        return NULL;
    }
    ht->size_table = INIT_SIZE;
    ht->num_of_elements = 0;

    /* Allocate pointers to the head nodes */
    ht -> table = malloc(sizeof(Entry) * (ht->size_table));
    if(!ht -> table ){ //check array od pointers was allocated successfully
        printf("Allocation of hashtable array FAILED !!!!\n");
        free(ht);
        return NULL;
    }

    for(int i = 0; i < (ht->size_table) ; i++ ){
        ht->table[i] = NULL;
    }

    return ht;
}

/* Hash a string for a particular hash table. */
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

/* Create a key-value pair */
//For block - size parameter will contain the block size
//For File - size parameter will be -1
Entry ht_newpair(char *key, unsigned int depth , unsigned long sn , unsigned int size ,unsigned int dir_sn , char flag){
    Entry newpair  = malloc(sizeof(*newpair));
    if(newpair == NULL){
        return NULL;
    }

    newpair->key = strdup(key);
    if(newpair->key == NULL){
        free(newpair);
        return NULL;
    }

    if(flag == 'B'){ // save the data object
        newpair->data = block_create(key , sn , size);
    }else{ //This is a file object
        newpair->data = file_create(key , depth , sn , dir_sn);
    }
    if(newpair->data == NULL) {
        free(newpair->key);
        free(newpair);
        return NULL;
    }
    newpair->next = NULL;
    return newpair;
}

/* Insert a key-value pair into a hash table. */
Data ht_set(HashTable ht, char *key, unsigned int depth , unsigned long sn , unsigned int size ,unsigned int dir_sn , char flag) {
    Entry newpair = NULL;
    Entry next = NULL;
    Entry last = NULL;

    long int hash_key = ht_hash( ht , key );
    next = ht->table[hash_key];

    /* Advance until get the end of the list OR first matching key*/
    while( next != NULL && next->key != NULL && strcmp( key, next->key ) > 0 ) {
        last = next;
        next = next->next;
    }

    /* There's already a pair. Let's replace that string. */
    if( next != NULL && next->key != NULL && strcmp( key, next->key ) == 0 ) {
        //Return the pointer to the Block/File that already exists in the hash
        return next->data;
    } else { /* Nope, could't find it.  Time to grow a pair. */
        newpair = ht_newpair(key, depth , sn, size, dir_sn, flag ); //allocate new pair
        if(newpair == NULL){
            printf(" --> Allocation Error in adding new value to hash\n");
            return NULL;
        }
        /* We're at the start of the linked list in this hash_key. */
        if( next == ht->table[hash_key] ){ //First element list
            newpair->next = next;
            ht->table[hash_key] = newpair;

            /* We're at the end of the linked list in this hash_key. */
        } else if ( next == NULL ) {
            last->next = newpair;

        } else  { /* We're in the middle of the list. */
            //Shouldn't really happen
            printf("--> Errorrrrrrrrrrrrrrrrrrr\n");
            newpair->next = next;
            last->next = newpair;
        }
        return newpair->data;
    }
}

/* Retrieve pointer for block/file element with corresponding key in hash table. */
Data ht_get( HashTable ht, char *key ) {
    long int hash_key = ht_hash(ht, key);
    Entry pair = ht->table[hash_key];

    /* Step through the hash_key, looking for our value. */
    while( pair != NULL && pair->key != NULL && strcmp( key, pair->key ) > 0 ) {
        pair = pair->next;
    }

    /* Did we actually find anything? */
    if( pair == NULL || pair->key == NULL || strcmp( key, pair->key ) != 0 ) {
        //didn't find anything
        return NULL;

    } else {
        //found the key - return the data
        return pair->data;
    }
}
void print_ht_File(HashTable ht){
    for(int i = 0; i < (ht->size_table) ; i++ ){
        Entry pair = ht->table[i];
        /* Step through the hash_key, looking for our value. */
        while( pair != NULL && pair->key != NULL) {
            printf("Key : %s \n SN : %lu \n" , pair->key , ((File)(pair->data))->file_sn);
            pair = pair->next;
        }
    }
}

#endif //DEDUPLICATION_PROJ_HASHTABLE_H
