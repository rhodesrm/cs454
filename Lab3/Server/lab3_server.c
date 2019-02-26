/****************************************************/
/*                                                  */
/*   CS-454/654 Embedded Systems Development        */
/*   Instructor: Renato Mancuso <rmancuso@bu.edu>   */
/*   Boston University                              */
/*                                                  */
/*   Description: template file for serial          */
/*                communication server              */
/*                                                  */
/****************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdint.h>
#include <fcntl.h>
#include "pc_crc16.h"
#include "lab3.h"
#include <time.h>
//#include "lab3_troll.h"

#define GREETING_STR						\
	"CS454/654 - Lab 3 Server\n"				\
	"Author: Bassel University (BU)\n"				\
	"(Enter a message to send.  Type \"quit\" to exit)\n"

#define USAGE_STR							\
	"\nUSAGE: %s [-v] [-t percentage] <dev>\n"			\
	"   -v \t\t Verbose output\n"					\
	"   -t \t\t Invoke troll with specified bit flipping percentage\n" \
	"   <dev> \t Path to serial terminal device to use, e.g. /dev/ttyUSB0\n\n"

#define TROLL_PATH "./lab3_troll"

char read_byte(int ifd) {
	time_t seconds = time(NULL);
	char buffer[2];
	int i = 0;
	while(i < 2) {
		int num_read = read(ifd,&buffer+i,1);
		if(num_read >= 0) {
			i += num_read;
		}
		else {
			printf("%d",num_read);
		}
		if(time(NULL) - seconds > 3) {
			return 0;
		}
	}
	return buffer[0];
}

int main(int argc, char* argv[])
{
	double troll_pct=0.3;		// Perturbation % for the troll (if needed)
	int ifd,ofd,i,N=1,troll=0;	// Input and Output file descriptors (serial/troll)
	char str[MSG_BYTES_MSG],opt;	// String input
	struct termios oldtio, tio;	// Serial configuration parameters
	int VERBOSE = 0;		// Verbose output - can be overriden with -v
	int dev_name_len;
	char * dev_name = NULL;
	
	/* Parse command line options */
	while ((opt = getopt(argc, argv, "-t:v")) != -1) {
		switch (opt) {
		case 1:
			dev_name_len = strlen(optarg);
			dev_name = (char *)malloc(dev_name_len);
			strncpy(dev_name, optarg, dev_name_len);
			break;
		case 't':
			troll = 1; 
			troll_pct = atof(optarg);                    
			break;
		case 'v':
			VERBOSE = 1;
			break;
		default:
			break;
		}
	}

	/* Check if a device name has be00en passed */
	if (!dev_name) {
		fprintf(stderr, USAGE_STR, argv[0]);
		exit(EXIT_FAILURE);
	}
	
	// Open the serial port (/dev/ttyS1) read-write
	ifd = open(dev_name, O_RDWR | O_NOCTTY);
	if (ifd < 0) {
		perror(dev_name);
		exit(EXIT_FAILURE);
	}

	printf(GREETING_STR);

	// Start the troll if necessary
	if (troll)
	{
		// Open troll process (lab5_troll) for output only
		FILE * pfile;		// Process FILE for troll (used locally only)
		char cmd[128];		// Shell command

		snprintf(cmd, 128, TROLL_PATH " -p%f %s %s", troll_pct,
			 (VERBOSE) ? "-v" : "", dev_name);

		pfile = popen(cmd, "w");
		if (!pfile) { perror(TROLL_PATH); exit(-1); }
		ofd = fileno(pfile);
	}
	else ofd = ifd;		// Use the serial port for both input and output
	//
	// WRITE ME: Set up the serial port parameters and data format
	//
	tcgetattr(ofd,&oldtio);

	tio.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
	tio.c_iflag = 0;
	tio.c_oflag = 0;
	tio.c_lflag = 0;
	// tio.c_cflag &= ~CSTOPB;
	// tio.c_cflag &= ~CRTSCTS;

	tcflush(ofd, TCIFLUSH);
	tcsetattr(ofd, TCSANOW, &tio);

	tcgetattr(ifd,&oldtio);
	tcflush(ifd, TCIFLUSH);
	tcsetattr(ifd, TCSANOW, &tio);


	while(1)
	{

		//
		// WRITE ME: Read a line of input (Hint: use fgetc(stdin) to read each character)
		//
		// char *buffer;
		// size_t bufsize = MSG_BYTES_MSG;
 
		// 
		// getline(&buffer,&bufsize,stdin);
		i = 0;
		while(i < MSG_BYTES_MSG-1) {
			char c = fgetc(stdin);
			if(c != '\n')
				str[i] = c;
			else break;
			i++;
		}
		str[i] = '\0';
		size_t msg_size = i+1;
		if (strcmp(str, "quit") == 0) break;

		//
		// WRITE ME: Compute crc (only lowest 16 bits are returned)
		//
		int crc = pc_crc16(str,msg_size-1);
		printf("crc:%d\n",crc);
 		size_t data_size = MSG_BYTES_HEADER + msg_size * sizeof(char);
		
		char data[data_size];
		data[0] = MSG_START;
		data[1] = crc >> 8;
		data[2] = crc;
		data[3] = msg_size-1;
		for(i = 0; i <= msg_size; i++)
			data[4+i] = str[i];
		char ack = 0;
		int attempts = 0;
		while (!ack)
		{
			

			printf("Sending (attempt %d)...\n", ++attempts);

			write(ofd,&data,data_size);
			
			// 
			// WRITE ME: Send message
			//
			printf("Message sent, waiting for ack... ");

			ack = read_byte(ifd);
			//
			// WRITE ME: Wait for MSG_ACK or MSG_NACK
			//


			printf("%s\n", ack ? "ACK" : "NACK, resending");
		}
		printf("\n");
		//free(data);
	}


	
	// WRITE ME: Reset the serial port parameters
	
	
	// Close the serial port
	close(ifd);
	
	return EXIT_SUCCESS;
}

