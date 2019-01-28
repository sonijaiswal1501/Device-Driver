#include <stdio.h>
#include <fcntl.h>

#define SIZE 50

int main()
{
        int fd, ret;
	char buff[SIZE];
        fd = open("/dev/test0", O_RDWR);

        if(fd<0)
        {
          perror("Error opening the devices\n");
          return -1;
        }

	ret = read(fd, buff, sizeof(buff));

        if(ret >= 0)
	   printf("Read is: %s\n", buff);
	else
	  perror("Error in Reading\n");

        return 0;
}

