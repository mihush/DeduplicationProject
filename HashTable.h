//
// Created by mihush on 11/12/2017.
//

#ifndef DEDUPLICATION_PROJ_HASHTABLE_H
#define DEDUPLICATION_PROJ_HASHTABLE_H

#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>

struct entry_s {
    char *key;
    char *value;
    struct entry_s *next; //Chain-hashing solution. ptr to the next element
};

typedef struct entry_s entry_t;

struct hashtable_t {
    long size_table;
    int num_of_elements;
    struct entry_s **table;
};

typedef struct hashtable_t HashTable;


/* Create a new HashTable. */
HashTable *ht_create( long size ) {

    HashTable *hashtable = NULL;
    int i=0;

    if( size < 1 ) return NULL;

    /* Allocate the table itself */
    hashtable = malloc( sizeof( HashTable ));
    if( !hashtable) {
        return NULL;
    }

    /* Allocate pointers to the head nodes */
    hashtable->table = malloc( sizeof( entry_t * ) * size );
    if(!hashtable->table ) {
        return NULL;
    }
    for( i = 0; i < size; i++ ) {
        hashtable->table[i] = NULL;
    }

    hashtable->size = size;
    return hashtable;
}

/* Hash a string for a particular hash table. */
int ht_hash( HashTable *hashtable, char *key ) {

    unsigned long int hashval;
    int i = 0;

    /* Convert our string to an integer */
    while( hashval < ULONG_MAX && i < strlen( key ) ) {
        hashval = hashval << 8;
        hashval += key[ i ];
        i++;
    }

    return hashval % hashtable->size;
}

/* Create a key-value pair. */
entry_t *ht_newpair( char *key, char *value ) {
    entry_t *newpair;

    if( ( newpair = malloc( sizeof( entry_t ) ) ) == NULL ) {
        return NULL;
    }

    if( ( newpair->key = strdup( key ) ) == NULL ) {
        return NULL;
    }

    if( ( newpair->value = strdup( value ) ) == NULL ) {
        return NULL;
    }

    newpair->next = NULL;

    return newpair;
}

/* Insert a key-value pair into a hash table. */
void ht_set( HashTable *hashtable, char *key, char *value ) {
    int bin = 0;
    entry_t *newpair = NULL;
    entry_t *next = NULL;
    entry_t *last = NULL;

    bin = ht_hash( hashtable, key );

    next = hashtable->table[ bin ];

    while( next != NULL && next->key != NULL && strcmp( key, next->key ) > 0 ) {
        last = next;
        next = next->next;
    }

    /* There's already a pair.  Let's replace that string. */
    if( next != NULL && next->key != NULL && strcmp( key, next->key ) == 0 ) {

        free( next->value );
        next->value = strdup( value );

        /* Nope, could't find it.  Time to grow a pair. */
    } else {
        newpair = ht_newpair( key, value );

        /* We're at the start of the linked list in this bin. */
        if( next == hashtable->table[ bin ] ) {
            newpair->next = next;
            hashtable->table[ bin ] = newpair;

            /* We're at the end of the linked list in this bin. */
        } else if ( next == NULL ) {
            last->next = newpair;

            /* We're in the middle of the list. */
        } else  {
            newpair->next = next;
            last->next = newpair;
        }
    }
}

/* Retrieve a key-value pair from a hash table. */
char *ht_get( HashTable *hashtable, char *key ) {
    int bin = 0;
    entry_t *pair;

    bin = ht_hash( hashtable, key );

    /* Step through the bin, looking for our value. */
    pair = hashtable->table[ bin ];
    while( pair != NULL && pair->key != NULL && strcmp( key, pair->key ) > 0 ) {
        pair = pair->next;
    }

    /* Did we actually find anything? */
    if( pair == NULL || pair->key == NULL || strcmp( key, pair->key ) != 0 ) {
        return NULL;

    } else {
        return pair->value;
    }

}
#endif //DEDUPLICATION_PROJ_HASHTABLE_H
