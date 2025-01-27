// Hash Table ADT Implementation (Linear Probing)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash_table.h"

#define RESIZE_FACTOR 4

struct hash_table {
	struct slot *slots;
	int num_slots;
};

struct slot {
	Key key;
	Value value;
	bool empty;
};

int hash(Key key, int num_slots);
bool key_compare(Key key1, Key key2);

////////////////////////////////// FUNCTIONS ///////////////////////////////////

HashTable HashTableNew(int num_slots) {
    HashTable ht = malloc(sizeof(struct hash_table));
    if (ht == NULL) {
        perror("malloc");
		exit(EXIT_FAILURE);
    }

    ht->slots = malloc(num_slots * RESIZE_FACTOR * sizeof(struct slot));
    ht->num_slots = num_slots * RESIZE_FACTOR;

    for (int i = 0; i < ht->num_slots; i++) {
        ht->slots[i].empty = true;
	}

    return ht;
}

void HashTableFree(HashTable ht) {
    free(ht->slots);
    free(ht);
}

void HashTableInsert(HashTable ht, Key key, Value value) {
    int idx = hash(key, ht->num_slots);

    for (int i = 0; i < ht->num_slots; i++) {
        if (ht->slots[idx].empty || key_compare(ht->slots[idx].key, key)) {
            struct slot new_slot = {key, value, false};
            ht->slots[idx] = new_slot;
            break;
        }

        idx = (idx + 1) % ht->num_slots;
    }
}

bool HashTableContains(HashTable ht, Key key) {
    int idx = hash(key, ht->num_slots);

    for (int i = 0; i < ht->num_slots; i++) {
        if (ht->slots[idx].empty) break;
        if (key_compare(ht->slots[idx].key, key)) return true;
        
        idx = (idx + 1) % ht->num_slots;
    }

    return false;
}

Value HashTableGet(HashTable ht, Key key) {
    int idx = hash(key, ht->num_slots);

    for (int i = 0; i < ht->num_slots; i++) {
        if (ht->slots[idx].empty) break;
        if (key_compare(ht->slots[idx].key, key)) return ht->slots[idx].value;
        
        idx = (idx + 1) % ht->num_slots;
    }

    return NULL;
}

////////////////////////////// HELPER FUNCTIONS ////////////////////////////////

/**
 * Hash function for the hash table.
 * Sums the ASCII value of each character of the key.
 */
int hash(Key key, int num_slots) {
    int hash = 0;
    while (*key) hash += *key++;
    return hash % num_slots;
}


/**
 * Checks if two keys are equal.
 */
bool key_compare(Key key1, Key key2) {
    return strcmp(key1, key2) == 0;                     
}
