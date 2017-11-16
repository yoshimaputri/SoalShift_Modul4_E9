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
#include <sys/stat.h>
#include <sys/time.h>
#include <syslog.h>

static const char *dirpath = "/home/yoshi/SoalShift_Modul4_E9/Downloads";

static int xmp_getattr(const char *path, struct stat *stbuf)
{
  int res;
  char fpath[1000];
  char newfile[100];
printf("path : %s, len : %d\n", path, strlen(path));
if(strcmp(path, "/") != 0){
memcpy(newfile, path, strlen(path) - 5);
newfile[strlen(path) - 5] = '\0';
} else {
  memcpy(newfile, path, strlen(path));
}
  printf("newfile: %s\n", newfile);
  sprintf(fpath,"%s%s",dirpath, path);
  res = lstat(fpath, stbuf);

  if (res == -1)
    return -errno;

  return 0;
}

static int xmp_rename(const char *from, const char *to)
{
    int res;
    char ffrom[1000];
    char fto[1000];
    system("mkdir /home/yoshi/SoalShift_Modul4_E9/simpanan");

    char direktori[] = "/home/yoshi/SoalShift_Modul4_E9/simpanan";

    sprintf(ffrom,"%s%s",dirpath,from);
    sprintf(fto,"%s%s",direktori,to);
    res = rename(ffrom, fto);

    if(res == -1)
    return -errno;

    return 0;
}

static int xmp_mkdir(const char *path, mode_t mode)
{
	int res;
	char dest[1000];
	sprintf(dest, "%s%s", dirpath,path);
	res = mkdir(dest, mode);
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
    char *newName;
    newName = strcat(de->d_name, ".copy");
    struct stat st;
    memset(&st, 0, sizeof(st));
    st.st_ino = de->d_ino;
    st.st_mode = de->d_type << 12;
    res = (filler(buf, newName, &st, 0));
      if(res!=0) break;
  }

  closedir(dp);
  return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
 	   struct fuse_file_info *fi)
{
  char fpath[1000];
  char newFile[100];
  if(strcmp(path,"/") == 0)
  {
    memcpy(newFile, path, strlen(path));
//    path=dirpath;
    sprintf(fpath,"%s",newFile);
  }
  else {
    memcpy(newFile, path, strlen(path) - 5);
    newFile[strlen(path) - 5] = '\0';

    sprintf(fpath, "%s%s",dirpath,newFile);
  }
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

static struct fuse_operations xmp_oper = {
  .getattr  = xmp_getattr,
  .rename   = xmp_rename,
  .mkdir    = xmp_mkdir,
  .readdir  = xmp_readdir,
  .read   = xmp_read,
};

int main(int argc, char *argv[])
{
  umask(0);
  return fuse_main(argc, argv, &xmp_oper, NULL);
}
