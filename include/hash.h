#ifndef __HASH_H__
#define __HASH_H__
/*
 * notice that the max items should be HASH_NUM_BUCKETS * HASH_BUCKET_ENTRIES,
 * but we can not ensure reach the 100% usage.
 */
#define HASH_NUM_BUCKETS        ((1 << 16) >> 3)
#define HASH_BUCKET_ENTRIES     (1 << 3)
#define HASH_KEY_LEN            64

#define HASH_ELEMENT_UESED  0x5a5a3434
#define HASH_ELEMENT_FREE   0

struct hash_element
{
    char key[HASH_KEY_LEN];
    int value;
    unsigned int hash_value;
    int flag;
};

struct hash_bucket
{
    struct hash_element list[HASH_BUCKET_ENTRIES];
};

struct hash
{
    int n_buckets;
    int n_elements;
    uint32_t iv;
    struct hash_bucket buckets[HASH_NUM_BUCKETS];
};

uint32_t
general_hash_function(const void *key, uint32_t iv);

bool
general_compare_function(const void *key1, const void *key2);

struct hash_element *
hash_lookup(struct hash *h, const void *key);

bool
hash_add(struct hash *h, const void *key, int value, bool replace);

bool
hash_remove(struct hash *h, const void *key);

#endif /* __HASH_H__ */
