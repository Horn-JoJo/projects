#include "../include/server.h"

//默认PATH 现在要改成一个指定的path然后进行遍历操作
//首先不需要指定路径，先固定死！
//然后备份list_files() 将其效果修改为ls的效果显示！！！

void get_file(char order[])
{
	//printf("%s:filename : %s\n", __func__, filename);
	char *file = order;
	while (*file != 0) 
	{
		if (*file != ' ') break;
		file++;
	}
	if (*file == ' ') return;
	printf("filename = %s\n", file);
	
	//1)开始初始化为资源的根路径
	char path[128] = RES_ROOT;
	
	//2)将当前登录的用户名追加到根路径后
	//称为用户独立所属的资源路径(这一步暂时不考虑)
	/*strcat(path, username)*/
	/*strcat(path, "/");*/

	//3)追加文件名
	strcat(path, file);
	FILE *fp = fopen(path, "r");
	if (NULL == fp)
	{
		perror("fopen");
		return;
	}

	while (1)
	{
		memset(buf, 0, sizeof(buf));
		int size = fread(buf, sizeof(char), MAX, fp);
		if (0 == size)
		{
			fclose(fp);
			break;
		}
		send(confd, buf, size, 0);
	}
}

void list_files()
{
	
	//需要显示当前用户资源路径下的文件
	//因此需要追加用户名到资源根路径后
	/*char path[128] = RES_ROOT;*/
	/*strcat(RES_ROOT, username);*/
	dirp = opendir(RES_ROOT);//打开默认的文件资源存路径
	if (NULL == dirp)
	{
		printf("ls: can not access '%s': %s", RES_ROOT, strerror(errno));
		return;
	}

	memset(buf, 0, sizeof(buf));
	direntp = NULL;

	//ls的输出格式 文件大小 最后访问时间 上一次修改时间 文件类型 文件名
	//puts("FILE_SIZE\tLAST_ACCESS_TIME\t\tLAST_MODIFY_TIME\t\tFILE_TYPE\tFILE_NAME");
	snprintf(buf, 1024, "FILE_SIZE\tLAST_ACCESS_TIME\t\tLAST_MODIFY_TIME\t\tFILE_TYPE\tFILE_NAME\n");
	send(confd, buf, strlen(buf), 0);

	while (1)
	{
		direntp = readdir(dirp);//默认读取的是../res下的内容
		if (NULL == direntp) break;

		//清除对应的缓存
		memset(&info, 0, sizeof(info));
		memset(file_path, 0, sizeof(file_path));
		memset(atime, 0, sizeof(atime));
		memset(mtime, 0, sizeof(mtime));
		memset(type, 0, sizeof(type));

		//当前没能修改路径，因此file_path的路径为默认的PATH宏常量指定的路径
		//strcpy(file_path, dir_path);
		strcpy(file_path, RES_ROOT);
		if (strcmp(direntp->d_name, ".") == 0 || strcmp(direntp->d_name, "..") == 0) continue;

		//追加当前的文件名形成完成的文件路径
		strcat(file_path, "/");
		strcat(file_path, direntp->d_name);

		//得到文件的属性
		stat(file_path, &info);
		//设置大小
		size = info.st_size;
		//最近的访问时间
		strcpy(atime, ctime(&info.st_atime));
		atime[strlen(atime) - 1] = 0;
		//最近的修改时间
		strcpy(mtime, ctime(&info.st_mtime));
		mtime[strlen(mtime) - 1] = 0;
		//文件的类型和权限等信息
		st_mode = info.st_mode;

		if (S_ISREG(st_mode))       strcpy(type, "普通");
		else if (S_ISDIR(st_mode)) 	strcpy(type, "目录");
		else if (S_ISCHR(st_mode)) 	strcpy(type, "字符设备");
		else if (S_ISBLK(st_mode)) 	strcpy(type, "块设备");
		else if (S_ISFIFO(st_mode)) strcpy(type, "管道");
		else if (S_ISLNK(st_mode)) 	strcpy(type, "链接");
		else if (S_ISSOCK(st_mode)) strcpy(type, "套接字");
		else                        strcpy(type, "未知");

		//服务器打印信息后续会使用会将输出日志重定向到log日志文件中！！！
		//printf("%d\t\t%s\t%s\t%s\t\t%s\n", size, atime, mtime, type, direntp->d_name);

		//这里当目录内容过多文件时，可能buf一次性装不完到完整的内容！！！,
		//优化方法：每次只发送一条数据，然后这样的会双方的通信协议需要改变！！！(后续再修改，可以直接参考员原来的代码！！！)
		/*snprintf(tmp, 1024, "%d\t\t%s\t%s\t%s\t\t%s\n", size, atime, mtime, type, direntp->d_name);*/
		/*strcat(buf, tmp);*/
		memset(buf, 0, sizeof(buf));
		snprintf(buf, 1024, "%d\t\t%s\t%s\t%s\t\t%s\n", size, atime, mtime, type, direntp->d_name);
		send(confd, buf, strlen(buf), 0);
	}
	closedir(dirp);
}

void put_file(char order[], int len)
{
	char filename[64] = {0};	
	/*char path[128] = {0};*/
	/*strcpy(path, PATH);*/
	char path[128] = RES_ROOT;
	int i, j = 0;
	//从第一个空格开始
	for (i = 0; i < len; i++)
	{
		if ('#' == order[i]) break;
		if (' ' == order[i]) continue;
		filename[j++] = order[i];
	}
	
	/*需要获取当前用户资源路径下
	 *strcat(path, username);
	 *strcat(path, "/");
	 * */
	strcat(path, filename);
	FILE *fp = fopen(path, "w+");
	if (NULL == fp)
	{
		perror("fopen");
		return;
	}

	char *p = order + i + 1;
	fwrite(p, sizeof(char), len - i - 1, fp);

	while (1)
	{
		memset(buf, 0, sizeof(buf));
		int size =recv(confd, buf, MAX, 0);
		if (0 == size) break;
		fwrite(buf, sizeof(char), size, fp);
	}
	fclose(fp);
}
