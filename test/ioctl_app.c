#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include "my_ioctl.h"

struct ioctl_struct
{
	int id;
	char name[20];
	unsigned long data;
};

struct ioctl_struct my_struct;

int main(int argc,char *argv[])
{
	int fd;
	char buff[15];
	unsigned int ch;

	fd = open("/dev/test3",O_RDWR,S_IRUSR|S_IWUSR);
	if(-1 == fd)
	{
		perror("Open");
		exit(0);
	}
	while(1)
	{
		printf("Enter command:\n1.CMD_WRITE_STRUCT\n2.CMD_READ_STRUCT\n3.EXIT\n");
		scanf("%d",&ch);
		switch(ch)
		{
			case 1:
				printf("Enter id:\n");
				scanf("%d",&my_struct.id);
				printf("Enter name:\n");
				scanf("%s",my_struct.name);
				printf("Enter data:\n");
				scanf("%ld",&my_struct.data);

				ioctl(fd,CMD_WRITE_STRUCT,(char*)&my_struct);

				break;
			case 2:
				printf("\n");
				struct ioctl_struct *my_struct1 = (struct ioctl_struct*)malloc(sizeof(struct ioctl_struct));
				ioctl(fd,CMD_READ_STRUCT,buff);
				my_struct1 = (struct ioctl_struct*)buff;
				printf("Received structure members are :\nid = %d\nname = %s\ndata = %ld\n",my_struct1->id,my_struct1->name,my_struct1->data);
				break;
			case 3:
				exit(0);
		       default: printf("Invalid choice!\n");
				break;
		}

	}

	return 0;

}


