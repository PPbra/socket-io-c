// Họ và tên:Ngô Minh Phương
// Mã sinh viên: 16021629
// Chương trình Server thiế t lập và lắng nghe kết nối từ Client , nhận tên file từ Client , tìm file và gử i cho Client => hiển thị số file cliets đã download ,tiếp tục lắng nghe kết nối tiếp
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/select.h>
#include <sys/time.h>

#define QLEN 5

extern int errno;

//

int handleARequest(int connectSocket);
// int *connClientSocket;
int countFile = 0;
// pthread_mutex_t access_point = PTHREAD_MUTEX_INITIALIZER;

int main()
{
	struct sockaddr_in clientAddress, serverAddress;
	int addressLenght, listenPort;
	int masterSocket;
	int listenQueue = 3;
	fd_set readFileDesc, activeFileDesc;
	int connectSocket;

	masterSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (masterSocket < 0)
	{
		perror("Server bi loi !");
		return 1;
	}

	printf("Nhap vao cong server: ");
	scanf("%d", &listenPort);
	getchar();

	while (1)
	{

		bzero(&serverAddress, sizeof(serverAddress));
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
		serverAddress.sin_port = htons(listenPort);

		int bind_c = bind(masterSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
		if (bind_c < 0)
		{
			printf(" Bind loi ,nhap lai cong server: ");
			scanf("%d", &listenPort);
			getchar();
			continue;
		}
		break;
	}

	if (listen(masterSocket, listenQueue) < 0)
	{
		perror("Listen err!");
	}

	printf("Server on at port %d\n", listenPort);

	FD_ZERO(&activeFileDesc);
	FD_SET(masterSocket, &activeFileDesc);

	while (1)
	{
		memcpy(&readFileDesc, &activeFileDesc, sizeof(readFileDesc));
		if (select(FD_SETSIZE, &readFileDesc, (fd_set *)0, (fd_set *)0, (struct timeval *)0) < 0)
		{
			perror("Select err!");
		}

		if (FD_ISSET(masterSocket, &readFileDesc))
		{
			int subSocket;

			addressLenght = sizeof(clientAddress);

			subSocket = accept(masterSocket, (struct sockaddr *)&clientAddress, &addressLenght);

			if (subSocket < 0)
			{
				perror("accept error!");
			}

			FD_SET(subSocket, &activeFileDesc);
		}

		for (connectSocket = 0; connectSocket < FD_SETSIZE; ++connectSocket)
		{
			if (connectSocket != masterSocket && FD_ISSET(connectSocket, &readFileDesc))
			{
				if (handleARequest(connectSocket) == 0)
				{
					(void)close(connectSocket);
					FD_CLR(connectSocket, &activeFileDesc);
				}
			}
		}
	}
	return 0;
}

int handleARequest(int connectSocket)
{

	int nbytes;
	int bufferLength = 2048;

	char buffer[bufferLength];

	nbytes = read(connectSocket, buffer, sizeof(buffer));
	if (nbytes < 0)
	{
		perror("Read err!\n");
	}

	if (nbytes == 0)
	{
		return 0;
	}
	int fileSize = 0;
	FILE *file, fileStat;
	file = fopen(buffer, "rb");

	if (file == 0)
	{
		int message = -1;
		write(connectSocket, (void *)&message, sizeof(int));
		return 1;
	}
	else
	{
		int message = 1;
		write(connectSocket, (void *)&message, sizeof(int));
	}

	printf("File name client requests:%s \n ", buffer);
	//lay kich thuoc file
	fseek(file, 0L, SEEK_END);
	fileSize = ftell(file);
	fseek(file, 0L, SEEK_SET);
	printf("Size of file: %d \n", fileSize);
	//gui kich thuoc file
	write(connectSocket, (void *)&fileSize, sizeof(int));
	//gui file
	bzero(buffer, sizeof(buffer));
	while (!feof(file))
	{
		int sizeRead = fread(buffer, 1, sizeof(buffer), file);
		write(connectSocket, buffer, sizeof(buffer));
		bzero(buffer, sizeof(buffer));
	}
	printf("Sent file done , amount of files have downloaded:%d \n", ++countFile);
	return 1;
}
