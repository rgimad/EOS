/*
*    EOS - Experimental Operating System
*    VFS (virtual file system) module
*/
#include <kernel/fs/vfs.h>
#include <kernel/mm/kheap.h>
#include <kernel/tty.h>

#include <libk/string.h>

#define MOUNTPOINTS_SIZE 100

int __vfs_init = 0;
int   vfs_lastmnt = 0;
vfs_mount_info_t **vfs_mount_points = 0;


void vfs_mount_list()
{
	for (int i = 0; i < vfs_lastmnt; ++i)
	{

		tty_printf("\n%s on %s type ", vfs_mount_points[i]->fs->dev->name, vfs_mount_points[i]->location);

		if(vfs_mount_points[i]->fs->fs_type == 0)
			tty_printf("initrd");
		else if(vfs_mount_points[i]->fs->fs_type == 1)
			tty_printf("ext2");
		else if(vfs_mount_points[i]->fs->fs_type == 2)
			tty_printf("ext3");
		else if(vfs_mount_points[i]->fs->fs_type == 3)
			tty_printf("ext4");
		else if(vfs_mount_points[i]->fs->fs_type == 4)
			tty_printf("fat32");
		else
			tty_printf("none");
	}
}


int vfs_mount(vfs_storage_dev_t *dev, vfs_filesystem_handles_t* fs_handles, int type, char* location, int block_size)
{

	for (int i = 0; i < vfs_lastmnt; ++i)
	{
		if(strcmp(vfs_mount_points[i]->location, location) == 0)
		{
			tty_printf("\nVFS: Device %s already mounted.", location);
			return 0;
		}
	}

	vfs_mount_info_t *mnt = (vfs_mount_info_t*)kheap_malloc(sizeof(vfs_mount_info_t));
	mnt->fs = (vfs_filesystem_t*)kheap_malloc(sizeof(vfs_filesystem_t));//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	mnt->fs->dev 		= dev;
	mnt->fs->block_size = block_size;
	mnt->fs_handles = fs_handles;
	mnt->fs->fs_type    = type;
	mnt->location 		= location;

	vfs_mount_points[vfs_lastmnt] = mnt;
	vfs_lastmnt++;
	tty_printf("\nVFS: Mounted new device at %s\n", location);
	return 1;
}


int vfs_mount_find(char* path, int *filename_add)
{
	char* a = (char*)kheap_malloc(strlen(path) + 1);
	memset(a, 0, strlen(path) + 1);
	memcpy(a, path, strlen(path) + 1);
	
	int at = 0;
	if(a[strlen(a)] == '/') str_bksp(a, '/');
	while(1)
	{
		for(int i = 0; i < MOUNTPOINTS_SIZE; ++i)
		{
			if(!vfs_mount_points[i])
			{
				break;
			}
			if(strcmp(vfs_mount_points[i]->location, a) == 0)
			{
				/* Adjust the orig to make it relative to fs/dev */
				*filename_add = strlen(a) - 1;
				kheap_free(a);
				return i;
			}
		}
		if(strcmp(a, "/") == 0)
		{
			break;
		}
		str_bksp(a, '/');
	}
	return 0;
}


int vfs_read(char* filename, int offset, int size, void* buf)
{
	int a = 0;
	int mntn = vfs_mount_find(filename, &a);
	filename += a + 1;

	if(vfs_mount_points[mntn]->fs_handles->read == 0)
	{
		return 0;
	}
 
	vfs_mount_points[mntn]->fs_handles->read(filename, offset, size, vfs_mount_points[mntn]->fs, buf);

	return 1;
} 


int vfs_get_size(char* filename)
{
	int a = 0;
	int mntn = vfs_mount_find(filename, &a);
	filename += a + 1;

	if(vfs_mount_points[mntn]->fs_handles->read == 0)
	{
		return 0;
	}
 
	return vfs_mount_points[mntn]->fs_handles->get_size(filename, vfs_mount_points[mntn]->fs);
} 

int vfs_is_dir(char* filename)
{
	int a = 0;
	int mntn = vfs_mount_find(filename, &a);
	filename += a + 1;

	if(vfs_mount_points[mntn]->fs_handles->is_dir == 0)
	{
		return 0;
	}
 
	return vfs_mount_points[mntn]->fs_handles->is_dir(filename, vfs_mount_points[mntn]->fs);
}


int vfs_write(char* filename, int offset, int size, void* buf)
{
	int a = 0;
	int mntn = vfs_mount_find(filename, &a);
	filename += a + 1;

	if(vfs_mount_points[mntn]->fs_handles->write == 0)
	{
		return 0;
	}

	vfs_mount_points[mntn]->fs_handles->write(filename, offset, size, vfs_mount_points[mntn]->fs, buf);

	return 1;
}


int vfs_mkdir(char* filename, char* path, uint16_t perms)
{
	int a = 0;
	int mntn = vfs_mount_find(path, &a);
	path += a + 1;

	if(vfs_mount_points[mntn]->fs_handles->mkdir == 0)
	{
		return 0;
	}

	vfs_mount_points[mntn]->fs_handles->mkdir(path, filename, vfs_mount_points[mntn]->fs, perms);

	return 1;
}


int vfs_mkfile(char* filename, char* path, uint16_t perms)
{
	int a = 0;
	int mntn = vfs_mount_find(path, &a);
	path += a + 1;

	if(vfs_mount_points[mntn]->fs_handles->mkfile == 0)
	{
		return 0;
	}

	vfs_mount_points[mntn]->fs_handles->mkfile(filename, path, vfs_mount_points[mntn]->fs, perms);

	return 1;
}

int vfs_list(char* path, void* buf)
{
	if(strcmp(path, "/") == 0)
	{
		for (int i = 0; i < vfs_lastmnt; ++i)
		{

			tty_printf(" %s ", vfs_mount_points[i]->location);
		}

		return 1;
	}

	int a = 0;
	int mntn = vfs_mount_find(path, &a);
	path += a + 1;

	if(vfs_mount_points[mntn]->fs_handles->readdir == 0)
	{
		return 0;
	}

	vfs_mount_points[mntn]->fs_handles->readdir(path, vfs_mount_points[mntn]->fs, buf);

	return 1;
}


int vfs_rm(char* filename)
{
	int a = 0;
	int mntn = vfs_mount_find(filename, &a);
	filename += a + 1;

	if(vfs_mount_points[mntn]->fs_handles->rm == 0)
	{
		return 0;
	}

	vfs_mount_points[mntn]->fs_handles->rm(filename, vfs_mount_points[mntn]->fs);

	return 1;
}


int vfs_exists(char* filename)
{
	int a = 0;
	int mntn = vfs_mount_find(filename, &a);
	filename += a + 1;

	if(vfs_mount_points[mntn]->fs_handles->read == 0)
	{
		return 0;
	}

	return vfs_mount_points[mntn]->fs_handles->exists(filename, vfs_mount_points[mntn]->fs);
}


void vfs_init()
{
	//tty_printf("\nVFS: Allocating memory for structures.");    
	vfs_mount_points = (vfs_mount_info_t **)kheap_malloc(sizeof(vfs_mount_info_t) * MOUNTPOINTS_SIZE);
	__vfs_init = 1;
}