#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include "hash.h"

/*
 * Return the next largest power of 2
 * or u if u is a power of 2.
 */
static inline size_t
adjust_power_of_2(size_t u)
{
    size_t ret = 1;

    while (ret < u)
    {
        ret <<= 1;
        assert(ret > 0);
    }

    return ret;
}

/*
 * --------------------------------------------------------------------
 * hash() -- hash a variable-length key into a 32-bit value
 * k     : the key (the unaligned variable-length array of bytes)
 * len   : the length of the key, counting by bytes
 * level : can be any 4-byte value
 * Returns a 32-bit value.  Every bit of the key affects every bit of
 * the return value.  Every 1-bit and 2-bit delta achieves avalanche.
 * About 36+6len instructions.
 *
 * The best hash table sizes are powers of 2.  There is no need to do
 * mod a prime (mod is sooo slow!).  If you need less than 32 bits,
 * use a bitmask.  For example, if you need only 10 bits, do
 * h = (h & hashmask(10));
 * In which case, the hash table should have hashsize(10) elements.
 *
 * If you are hashing n strings (uint8_t **)k, do it like this:
 * for (i=0, h=0; i<n; ++i) h = hash( k[i], len[i], h);
 *
 * By Bob Jenkins, 1996.  bob_jenkins@burtleburtle.net.  You may use this
 * code any way you wish, private, educational, or commercial.  It's free.
 *
 * See http://burlteburtle.net/bob/hash/evahash.html
 * Use for hash table lookup, or anything where one collision in 2^32 is
 * acceptable.  Do NOT use for cryptographic purposes.
 *
 * --------------------------------------------------------------------
 *
 * mix -- mix 3 32-bit values reversibly.
 * For every delta with one or two bit set, and the deltas of all three
 * high bits or all three low bits, whether the original value of a,b,c
 * is almost all zero or is uniformly distributed,
 * If mix() is run forward or backward, at least 32 bits in a,b,c
 * have at least 1/4 probability of changing.
 * If mix() is run forward, every bit of c will change between 1/3 and
 * 2/3 of the time.  (Well, 22/100 and 78/100 for some 2-bit deltas.)
 * mix() was built out of 36 single-cycle latency instructions in a
 * structure that could supported 2x parallelism, like so:
 *    a -= b;
 *    a -= c; x = (c>>13);
 *    b -= c; a ^= x;
 *    b -= a; x = (a<<8);
 *    c -= a; b ^= x;
 *    c -= b; x = (b>>13);
 *    ...
 * Unfortunately, superscalar Pentiums and Sparcs can't take advantage
 * of that parallelism.  They've also turned some of those single-cycle
 * latency instructions into multi-cycle latency instructions.  Still,
 * this is the fastest good hash I could find.  There were about 2^^68
 * to choose from.  I only looked at a billion or so.
 *
 * James Yonan Notes:
 *
 * This function is faster than it looks, and appears to be
 * appropriate for our usage in OpenVPN which is primarily
 * for hash-table based address lookup (IPv4, IPv6, and Ethernet MAC).
 * NOTE: This function is never used for cryptographic purposes, only
 * to produce evenly-distributed indexes into hash tables.
 *
 * Benchmark results: 11.39 machine cycles per byte on a P2 266Mhz,
 *                   and 12.1 machine cycles per byte on a
 *                   2.2 Ghz P4 when hashing a 6 byte string.
 * --------------------------------------------------------------------
 */

#define mix(a,b,c)               \
    {                                \
        a -= b; a -= c; a ^= (c>>13);  \
        b -= c; b -= a; b ^= (a<<8);   \
        c -= a; c -= b; c ^= (b>>13);  \
        a -= b; a -= c; a ^= (c>>12);  \
        b -= c; b -= a; b ^= (a<<16);  \
        c -= a; c -= b; c ^= (b>>5);   \
        a -= b; a -= c; a ^= (c>>3);   \
        b -= c; b -= a; b ^= (a<<10);  \
        c -= a; c -= b; c ^= (b>>15);  \
    }

