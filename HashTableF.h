//
// Created by Polina on 06-Jan-18.
//

#ifndef DEDUPLICATIONPROJECT_HASHTABLEF_H
#define DEDUPLICATIONPROJECT_HASHTABLEF_H

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <assert.h>

#define GROWTH_FACTOR 2
#define INIT_SIZE 5007
#define BLOCKS_IN_FILE_SIZE 300
typedef void* DataF;

struct entryf_t {
    char *key;
    DataF data;
    struct entryf_t *next;//Chain-hashing solution. ptr to the next element
};
typedef struct entryf_t *EntryF;

struct hashtablef_t {
    long size_table;
    long num_of_elements;
    EntryF *table; // array of pointers to Entries
};
typedef struct hashtablef_t *HashTableF;

/* **************** END *************** HashTable Definition **************** END **************** */
/* *********************************************************************************************** */
/* *********************************************************************************************** */
/* *************** START ************** HashTable Functions *************** START **************** */
/* Create a new HashTable. */
HashTableF ht_createF(char type) {
    HashTableF ht = NULL;

    /* Allocate the table itself */
    ht = malloc(sizeof(*ht));
    if(!ht){ //check allocation was successful
        printf("(HashTableF)--> Creating HashTable - Allocation Error (1) \n");
        return NULL;
    }
    switch(type){
        case 'N': //for Hashtable in file object
            ht->size_table = BLOCKS_IN_FILE_SIZE;
            break;
        default:
            ht->size_table = INIT_SIZE; //Shouldn't really get here
            break;
    }
    ht->num_of_elements = 0;

    /* Allocate pointers to the head nodes */
    ht -> table = malloc(sizeof(EntryF) * (ht->size_table));
    if(!ht -> table ){ //check array od pointers was allocated successfully
        printf("(HashTableF)--> Creating HashTable - Allocation Error (2) \n");
        free(ht);
        return NULL;
    }

    for(int i = 0; i < (ht->size_table) ; i++ ){
        ht->table[i] = NULL;
    }
    printf("(HashTableF)--> Created HashTable Successfully of size %lu \n" , ht->size_table);
    return ht;
}

/*
 * ht_hash - Given a key (string) Generates a Hash Value by which it will be stored in the table
 */
long int ht_hashF( HashTableF ht, char *key ) {
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
EntryF ht_newpairF(char *key){
    EntryF newpair  = malloc(sizeof(*newpair));
    if(newpair == NULL){
        printf("(HashTableF)--> Creating new pair - Allocation Error (1) \n");
        return NULL;
    }

    newpair->key = malloc(sizeof(char)*(strlen(key)+1));
    if(newpair->key == NULL){
        printf("(HashTableF)--> Creating new pair - Allocation Error (2) \n");
        free(newpair);
        return NULL;
    }
    strcpy(newpair->key , key);
    newpair->data = NULL;
    newpair->next = NULL;
    return newpair;
}

/*
 * ht_set - Insert a key-value pair into a hash table.
 */
DataF ht_setF(HashTableF ht, char *key) {
    EntryF newpair = NULL;
    EntryF next = NULL;
    EntryF last = NULL;

    long int hash_key = ht_hashF( ht , key );
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
        newpair = ht_newpairF(key); //allocate new pair
        if(newpair == NULL){
            printf("(HashTableF)--> Adding Pair to HT - Allocation Error (1) \n");
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
DataF ht_getF(HashTableF ht, char *key ) {
    long int hash_key = ht_hashF(ht, key);
    EntryF pair = ht->table[hash_key];

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
 * ht_free - freeing all allocations of HashTable.
 */
void hashTableF_destroy(HashTableF ht){
    long num_of_elements = ht->num_of_elements;
    long size_of_lists = 0;
    struct entryf_t* temp_to_free;
    // Remove lists elements of each HashTable cell
    for(int i = 0 ; i < num_of_elements ; i++){
        // free each list element of cell i
        while(ht->table[i]) {
            temp_to_free = ht->table[i];
            ht->table[i] = temp_to_free->next;

            // Destroy elements fields
            free(temp_to_free->key);
            free(temp_to_free);
        }
        assert(ht->table[i]==NULL);
    }
    free(ht->table);
    free(ht);
}
#endif //DEDUPLICATIONPROJECT_HASHTABLEF_H
