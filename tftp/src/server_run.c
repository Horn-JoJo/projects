#include "../include/server.h"

int main()
{

	int listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (0 > listenfd)
	{
		perror("socket");
		return -1;
	}

	seraddr.sin_family = AF_INET;
	seraddr.sin_port = htons(9999);
	seraddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (0 > bind(listenfd, (SA *)&seraddr, sizeof(seraddr)))
	{
		perror("bind");
		close(listenfd);
		return -1;
	}

	if (0 > listen(listenfd, 15))
	{
		perror("listen");
		close(listenfd);
		return -1;
	}

	char order[128];
	addrlen = sizeof(cliaddr);

	while (1)
	{
		confd = accept(listenfd, (SA *)&cliaddr, &addrlen);
		if (0 > confd)
		{
			perror("accept");
			close(listenfd);
			return -1;
		}

		memset(order, 0, sizeof(order));

		int size = recv(confd, order, sizeof(order), 0);
		if (0 == size) 
		{
			printf("peer exit.\n");
			close(confd);
			continue;
		}

		if (strncmp(order, "get ", 4) == 0)
		{
			get_file(order + 4);
		}
		else if (strncmp(order, "put ", 4) == 0)
		{
			put_file(order + 4, size - 4);
		}
		else
		{
			list_files();
		}
		close(confd);
	}
	close(listenfd);

	return 0;
}
