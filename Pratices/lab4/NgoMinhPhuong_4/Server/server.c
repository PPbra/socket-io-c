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

void *handleARequest(void *arg);
int *connClientSocket;
int countFile = 0;
pthread_mutex_t access_point = PTHREAD_MUTEX_INITIALIZER;

int main()
{
	int listenfd, connfd, listenFort;
	int listenQueue = 3;
	struct sockaddr_in cliaddr, servaddr, connClientAddr;
	pid_t childId;
	socklen_t clilen;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd < 0)
	{
		perror("Server bi loi !");
		return 1;
	}

	printf("Nhap vao cong server: ");
	scanf("%d", &listenFort);
	getchar();

	while (1)
	{

		bzero(&servaddr, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
		servaddr.sin_port = htons(listenFort);

		int bind_c = bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
		if (bind_c < 0)
		{
			printf(" Bind loi ,nhap lai cong server: ");
			scanf("%d", &listenFort);
			getchar();
			continue;
		}
		break;
	}

	if (listen(listenfd, listenQueue) < 0)
	{
		perror("Listen loi!");
	}

	printf("Server dang nghe o cong %d\n", listenFort);

	while (1)
	{
		unsigned int addrLength = sizeof(cliaddr);
		connClientSocket = malloc(sizeof(int));
		*connClientSocket = accept(listenfd, (struct sockaddr *)&cliaddr, &addrLength);

		if (connClientSocket < 0)
		{
			perror("Accept error");
			return 1;
		}

		printf("Thong tin Client: %s:%d\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));

		pthread_t tid;
		pthread_create(&tid, NULL, &handleARequest, (void *)connClientSocket);
	}

	close(listenfd);
	printf("Dong ket noi!");

	return 0;
}

void *handleARequest(void *arg)
{
	int connClientSocket = *((int *)arg);
	int nbytes;
	int bufferLength = 1024;
	free(arg);
	pthread_detach(pthread_self());
	nbytes = read(connClientSocket, &bufferLength, sizeof(bufferLength));

	if (nbytes < 0)
	{
		perror("Doc loi! \n");
		return NULL;
	};

	char buffer[bufferLength];
	while (1)
	{
		nbytes = read(connClientSocket, buffer, sizeof(buffer));
		if (nbytes < 0)
		{
			perror("Đọc bị lỗi!\n");
			break;
		}
		else if (nbytes == 0)
		{
			break;
		}
		else
		{

			int fileSize = 0;
			FILE *file, fileStat;
			file = fopen(buffer, "rb");
			printf("Ten file Client gui len la:%s\n", buffer);
			//lay kich thuoc file
			fseek(file, 0L, SEEK_END);
			fileSize = ftell(file);
			fseek(file, 0L, SEEK_SET);
			printf("Kich thuoc cua file: %d \n", fileSize);
			//gui kich thuoc file
			write(connClientSocket, (void *)&fileSize, sizeof(int));
			//gui file
			bzero(buffer, sizeof(buffer));
			while (!feof(file))
			{
				int sizeRead = fread(buffer, 1, sizeof(buffer), file);
				write(connClientSocket, buffer, sizeof(buffer));
				bzero(buffer, sizeof(buffer));
			}
			printf("Gui file %s xong!\n", buffer);
			pthread_mutex_lock(&access_point);
			countFile++;
			printf("Tổng file Clients đã download: %d\n", countFile);
			pthread_mutex_unlock(&access_point);
		}
	}
	return NULL;
}