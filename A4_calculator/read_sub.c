#include <stdio.h>
#include <fcntl.h>

#define SIZE 50

int main()
{
	int fd, ret;

	fd = open("/dev/sub1", O_RDWR);

	if(fd<0)
	{
	  perror("Error opening the devices\n");
	  return -1;
	}

	ret = read(fd, buff, SIZE);
	printf("Read is: %s\n", buff);

	return 0;
}
