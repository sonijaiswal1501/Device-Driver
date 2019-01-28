#define MY_TYPE 'x'

#define CMD_WRITE_STRUCT _IOW(MY_TYPE, 0, char)
#define CMD_READ_STRUCT _IOR(MY_TYPE, 1, char)

