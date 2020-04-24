/*
*    EOS - Experimental Operating System
*    VFS (virtual file system) module header
*/
#ifndef _VFS_H
#define _VFS_H

#include <stdbool.h>
#include <stdint.h>


typedef struct vfs_storage_dev_t
{
	int type; // 0 - disk not used, 1 - AHCI, 2 - ATA, 3 - Mass Storage Device, 4 - Other
	int disk_id;
	int partition_id;
	int lba_start;
	int lba_end;
	char *name;

	uint32_t (*read)   (int *, unsigned long long , unsigned long long , uint16_t *);
	uint32_t (*write)  (int *, unsigned long long , unsigned long long , uint16_t *);
} vfs_storage_dev_t;


typedef struct vfs_filesystem_t //structure that represents some filesystem
{
	int fs_type; // 0 - initrd, 1 - ext2, 2 - ext3, 3 - ext4, 4 - fat32
	int block_size;
	vfs_storage_dev_t* dev;
} vfs_filesystem_t;


typedef struct vfs_filesystem_handles_t //structure that contains pointers to some fs's methods
{
	uint32_t (*read)   (char *fn, int *, int  *, vfs_filesystem_t*, void *);
	uint32_t (*write)  (char *fn, int *, int  *, vfs_filesystem_t*, void *);
	uint32_t (*readdir)(char *, vfs_filesystem_t*, void *);
	uint32_t (*mkfile) (char *fn, char *, vfs_filesystem_t*, uint16_t*);
	uint32_t (*mkdir)  (char *fn, char *, vfs_filesystem_t*, uint16_t*);
	uint32_t (*rm)     (char *fn, vfs_filesystem_t*);
	uint32_t (*get_size)  (char *fn, vfs_filesystem_t*);
	uint32_t (*exists)  (char *fn, vfs_filesystem_t*);
	uint32_t (*is_dir)  (char *fn, vfs_filesystem_t*);//added
} vfs_filesystem_handles_t;


typedef struct vfs_mount_info_t //vfs mount point
{
	char *location; // for example /bin
	vfs_filesystem_t* fs;
	vfs_filesystem_handles_t* fs_handles;
} vfs_mount_info_t;

//-------------------------------------------

void vfs_init();

//TODO correct them! corrected
void vfs_mount_list();
int vfs_mount(vfs_storage_dev_t *dev, vfs_filesystem_handles_t* fs_handles, int type, char* location, int block_size);
int vfs_mount_find(char* path, int *filename_add);
int vfs_read(const char* filename, int offset, int size, void* buf);
int vfs_get_size(const char* filename);
int vfs_write(char* filename, int offset, int size, void* buf);
int vfs_mkdir(char* filename, char* path, uint16_t perms);
int vfs_mkfile(char* filename, char* path, uint16_t perms);
int vfs_list(char* path, void* buf);
int vfs_rm(char* filename);
int vfs_exists(const char* filename);
int vfs_is_dir(char* filename);//added
void vfs_get_file_name_from_path(char *fpath, char *buf); // get file name from full path to the file


#endif  // _VFS_H