#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "my_ioctl.h"

#define SIZE 50

int main()
{
        int fd, ret, cnt;
	char buff[SIZE];
        fd = open("/dev/test0", O_RDWR);

        if(fd<0)
        {
          perror("Error opening the devices\n");
          return -1;
        }

	ioctl(fd, CMD_TEST1 );
	ioctl(fd, CMD_TEST2 , &ret);
	printf("Read is: %d\n", ret);

	write(fd, "Hello", sizeof("Hello"));
	ioctl(fd, BUF_CNT , &cnt);
	printf("Buffer count is: %d\n", cnt);

	ioctl(fd, BUF_CLR);
	ioctl(fd, BUF_DISP);


        return 0;
}

