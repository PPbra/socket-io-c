// Họ và tên:Ngô Minh Phương
// Mã sinh viên: 16021629
// Chương trình Client thiết lập kết nối với Server và yêu cầu file để download cho server => download file ,  tiếp tục yêu cầu download cho đên khi nhập vào "quit"

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
	printf("Nhap vao kich thuoc buffer:");
	scanf("%d", &bufferLength);
	getchar();
	nbytes = write(sockfd, (void*)&bufferLength, sizeof(bufferLength));
	if (nbytes < 0)
	{
		perror("Write error!");
		return 1;
	}
	
	char fileName[bufferLength];
	while (1)
	{
		printf("Nhap vao ten file can download cho server:\n");
		fgets(fileName, sizeof(fileName), stdin);
		fileName[strlen(fileName) - 1] = '\0';
		if (!strcmp("quit", fileName))
		{
			printf("Dong ket noi!\n");
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
			bzero(getBuffer, bufferLength);

			int sizePlus = fileSize % bufferLength;
			//Trường hợp file có kich thước nhỏ hơn buffer
			if (fileSize < bufferLength)
			{
				while (getSize < fileSize)
				{
					int current = read(sockfd, getBuffer, bufferLength);
					fwrite(getBuffer, 1, fileSize, file);
					printf("%d\n", current);
					getSize += current;
					bzero(getBuffer, bufferLength);
				}
			}
			else
			{
				//Trường hợp file có kich thước lớn hơn buffer

				while (1)
				{
					if (getSize < fileSize - bufferLength)
					{
						int current = read(sockfd, getBuffer, bufferLength);
						fwrite(getBuffer, 1, current, file);
						getSize += current;
						continue;
					}
					read(sockfd, getBuffer, bufferLength);
					fwrite(getBuffer, 1, sizePlus, file);
					bzero(getBuffer, bufferLength);
					break;
				}
			}
			printf("Download file %s xong!\n", fileName);
			fclose(file);
		}
	}
	close(sockfd);
	printf("Dong ket noi!\n");
	return 1;
}