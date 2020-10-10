#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#define FUSE_USE_VERSION 30
#define _FILE_OFFSET_BITS 64

static void init_module();

static int do_getattr( const char *path, struct stat *st );

static int do_readdir( const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi );

static int do_read( const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi );

static int do_write(const char* path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi);