uint32_t
hash_func(const uint8_t *k, uint32_t length, uint32_t initval)
{
    uint32_t a, b, c, len;

    /* Set up the internal state */
    len = length;
    a = b = 0x9e3779b9;      /* the golden ratio; an arbitrary value */
    c = initval;             /* the previous hash value */

    /*---------------------------------------- handle most of the key */
    while (len >= 12)
    {
        a += (k[0] + ((uint32_t) k[1] << 8)
              + ((uint32_t) k[2] << 16)
              + ((uint32_t) k[3] << 24));
        b += (k[4] + ((uint32_t) k[5] << 8)
              + ((uint32_t) k[6] << 16)
              + ((uint32_t) k[7] << 24));
        c += (k[8] + ((uint32_t) k[9] << 8)
              + ((uint32_t) k[10] << 16)
              + ((uint32_t) k[11] << 24));
        mix(a, b, c);
        k += 12;
        len -= 12;
    }

    /*------------------------------------- handle the last 11 bytes */
    c += length;
    switch (len)            /* all the case statements fall through */
    {
        case 11:
            c += ((uint32_t) k[10] << 24);

        case 10:
            c += ((uint32_t) k[9] << 16);

        case 9:
            c += ((uint32_t) k[8] << 8);

        /* the first byte of c is reserved for the length */
        case 8:
            b += ((uint32_t) k[7] << 24);

        case 7:
            b += ((uint32_t) k[6] << 16);

        case 6:
            b += ((uint32_t) k[5] << 8);

        case 5:
            b += k[4];

        case 4:
            a += ((uint32_t) k[3] << 24);

        case 3:
            a += ((uint32_t) k[2] << 16);

        case 2:
            a += ((uint32_t) k[1] << 8);

        case 1:
            a += k[0];
            /* case 0: nothing left to add */
    }
    mix(a, b, c);
    /*-------------------------------------- report the result */
    return c;
}

uint32_t
general_hash_function(const void *key, uint32_t iv)
{
    return hash_func((const uint8_t *)key, strlen((char *)key) + 1, iv);
}

bool
general_compare_function(const void *key1, const void *key2)
{
    return !strcmp((const char *)key1, (const char *)key2);
}

uint32_t
hash_value(const struct hash *hash, const void *key)
{
    return general_hash_function(key, hash->iv);
}

/* Calc the secondary hash value from the primary hash value of a given key */
uint32_t
secondary_hash_value(const uint32_t primary_hash)
{
	static const unsigned all_bits_shift = 12;
	static const unsigned alt_bits_xor = 0x5bd1e995;

	uint32_t tag = primary_hash >> all_bits_shift;

	return primary_hash ^ ((tag + 1) * alt_bits_xor);
}

struct hash_element *
hash_secondary_lookup_iter(struct hash *h, uint32_t sig, const void *key)
{
    struct hash_bucket *sec_bkt;
    struct hash_element *he;
    int i;

    sec_bkt = &h->buckets[sig & (HASH_NUM_BUCKETS - 1)];
    for (i = 0; i < HASH_BUCKET_ENTRIES; i++)
    {
        he = &sec_bkt->list[i];
        if (he->hash_value == sig && general_compare_function(key, he->key))
        {
            return he;
        }
    }

    return NULL;

}

struct hash_element *
hash_secondary_lookup(struct hash *h, const void *key)
{
    struct hash_element *he = NULL;
    uint32_t sig_alt;
    int i = 0;

    sig_alt = hash_value(h, key);
    for (i = 0; i < HASH_NUM_BUCKETS; i++)
    {        
        he = hash_secondary_lookup_iter(h, sig_alt, key);
        if (he != NULL)
        {
            return he;
        }
        sig_alt = secondary_hash_value(sig_alt);
    }

    return NULL;
}

struct hash_element *
hash_lookup(struct hash *h, const void *key)
{
    return hash_secondary_lookup(h, key);
}

bool
hash_secondary_add_iter(struct hash *h, uint32_t sig, const void *key, int value)
{
    struct hash_bucket *sec_bkt;
    struct hash_element *he;
    int i;

    sec_bkt = &h->buckets[sig & (HASH_NUM_BUCKETS - 1)];
    for (i = 0; i < HASH_BUCKET_ENTRIES; i++)
    {
        he = &sec_bkt->list[i];
        if (he->flag == HASH_ELEMENT_FREE)
        {
            he->value = value;
            strcpy(he->key, key);
            he->hash_value = sig;
            he->flag = HASH_ELEMENT_UESED;
            return true;
        }
    }

    return false;
}

bool
hash_secondary_add(struct hash *h, const void *key, int value)
{
    uint32_t sig_alt;
    bool ret = false;
    int i = 0;

    sig_alt = hash_value(h, key);
    for (i = 0; i < HASH_NUM_BUCKETS; i++)
    {
        ret = hash_secondary_add_iter(h, sig_alt, key, value);
        if (ret == true)
        {
            return true;
        }
        sig_alt = secondary_hash_value(sig_alt);
    }

    return false;
}

bool
hash_add(struct hash *h, const void *key, int value, bool replace)
{
    bool ret = false;
    struct hash_element *he;

    /* 先查是否已有表项 */
    if (he = hash_lookup(h, key))
    {
        if (replace)
        {
            he->value = value;
            ret = true;
        }
    }
    else
    {
        ret = hash_secondary_add(h, key, value);
    }

    return ret;    
}

bool
hash_remove(struct hash *h, const void *key)
{    
    bool ret = false;   

    struct hash_element *he = hash_lookup(h, key);
    if (he != NULL)
    {
        he->flag = HASH_ELEMENT_FREE;
        memset(he->key, 0, HASH_KEY_LEN);
        he->value = 0;
        he->hash_value = 0;
        ret = true;
    }

    return ret;
}



