// Họ và tên:Ngô Minh Phương
// Mã sinh viên: 16021629
// Chương trình Client thiết lập kết nối với Server và gửi thông báo lên Server

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
	int bufferLength = 1024;
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
	char fileName[bufferLength];
	printf("Nhap vao ten file can download cho server:\n");
	fgets(fileName, sizeof(fileName), stdin);
	fileName[strlen(fileName) - 1] = '\0';
	if (!strcmp("quit", fileName))
	{
		close(sockfd);
		return 1;
	}
	else
	{
		nbytes = write(sockfd, fileName, sizeof(fileName));
		if (nbytes < 0)
		{
			perror("Write error!");
			return 1;
		}

		int fileSize;
		nbytes = read(sockfd, &fileSize, sizeof(fileSize));
		if (nbytes < 0)
		{
			perror("Read error!");
			return 1;
		}

		printf("Dang download file ----> Kich thuoc file:%d bytes.......\n", fileSize);

		FILE *file = fopen(fileName, "wb");

		int getSize = 0;
		char getBuffer[bufferLength];
		while (getSize < fileSize)
		{
			int current = read(sockfd, getBuffer, bufferLength);
			getSize += current;
			fwrite(getBuffer, 1, current, file);
		}

		printf("Download file %s xong!\n", fileName);
		fclose(file);
	}

	// while (1)
	// {
	// 	printf("Nhap vao thong bao cho server:\n");
	// 	fgets(send_msg, sizeof(send_msg), stdin);
	// 	send_msg[strlen(send_msg) - 1] = '\0';
	// 	if (!strcmp("quit", send_msg))
	// 	{
	// 		close(sockfd);
	// 		return 1;
	// 	}
	// 	else
	// 	{
	// 		nbytes = write(sockfd, send_msg, sizeof(send_msg));
	// 		if (nbytes < 0)
	// 		{
	// 			perror("Write error!");
	// 			return 1;
	// 		}
	// 		char buffer[1000];
	// 		nbytes = read(sockfd, buffer, sizeof(buffer));
	// 		if (nbytes < 0)
	// 		{
	// 			perror("Read error!");
	// 			return 1;
	// 		}
	// 		printf("Server tra loi :%s\n", buffer);
	// 	}
	// }
	close(sockfd);
	printf("Dong ket noi!\n");
	return 1;
}