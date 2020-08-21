#include <unistd.h>				//Needed for I2C port
#include <fcntl.h>				//Needed for I2C port
#include <sys/ioctl.h>			//Needed for I2C port
#include <linux/i2c-dev.h>		//Needed for I2C port
#include <stdio.h>
#include <string>

#define MCP23017_ADDRESS 0x20

// registers
#define MCP23017_IODIRA 0x00
#define MCP23017_IPOLA 0x02
#define MCP23017_GPINTENA 0x04
#define MCP23017_DEFVALA 0x06
#define MCP23017_INTCONA 0x08
#define MCP23017_IOCONA 0x0A
#define MCP23017_GPPUA 0x0C
#define MCP23017_INTFA 0x0E
#define MCP23017_INTCAPA 0x10
#define MCP23017_GPIOA 0x12
#define MCP23017_OLATA 0x14


#define MCP23017_IODIRB 0x01
#define MCP23017_IPOLB 0x03
#define MCP23017_GPINTENB 0x05
#define MCP23017_DEFVALB 0x07
#define MCP23017_INTCONB 0x09
#define MCP23017_IOCONB 0x0B
#define MCP23017_GPPUB 0x0D
#define MCP23017_INTFB 0x0F
#define MCP23017_INTCAPB 0x11
#define MCP23017_GPIOB 0x13
#define MCP23017_OLATB 0x15

#define MCP23017_INT_ERR 255

//constants set up to emulate Arduino pin parameters
#define HIGH 1
#define LOW 0

#define INPUT 1
#define OUTPUT 0

#define CHANGE 0
#define FALLING 1
#define RISING 2

void writeRegister(int ,int , int );
void init(int );

int main()
{
	int file_i2c;
	int length;
	unsigned char buffer[60] = {0};

	
	//----- OPEN THE I2C BUS -----
	char *filename = (char*)"/dev/i2c-1";
	if ((file_i2c = open(filename, O_RDWR)) < 0)
	{
		//ERROR HANDLING: you can check errno to see what went wrong
		printf("Failed to open the i2c bus");
		return -1;
	}
	
	int addr = MCP23017_ADDRESS;          //<<<<<The I2C address of the slave
	if (ioctl(file_i2c, I2C_SLAVE, addr) < 0)
	{
		printf("Failed to acquire bus access and/or talk to slave.\n");
		//ERROR HANDLING; you can check errno to see what went wrong
		return -1;
	}

    init(file_i2c);
	printf("file_i2c: %n",file_i2c);
	while(true)
    {
	//----- READ BYTES -----
	length = 4;			//<<< Number of bytes to read
	if (read(file_i2c, buffer, length) != length)		//read() returns the number of bytes actually read, if it doesn't match then an error occurred (e.g. no response from the device)
	{
		//ERROR HANDLING: i2c transaction failed
		printf("Failed to read from the i2c bus.\n");
	}
	else
	{
        int i;
for (i=0;i<4;i++)
{
    printf("%.2x",buffer[i]);
}
printf("\n");
	}
}
}
void writeRegister(int file_i2c,int regAddress, int value)
{
	//----- WRITE BYTES -----
	unsigned char buffer[2] = {0};
	buffer[0] = regAddress;
	buffer[1] = value;
	int length = 4;			//<<< Number of bytes to write
	if (write(file_i2c, buffer, length) != length)		//write() returns the number of bytes actually written, if it doesn't match then an error occurred (e.g. no response from the device)
	{
		/* ERROR HANDLING: i2c transaction failed */
		printf("Failed to write to the i2c bus.\n");
	}
}
void init(int file_i2c)
{
    //set bank A as input
    writeRegister(file_i2c, MCP23017_IODIRA, 0xff);
    //set bank B as input
    writeRegister(file_i2c, MCP23017_IODIRB, 0xff);
    //set pullup bank A
    writeRegister(file_i2c, MCP23017_GPPUA, 0xff);
    //set pullup bank B
    writeRegister(file_i2c, MCP23017_GPPUB, 0xff);
}