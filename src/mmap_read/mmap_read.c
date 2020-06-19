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

int main(void)
{
	int fd = -1;
	size_t map_size = sizeof(struct hash);
	fd = open(LOGIN_STAT_MMAP_DB, O_RDWR, 0644);
	if (fd < 0)
	{
		perror("open error");
		return -1;
	}
	struct hash *login_hash = (struct hash *)mmap(NULL, map_size, PROT_READ, MAP_SHARED, fd, 0);
	if (login_hash == NULL)
	{
		printf("mmap error");
		close(fd);
		return -1;
	}

	struct hash_element *he = NULL;
	he = hash_lookup(login_hash, "zhangsan");
	if (he != NULL)
		printf("key 'zhangsan' found, value is %d \n", he->value);
	else 
		printf("key 'zhangsan' dosen't found\n");
	he = hash_lookup(login_hash, "lisi");
	if (he != NULL)
		printf("key 'lisi' found, value is %d \n", he->value);
	else
		printf("key 'lisi' doesn't found\n");
    he = hash_lookup(login_hash, "wangermazi");
	if (he != NULL)
		printf("key 'wangermazi' found, value is %d \n", he->value);
	else
		printf("key 'wangermazi' doesn't found\n");

	return 0;
}
