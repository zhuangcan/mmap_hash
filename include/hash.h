#ifndef __HASH_H__
#define __HASH_H__
/*
 * This code is a fairly straightforward hash
 * table implementation using Bob Jenkins'
 * hash function.
 *
 * Hash tables are used in OpenVPN to keep track of
 * client instances over various key spaces.
 */
#define HASH_N_BUCKETS      256
#define HASH_N_LIST         256
#define HASH_KEY_LEN        64

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
    struct hash_element list[HASH_N_LIST];
};

struct hash
{
    int n_buckets;
    int n_elements;
    uint32_t iv;
    struct hash_bucket buckets[HASH_N_BUCKETS];
};

uint32_t
general_hash_function(const void *key, uint32_t iv);

bool
general_compare_function(const void *key1, const void *key2);

int
hash_lookup(struct hash *hash, const void *key);

bool
hash_add(struct hash *hash, const void *key, int value, bool replace);

bool
hash_remove(struct hash *hash, const void *key);

#endif /* __HASH_H__ */
