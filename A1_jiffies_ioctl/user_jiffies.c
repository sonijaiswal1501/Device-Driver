#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "my_ioctl.h"

#define SIZE 50

int main()
{
        int fd, ret, cnt, delay;
        char buff[SIZE];

        fd = open("/dev/test0", O_RDWR);

        if(fd<0)
        {
          perror("Error opening the devices\n");
          return -1;
        }
	printf("\nEnter the delay\n");
	scanf("%d", &delay);
	ioctl(fd, CMD_DELAY , delay);

	read(fd,buff, sizeof(buff));
	printf("\nReceived data : %s\n",buff);

	close(fd);

        return 0;
}
