#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <strings.h>
#include <stdlib.h>
#include <stdio.h>

int main()
{
	int sockfd;
	int servport;
	struct sockaddr_in servadddr;
	char serverIpAddr[1000];
	// char* serverIpAddr = (char *) malloc(100 * sizeof(char *));
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		perror("Socket Error!");
	}
	printf("Nhap vao dia chi cua server: ");
	scanf("%s", serverIpAddr);
	getchar();
	printf("Nhap vao cong ket noi server: ");
	scanf("%d", &servport);
	getchar();
	bzero(&servadddr, sizeof(servadddr));
	servadddr.sin_family = AF_INET;
	servadddr.sin_addr.s_addr = inet_addr(serverIpAddr);
	servadddr.sin_port = htons(servport);

	int connect_c = connect(sockfd, (struct sockaddr *)&servadddr, sizeof(servadddr));
	if (connect_c < 0)
	{
		perror("Connect error!");
		return 1;
	}
	int nbytes;
	//tao msg
	char send_msg[1000];
	while (1)
	{
		printf("Nhap vao thong bao cho server:\n");
		fgets(send_msg, sizeof(send_msg), stdin);
		send_msg[strlen(send_msg) - 1] = '\0';
		if (!strcmp("quit", send_msg))
		{
			break;
		}
		else
		{
			nbytes = write(sockfd, send_msg, sizeof(send_msg));
			if (nbytes < 0)
			{
				perror("Write error!");
				return 1;
			}
			char buffer[1000];
			nbytes = read(sockfd, buffer, sizeof(buffer));
			if (nbytes < 0)
			{
				perror("Read error!");
				return 1;
			}
			printf("Server tra loi :%s\n", buffer);
		}
	}
	close(sockfd);
	printf("Dong ket noi!\n");
	return 0;
}