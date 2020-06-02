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

void new_hash_test(void)
{
    struct hash *stat_hash = NULL;
    stat_hash = malloc(sizeof(struct hash));
    memset(stat_hash, 0, sizeof(struct hash));

    if(hash_add(stat_hash, "zhangsan", 1, true))
    {
        printf("key 'zhuangcan' value '1' add ok!\n");
    }
    if(hash_add(stat_hash, "lisi", 2, true))
    {
        printf("key 'lisi' value '2' add ok!\n");
    }

    int ret;
    ret = hash_lookup(stat_hash, "zhangsan");
    if (ret > 0)
    {
        printf("key 'zhangsan' found, value is '%d'!\n", ret);
    }
    ret = hash_lookup(stat_hash, "lisi");
    if (ret > 0)
    {
        printf("key 'lisi' found, value is '%d'!\n", ret);
    }

    if (hash_remove(stat_hash, "zhangsan"))
    {
        printf("key 'zhuangcan' remove ok!\n");
    }
    if (hash_remove(stat_hash, "lisi"))
    {
        printf("key 'lisi' remove ok!\n");
    }

    ret = hash_lookup(stat_hash, "zhangsan");
    if (ret > 0)
    {
        printf("key 'zhangsan' found, value is '%d'!\n", ret);
    }
    else
    {
        printf("key 'zhangsan' doesn't found!\n");
    }
    ret = hash_lookup(stat_hash, "lisi");
    if (ret > 0)
    {
        printf("key 'lisi' found, value is '%d'!\n", ret);
    }
    else 
    {
        printf("key 'lisi' doesn't found!\n");
    }

    return;
}

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
    
    if(hash_add(login_hash, "zhangsan", 1, true))
    {
        printf("key 'zhuangcan' value '1' add ok!\n");
    }
    if(hash_add(login_hash, "lisi", 2, true))
    {
        printf("key 'lisi' value '2' add ok!\n");
    }

    int ret;
    ret = hash_lookup(login_hash, "zhangsan");
    if (ret > 0)
    {
        printf("key 'zhangsan' found, value is '%d'!\n", ret);
    }
    ret = hash_lookup(login_hash, "lisi");
    if (ret > 0)
    {
        printf("key 'lisi' found, value is '%d'!\n", ret);
    }

    printf("initial over!\n");
    munmap(login_hash, map_size);
    printf("umap ok!\n");
    
	return 0;
}
