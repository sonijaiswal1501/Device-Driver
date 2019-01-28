#define MAGIC 'x'


#define CMD_TEST1 _IO(MAGIC, 1)
#define CMD_TEST2 _IOR(MAGIC, 1, int*)
#define	BUF_CNT _IOR(MAGIC, 3, int*)
#define BUF_CLR	_IO(MAGIC, 4)
#define BUF_DISP _IO(MAGIC, 5)

