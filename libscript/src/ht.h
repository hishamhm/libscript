
#ifndef HT_H
#define HT_H

#define HT_BORROW_REFS 0
#define HT_OWN_REFS 1

typedef struct ht_ ht;
typedef struct ht_item_ ht_item;
typedef struct ht_iterator_ ht_iterator;

typedef enum {
   HT_PTR,
   HT_I,
   HT_STR
} ht_type;

typedef union {
   void* ptr;
   int i;
   const char* str;
} ht_key;

typedef int(*ht_hash_fn)(ht*, ht_key);
typedef int(*ht_eq_fn)(ht*, ht_key, ht_key);

struct ht_item_ {
   ht_key key;
   void* value;
   ht_item* next;
};

struct ht_ {
   int size;
   ht_item** buckets;
   int items;
   ht_type type;
   ht_hash_fn hash;
   ht_eq_fn eq;
   int owner;
};

struct ht_iterator_ {
   ht* table;
   int bucket;
   ht_item* item;
};

ht* ht_new(int size, ht_type type, int owner);
void ht_delete(ht* this);
int ht_size(ht* this);
void ht_put(ht* this, ht_key key, void* value);
void* ht_remove(ht* this, ht_key key);
void* ht_take(ht* this, ht_key key);
void* ht_get(ht* this, ht_key key);
void* ht_take_first(ht* this);
void ht_start(ht* this, ht_iterator* iter);
void* ht_iterate(ht_iterator* iter);

#endif
