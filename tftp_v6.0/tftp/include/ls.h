#ifndef _LS_H
#define _LS_H
#include <time.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>

//指定的ls的目录路径(只能ls 目录文件)
char *dir_path;
//目录指针
DIR *dirp;
//当前遍历的ls[目录]下的某个文件路径
char file_path[1024];
//属性结构体
struct stat info;
//当前遍历的ls[目录]下的某个文件的大小
int size;
//最近的访问时间
char atime[128];
//最近的修改时间
char mtime[128];
//文件的类型
char type[128];
//文件的属性和权限
mode_t st_mode;
//指向遍历目录下的某个文件的指针
struct dirent *direntp;

#endif
