#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#define SIZE 50

int main()
{
	int fd, ret;
	char buff[SIZE];
	fd = open("/dev/add0", O_RDWR);

	if(fd<0)
	{
	  perror("Error opening the devices\n");
	  return -1;
	}

	ret = read(fd, buff, sizeof(buff));
	printf("Read is: %s\n", buff);

	return 0;
}
