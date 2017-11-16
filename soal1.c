#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/stat.h>

static const char *dirpath = "/home/yoshi/SoalShift_Modul4_E9/Documents";
char str[3][5] = {".pdf",".doc",".txt"};
struct stat st = {0};
static int xmp_getattr(const char *path, struct stat *stbuf)
{
  int res;
  char fpath[1000];
  char newFile[100];
  printf("path   : %s, len: %d\n", path, strlen(path));
  if (strcmp(path, "/") != 0) {
    memcpy(newFile, path, strlen(path) - 4);
    newFile[strlen(path) - 4] = '\0';
  } else {
    memcpy(newFile, path, strlen(path));
  }
  printf("newFile: %s\n", newFile);
  sprintf(fpath,"%s%s",dirpath, newFile);
  res = lstat(fpath, stbuf);

  if (res == -1)
    return -errno;

  return 0;
}
static int xmp_open(const char *path, int flags)
{
    int res;
    int counter=0;
    char fpath[1000];
    char path2[1000];
    char path3[1000];
    sprintf(fpath,"%s%s", dirpath, path);
    sprintf(path2,"%s%s","/home/yoshi/SoalShift_Modul4_E9/Documents",path);
    int i;
    for(i=0;i<strlen(path);i++)
    {
        if(path[i]=='/')
        {
            counter++;
        }
    }

    if(counter==2)
    {
        sprintf(path3,"%s%s","chmod 444 ",path2);
        system(path3);
        char temp[]="zenity --info --text=\"";
                strcat(temp,"File Tidak Bisa Dibuka");
                strcat(temp,"\"");
                system(temp);
    res = open(fpath, flags);
    if(res == -1)
        return -errno;
    }

    close(res);
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
    int flag = 0;
    newName = de->d_name;
    
    //newName = strcat(de->d_name, ".bak");
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
    memcpy(newFile, path, strlen(path) - 4);
    newFile[strlen(path) - 4] = '\0';

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
  .open   = xmp_open,
  .readdir  = xmp_readdir,
  .read   = xmp_read,
};

int main(int argc, char *argv[])
{
  umask(0);
  return fuse_main(argc, argv, &xmp_oper, NULL);
}