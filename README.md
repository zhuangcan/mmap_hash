# mmap_hash

这是一个适用于共享内存(mmap方式)的hash table。不同于general hash动态申请内存，理论上条目数无上限，mmap_hash需要修改

HASH_N_BUCKETS以及HASH_N_LIST来分别定义hash桶的个数以及桶的深度。

这么做的原因是避免动态内存申请，否则无法获得连续的内存做映射。

其他限制：

	key限制为字符串类型

	value限制为int型

编译：
	make

运行：
	sudo ./mmap_write  -- 写hash表
	sudo ./mmap_read   -- 读hash表
	由于是文件映射至内存，即使write先退出了，数据依然保存在文件，read后运行也可以读取到数据

