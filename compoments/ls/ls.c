#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>

int main(int argc, const char *argv[]) {
	//默认文件夹路径
	char *dir_path = ".";
	if (2 == argc) dir_path = argv[1];

	DIR *dirp = opendir(dir_path);
	if (NULL) {
		printf("ls: can not access '%s': %s", dir_path, strerror(errno));
		return -1;
	}

	char file_path[1024];
	struct stat info;
	memset(&info, 0, sizeof(info));
	memset(file_path, 0, sizeof(file_path));
	struct dirent *direntp = NULL;

	//ls的格式输出
	puts("FILE_SIZE\tLAST_ACCESS_TIME\t\tLAST_MODIFY_TIME\t\tFILE_TYPE\tFILE_NAME");
	while (1) { 
		direntp = readdir(dirp);
		if (NULL == direntp) break;
		strcpy(file_path, dir_path);
		if (strcmp(file_path, ".") != 0)
			strcat(file_path, direntp->d_name);
		stat(file_path, &info);
		//printf("file_path : %s\n", file_path);

		mode_t st_mode = info.st_mode;
		char type[128] = {0};
		int size = info.st_size;

		char atime[128] = {0};
		strcpy(atime, ctime(&info.st_atime));
		atime[strlen(atime) - 1] = 0;
		char mtime[128] = {0};
		strcpy(mtime, ctime(&info.st_mtime));
		mtime[strlen(mtime) - 1] = 0;

		if (S_ISREG(st_mode))       strcpy(type, "普通");
		else if (S_ISDIR(st_mode)) 	strcpy(type, "目录");
		else if (S_ISCHR(st_mode)) 	strcpy(type, "字符设备");
		else if (S_ISBLK(st_mode)) 	strcpy(type, "块设备");
		else if (S_ISFIFO(st_mode)) strcpy(type, "管道");
		else if (S_ISLNK(st_mode)) 	strcpy(type, "链接");
		else if (S_ISSOCK(st_mode)) strcpy(type, "套接字");
		else                        strcpy(type, "未知");

		printf("%d\t\t%s\t%s\t%s\t\t%s\n", size, atime, mtime, type, direntp->d_name);

		memset(&info, 0, sizeof(info));
		memset(file_path, 0, sizeof(file_path));
	}
	closedir(dirp);

	return 0;
}
