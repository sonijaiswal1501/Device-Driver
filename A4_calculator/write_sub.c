#include<stdio.h>
#include<fcntl.h>
#include<stdlib.h>

#define SIZE 20

int main()
{
	int fd, ret;
	char buff[SIZE] = "52\n";

	fd = open("/dev/sub2", O_RDWR);

	if(fd < 0)
	{
	  perror("Error opening the devices\n");
	  return -1;
	}

	ret = write(fd, buff, sizeof(buff));

	return 0;
}
