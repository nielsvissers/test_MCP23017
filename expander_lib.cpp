#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#include "expander_lib.h"

/* defined in <linux/i2c-dev.h>
#define I2C_SLAVE 0x703 */


/* Init des Expanders; gibt den Expander zurück
 * address: I2C-Busadresse des Bausteines (i2cdetect -y 1)
 * directionA/B: Richtungen der Ports
 * I2CBus: Pfad zum I2CBus ("/dev/i2c-1" für Bus 1)
 */
int main()
{
  mcp23017 expander;  /* Verwaltungs-Structure */
  int data = 0x01;    /* Ausgabewert */
  int down = 0;       /* Richtungsangabe */
  expander = init_mcp23017(0x20,255,255);

  while(1)
    {
    /* beide Ports identisch "bedienen" */
    
    int fd;
    fd = open_mcp23017(expander);
    
	unsigned char buf[1] = {0};
    int returnvalue=read_register(expander,0x13, buf, 1);
    printf("return value: %.2x\n",returnvalue);
    usleep(100000); /* 100 ms Pause */
    }

  return 0;
}
int read_register(int busfd, __uint16_t reg, unsigned char *buf, int bufsize)
{
	unsigned char reg_buf[2];
	int ret;
  

	reg_buf[0] = (reg >> 0) & 0xFF;
	reg_buf[1] = (reg >> 8) & 0xFF;

	ret = write(busfd, reg_buf, 2);
	if (ret < 0) {
		printf("Failed to write [0x%02x 0x%02x] (reg: %04x).\n", reg_buf[0], reg_buf[1], reg);
		return ret;
	}

	printf("wrote %d bytes\n", ret);
	return read_bus(busfd, buf, bufsize);

}
mcp23017 init_mcp23017(int address, int directionA, int directionB/*, char* I2CBus*/)
  {
  int fd;             /* Filehandle */
  mcp23017 expander;  /* Rueckgabedaten */

  /* Structure mit Daten fuellen */
  expander.address = address;
  expander.directionA = directionA;
  expander.directionB = directionB;
 // expander.I2CBus = I2CBus;

  // Port-Richtung (Eingabe/Ausgabe) setzen
  fd = open_mcp23017(expander);
  printf("set iodira \n");
  setdir_mcp23017(expander, IODIRA, expander.directionA);
  printf("set iodirb \n");
  setdir_mcp23017(expander, IODIRB, expander.directionB);
  close(fd);
  return expander;
  }

/* Datenrichtung der Ports festlegen
 * richtungsregister: muss "IODIRA" oder "IODIRB" sein!
 * value: Zuordnung der Bits (Input: 1, Output: 0)
 * Bei den Eingangspins wird der Pullup-Widerstand eingeschaltet und die Logik umgekehrt
 */
void setdir_mcp23017(mcp23017 expander, int richtungsregister, int value)
  {
  if(richtungsregister == IODIRA)
    {
    /* Datenrichtung schreiben */
    
  printf("set bank A (richting) \n");
    write_mcp23017(expander, IODIRA, value);
    /* Pull-Up-Widerstaende einschalten Port A */
  printf("set bank A (pullup) \n");
    write_mcp23017(expander, GPPUA, value);
    /* Logik umkehren */
   // write_mcp23017(expander, IPOLA, value);
    }
  else if(richtungsregister == IODIRB)
    {
    /* Datenrichtung schreiben */
    write_mcp23017(expander, IODIRB, value);
    /* Pull-Up-Widerstaende einschalten Port B */
    write_mcp23017(expander, GPPUB, value);
    /*Logik umkehren */
   // write_mcp23017(expander, IPOLB, value);
    }
  else
    {
    printf("Richtungsregister falsch!\n");
    exit(1);
    }
  }

/* Oeffnet den Bus und gibt Filedescriptor zurueck
 * (write_mcp23017 und read_mcp23017 übernehmen das selbst)
 */
int open_mcp23017(mcp23017 expander)
  {
  int fd;
  if ((fd = open("/dev/i2c-1", O_RDWR)) < 0)
    {
    printf("Failed to open the i2c bus\n");
    exit(1);
    }

  /* Spezifizieren der Adresse des slave device */
  if (ioctl(fd, I2C_SLAVE, expander.address) < 0)
    {
    printf("Failed to acquire bus access and/or talk to slave\n");
    exit(1);
    }
  return fd;
  }

/* Schreibt in ein Register des Expanders
 * reg: Register in das geschrieben werden soll
 * value: Byte das geschrieben werden soll
 */
void write_mcp23017(mcp23017 expander, int reg, int value)
  {
  int fd;
  fd = open_mcp23017(expander);
  unsigned char buffer[2]={0};
  buffer[0]=reg;
  buffer[1]=value;
  if(write(fd,buffer, 1) < 0)
    {
    printf("Failed to write to the i2c bus\n");
    printf("%d / %d \n", reg, value);
    exit(1);
    }
  close(fd);
  }

/* Liest Register des Expanders
 * reg: Register, das ausgelesen wird;
 * gibt ausgelesenen Registerwert zurück
 */
int read_mcp23017(mcp23017 expander, int reg)
   {
   int value,fd;
   fd = open_mcp23017(expander);
   unsigned char buffer[2]={0};
   
  unsigned char writebuffer[1]={0};
  writebuffer[0]=0x13;
   write(fd, writebuffer, 1);
   if((value = read(fd, buffer ,1)) < 0)
     {
     printf("Failed to read from the i2c bus\n");
     close(fd);
     exit(1);
     return 0;
     }
   else
     {
     close(fd);
     printf("value %d\n",value);
     printf("buffer %s\n",buffer);
     return value;
     }
  }