#include "../include/client.h"

int main(int argc, const char* argv[])
{
	if (2 != argc)
	{
		fprintf(stderr, "FORMAT: ./APP IP \n");
		return -1;
	}

	seraddr.sin_family = AF_INET;
	seraddr.sin_port = htons(9999);
	seraddr.sin_addr.s_addr = inet_addr(argv[1]);

	//order的操作小于128
	char order[SIZE];

	while (1)
	{
		printf(">>tftp:");
		fgets(order, SIZE, stdin);
		order[strlen(order) - 1] = 0;

		if (strncmp(order, "get ", 4) == 0)
			do_get(order);
		else if (strncmp(order, "put ", 4) == 0)
			do_put(order);
		else if (strncmp(order, "list", 4) == 0 || strncmp(order, "ls", 2) == 0)
			do_list(order);
		else if (strncmp(order, "help", 4) == 0)
			do_help();
		else if (strncmp(order, "exit", 4) == 0)
		{
			printf("Thanks for your welcom, ByeBye...\n");
			exit(0);
		}
		else if (strncmp(order, "clear", 5) == 0 || strncmp(order, "cls", 3) == 0)
			system("clear");
		else if (strncmp(order, "lock", 4) == 0)
			do_lock();//对本地文件进行加密
		else printf("%s: command can not found.\n%s: or the input format is not wrong.\n", order, order);
		memset(order, 0, sizeof(order));
	}

	return 0;
}
