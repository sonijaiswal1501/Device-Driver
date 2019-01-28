#include<stdio.h>
#include<fcntl.h>

#define SIZE 20


int main()
{
        int fd, ret;
        char buff[SIZE];

        fd = open("/dev/test0", O_RDWR);

        if(fd<0)
        {
          perror("Error opening the device\n");
          return -1;
        }

        ret = read(fd, buff, SIZE);
        printf("Read is: %s\n", buff);
        return 0;
}


