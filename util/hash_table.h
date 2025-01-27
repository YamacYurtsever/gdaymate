// Hash Table Interface

#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdbool.h>

typedef struct hash_table *HashTable;
typedef char *Key;
typedef char *Value;

/**
 * Creates a new hash table.
 */
HashTable HashTableNew(int num_slots);

/**
 * Frees a hash table.
 */
void HashTableFree(HashTable table);

/**
 * Inserts a new key value pair into a hash table. 
 * Replaces the value if key already exists.
 */
void HashTableInsert(HashTable table, Key key, Value value);

/**
 * Returns whether a hash table contains the given key.
 */
bool HashTableContains(HashTable table, Key key);

/**
 * Returns the value that the given key maps to in the hash table.
 * Assumes that the key exists.
 */
Value HashTableGet(HashTable table, Key key);

#endif
