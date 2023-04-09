#include "ls.h"

int main(int argc, const char *argv[]) {
	//默认文件夹路径
	char *dir_path = ".";
	if (2 == argc) dir_path = argv[1];

	DIR *dirp = opendir(dir_path);
	if (NULL == dirp) {
		printf("ls: can not access '%s': %s", dir_path, strerror(errno));
		return -1;
	}
	
	//ls的格式输出格式 文件大小 最后访问时间 上一次修改时间 文件类型 文件名
	puts("FILE_SIZE\tLAST_ACCESS_TIME\t\tLAST_MODIFY_TIME\t\tFILE_TYPE\tFILE_NAME");
	while (1) { 
		direntp = readdir(dirp);//默认是读取"."下的所有文件
		if (NULL == direntp) break;

		//清除对应的缓存
		memset(&info, 0, sizeof(info));
		memset(file_path, 0, sizeof(file_path));
		memset(atime, 0, sizeof(atime));
		memset(mtime, 0, sizeof(mtime));
		memset(type, 0, sizeof(type));

		//不输入参数应该file_path中应该是"."
		strcpy(file_path, dir_path);
		if (strcmp(direntp->d_name, ".") == 0 || strcmp(direntp->d_name, "..") == 0) continue;

		//追加当前的文件名形成完成的文件路径
		strcat(file_path, "/");
		//printf("file_path = %s\n", file_path);
		strcat(file_path, direntp->d_name);

		//得到当前文件的属性
		stat(file_path, &info);
		//设置大小
		size = info.st_size;
		//最近的访问时间
		strcpy(atime, ctime(&info.st_atime));
		atime[strlen(atime) - 1] = 0;
		//最近的修改时间
		strcpy(mtime, ctime(&info.st_mtime));
		mtime[strlen(mtime) - 1] = 0;
		//文件的权限类型等信息
		st_mode = info.st_mode;

		if (S_ISREG(st_mode))       strcpy(type, "普通");
		else if (S_ISDIR(st_mode)) 	strcpy(type, "目录");
		else if (S_ISCHR(st_mode)) 	strcpy(type, "字符设备");
		else if (S_ISBLK(st_mode)) 	strcpy(type, "块设备");
		else if (S_ISFIFO(st_mode)) strcpy(type, "管道");
		else if (S_ISLNK(st_mode)) 	strcpy(type, "链接");
		else if (S_ISSOCK(st_mode)) strcpy(type, "套接字");
		else                        strcpy(type, "未知");

		printf("%d\t\t%s\t%s\t%s\t\t%s\n", size, atime, mtime, type, direntp->d_name);
	}
	closedir(dirp);

	return 0;
}
