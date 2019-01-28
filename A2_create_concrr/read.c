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

        ret = read(fd, buff, sizeof(buff));

        if(ret >= 0)
          printf("Read is: %s\n", buff);
        else
          printf("Successfully written\n");

        return 0;
}








