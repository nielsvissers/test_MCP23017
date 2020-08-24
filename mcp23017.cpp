#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <poll.h>
#include <chrono>
char buff[10];
char  buff1[10];
char arr[10];
int wbyte,wbyte1,io,rd;
void write_reg(int fh, uint8_t reg, uint8_t val);
int read_reg(int fh, uint8_t reg, int count);
pollfd init_interuptPin(int gpio);
int main()
{
int fd=open("/dev/i2c-1",O_RDWR);
if(fd<0){
perror("Cannot open fd\n");
exit(1);
}
else
{
printf("I2c working\n");
}
buff[0]=0x00;
buff[1]=0x00;
buff1[0]=0x12;
buff1[1]=0xFF;
//arr[0]=0x12;
//arr[1]=0x00;
io=ioctl(fd,I2C_SLAVE,0x20);
if(io<0)
{
printf("ERROR IOCTL \n");
}
/*
write_reg(fd,0x00,0x00); //bank a output
write_reg(fd,0x12,0x00); //alles uit
usleep(1000000);
write_reg(fd,0x12,0xFF); //alles aan
usleep(1000*1000);*/

struct pollfd pfd;
pfd=init_interuptPin(4);
write_reg(fd,0x00,0xFF); //bank a input
write_reg(fd,0x0C,0xFF); //bank a pullup alles
write_reg(fd,0x01,0xFF); //bank a input
write_reg(fd,0x0D,0xFF); //bank a pullup alles
    write_reg(fd, 0x04, 0xFF);
    write_reg(fd, 0x05, 0xFF);
    write_reg(fd, 0x0A, 0x40);
    write_reg(fd, 0x0B, 0x40);
    read_reg(fd,0x12,1);
    read_reg(fd,0x13,1);
    
   char bufinterupt[8];
while(1)
{
       lseek(pfd.fd, 0, SEEK_SET);    /* consume any prior interrupt */
   read(pfd.fd, bufinterupt, sizeof bufinterupt);
    poll(&pfd, 1, -1);         /* wait for interrupt */
       lseek(pfd.fd, 0, SEEK_SET);    /* consume interrupt */
   read(pfd.fd, bufinterupt, sizeof bufinterupt);
    rd=read_reg(fd,0x13,1);
    printf("After : GPIOB 0x%02X\n",rd);
    int hexMask=0x01;
    int hexAND01=hexMask & rd;
    printf("GPIOB changed: %d\n",hexAND01);rd=read_reg(fd,0x12,1);
    printf("After : GPIOA 0x%02X\n",rd);
    hexAND01=hexMask & rd;
    printf("GPIOA changed: %d\n",hexAND01);
    
    write_reg(fd, 0x04, 0xFF);
    write_reg(fd, 0x05, 0xFF);
    
}
printf("Everything goes right\n");

return 1;
}

void write_reg(int fd, uint8_t reg, uint8_t val)
{
	uint8_t data[2];

	data[0] = reg;
	data[1] = val;

	if (write(fd, &data, 2) != 2) { 		 
		perror("write");
	
	}
}

pollfd init_interuptPin(int gpio)
{
       char str[256];
   struct pollfd pfd;
   int fd;
   char buf[8];
    sprintf(str, "/sys/class/gpio/gpio%d/value", gpio);

   if ((fd = open(str, O_RDONLY)) < 0)
   {
      fprintf(stderr, "Failed, gpio %d not exported.\n", gpio);
      exit(1);
   }

   pfd.fd = fd;

   pfd.events = POLLPRI;
   lseek(fd, 0, SEEK_SET);    /* consume any prior interrupt */
   read(fd, buf, sizeof buf);

return pfd;

}

int read_reg(int fh, uint8_t reg, int count)
{
	uint8_t data[2];
	data[0] = reg;

	if (write(fh, &data, 1) != 1) { 		 
		perror("write before read");
		return -1;
	}

	data[1] = 0;

	if (read(fh, &data, count) != count) {
		perror("read");
		return -1;	
	}
	return (data[1] << 8) + data[0];
}
