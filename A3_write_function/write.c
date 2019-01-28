#include<stdio.h>
#include<fcntl.h>
#include<stdlib.h>
#include<string.h>

#define SIZE 20

int main()
{
	int i, fd, length;
	char buff[SIZE];

	fd = open("/dev/test0", O_RDWR);

	printf("\nEnter the message: ");
	gets(buff);

	printf("\nEntered message is: %s\n", buff);
	length = strlen(buff);

	printf("\nMessage length is: %d\n", length);

	if(fd < 0)
	{
	  perror("Error opening the devices\n");
	  return -1;
	}

	write(fd, buff, length);

	return 0;
}
