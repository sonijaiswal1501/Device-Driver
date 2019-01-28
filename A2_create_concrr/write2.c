#include<stdio.h>
#include<fcntl.h>
#include<stdlib.h>

int main()
{
        int fd, ret;
        char buff[] = "Welcome\n";

        fd = open("/dev/test0", O_RDWR);

        if(fd < 0)
        {
          perror("Error opening the device\n");
          return -1;
        }

        ret = write(fd, buff, sizeof(buff));

        if(ret != sizeof(buff))
          printf("Error in writing message\n");
        else
          printf("Successfully written\n");

        return 0;
}

