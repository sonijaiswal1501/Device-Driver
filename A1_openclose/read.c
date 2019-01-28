#include <stdio.h>
#include <fcntl.h>

#define SIZE 50

int main()
{
	int fd, ret;

	fd = open("/dev/test0", O_RDWR);

	if(fd<0)
	{
	  perror("Error opening the devices\n");
	  return -1;
	}

	return 0;
}
