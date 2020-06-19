#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include "hash.h"

#define LOGIN_STAT_MMAP_DB "/home/mmap.db"

int main()
{
    int fd = -1;
    size_t map_size = sizeof(struct hash);
    fd = open(LOGIN_STAT_MMAP_DB, O_CREAT|O_RDWR|O_TRUNC, 0644);
    if (fd < 0)
    {
        perror("open error");
        return -1;
    }
    struct stat st;
    fstat(fd, &st);
    if (st.st_size != map_size)
    {
        if (ftruncate(fd, map_size) < 0)
        {
            perror("ftruncate error");
            close(fd);
            return -1;
        }
    }

    struct hash *login_hash = NULL;
    login_hash = (struct hash *)mmap(NULL, map_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);

    /* add */
    if(hash_add(login_hash, "zhangsan", 17, true))
        printf("key 'zhangsan' value '17' add ok!\n");
    else
        printf("key 'zhangsan' value '17' add error!\n");
    if(hash_add(login_hash, "lisi", 28, true))
        printf("key 'lisi' value '28' add ok!\n");
    else
        printf("key 'lisi' value '28' add error!\n");
    if(hash_add(login_hash, "wangermazi", 39, true))
        printf("key 'wangermazi' value '39' add ok!\n");
    else
        printf("key 'wangermazi' value '39' add error!\n");
    /* remove */
    if(hash_remove(login_hash, "wangermazi"))
        printf("key 'wangermazi' value remove ok!\n");
    else
        printf("key 'wangermazi' remove error!\n");

    /* lookup */
    struct hash_element *he = NULL;
    he = hash_lookup(login_hash, "zhangsan");
    if (he != NULL)
        printf("key 'zhangsan' found, value is '%d'!\n", he->value);
    else
        printf("key 'zhangsan' dosen't found!\n");
    he = hash_lookup(login_hash, "lisi");
    if (he != NULL)
        printf("key 'lisi' found, value is '%d'!\n", he->value);    
    else
        printf("key 'lisi' dosen't found!\n");
    he = hash_lookup(login_hash, "wangermazi");
    if (he != NULL)
        printf("key 'wangermazi' found, value is '%d'!\n", he->value);
    else
        printf("key 'wangermazi' dosen't found!\n");

    printf("initial over!\n");
    munmap(login_hash, map_size);
    printf("umap ok!\n");
    
	return 0;
}
