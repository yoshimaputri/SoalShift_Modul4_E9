#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>

static const char *dirpath = "/home/yoshi/SoalShift_Modul4_E9/Downloads";

static int xmp_getattr(const char *path, struct stat *stbuf)
{
  int res;
  char fpath[1000];
  sprintf(fpath,"%s%s",dirpath, path);
  res = lstat(fpath, stbuf);

  if (res == -1)
    return -errno;

  return 0;
}

static int xmp_mkdir(const char *path, mode_t mode)
{
	int res;
	char fpath[1000];
	sprintf(fpath, "%s%s", dirpath,path);
	res = mkdir(fpath, mode);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
 	      off_t offset, struct fuse_file_info *fi)
{
 char fpath[1000];
  if(strcmp(path,"/") == 0)
  {
    path=dirpath;
    sprintf(fpath,"%s",path);
  }
  else sprintf(fpath, "%s%s",dirpath,path);
  int res = 0;

  DIR *dp;
  struct dirent *de;

  (void) offset;
  (void) fi;

  dp = opendir(fpath);
  if (dp == NULL)
    return -errno;

  while ((de = readdir(dp)) != NULL) {
    struct stat st;
    memset(&st, 0, sizeof(st));
    st.st_ino = de->d_ino;
    st.st_mode = de->d_type << 12;
    res = (filler(buf, de->d_name, &st, 0));
      if(res!=0) break;
  }

  closedir(dp);
  return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
 	   struct fuse_file_info *fi)
{
    char fpath[1000];
    if(strcmp(path,"/") == 0)
    {
        path=dirpath;
        sprintf(fpath,"%s",path);
    }
    else sprintf(fpath, "%s%s",dirpath,path);
    int res = 0;
  int fd = 0 ;

    (void) fi;
    fd = open(fpath, O_RDONLY);
    if (fd == -1)
        return -errno;

    res = pread(fd, buf, size, offset);
    if (res == -1)
        res = -errno;

    close(fd);
    return res;
}

static int xmp_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    int fd;
    int res;
    char fpath[1000], temp[1000];
    const char *newnamedir = "/simpanan/";
    sprintf(temp, "mkdir %s%s", dirpath, newnamedir);
    system(temp);
   char ffile[1000];
    sprintf(ffile, "cp %s%s %s%s%s", dirpath,path, dirpath,newnamedir, path);
    system(ffile);
    sprintf(fpath, "%s%s%s", dirpath,newnamedir, path);
    fd = open(fpath, O_WRONLY);
    if(fd == -1)
    {
        return -errno;
    }

    res = pwrite(fd, buf, size, offset);
    if(res == -1)
        res = -errno;

    close(fd);
    return res;
}

static int xmp_truncate(const char *path, off_t size)
{
	int res;

	res = truncate(path, size);
	if (res == -1)
		return -errno;

	return 0;
}

static struct fuse_operations xmp_oper = {
  .getattr  = xmp_getattr,
  .mkdir    = xmp_mkdir,
  .readdir  = xmp_readdir,
  .read   = xmp_read,
  .write  = xmp_write,
  .truncate  = xmp_truncate,
};

int main(int argc, char *argv[])
{
  umask(0);
  return fuse_main(argc, argv, &xmp_oper, NULL);
}
