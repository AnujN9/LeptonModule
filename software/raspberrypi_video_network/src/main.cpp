#include <iostream>
#include <ctime>
#include <stdint.h>
#include <chrono>
#include <unistd.h>
#include <cstdint>
#include <cstring>
#include <arpa/inet.h>

#include "SPI.h"
#include "Lepton_I2C.h"

/// \file main.cpp
/// \brief Main loop that sends raw data through ethernet using UDP protocol.

#define PACKET_SIZE 164
#define PACKET_SIZE_UINT16 (PACKET_SIZE/2)
#define PACKETS_PER_FRAME 60
#define FRAME_SIZE_UINT16 (PACKET_SIZE_UINT16*PACKETS_PER_FRAME)
#define FPS 27;

/// \brief Function to describe how to use the command line arguments
/// \param cmd Argument of the command line, here it is the program
void printUsage(char *cmd)
{
	char *cmdname = basename(cmd);
	printf("Usage: %s [OPTION]...\n"
		   " -h			display this help and exit\n"
		   " -net x		set the ip address (default: 10.42.0.1)\n"
		   " -port x	set the ip port (default: 8080)\n"
		   "", cmdname);
	return;
}

/// \brief Main function that captures raw data from the Lepton and transmits it using UDP.
/// \param argc Number of command-line arguments.
/// \param argv Array of command-line arguments.
/// \return 0 if successful, -1 if failure.
int main(int argc, char **argv)
{
	int typeLepton = 3; // Lepton 3.x
	unsigned int spiSpeed = 20 * 1000 * 1000; // SPI bus speed 20MHz
	int myImageWidth = 160;
	int myImageHeight = 120;

	uint8_t result[PACKET_SIZE*PACKETS_PER_FRAME];
	uint8_t shelf[4][PACKET_SIZE*PACKETS_PER_FRAME];
	uint16_t *frameBuffer;

	uint16_t n_wrong_segment = 0;
	uint16_t n_zero_value_drop_frame = 0;

	int sockfd;
	struct sockaddr_in servaddr;
	const char *netIP = "10.42.0.1";
	const char *port = "8080";

	for(int i=1; i < argc; i++)
	{
		if (strcmp(argv[i], "-h") == 0)
		{
			printUsage(argv[0]);
			exit(0);
		}
		else if (strcmp(argv[i], "-net") == 0)
		{
			if (i + 1 != argc)
			{
				netIP = argv[++i];
			} else {
				std::cerr << "Error: Enter an IP." << std::endl;
				exit(1);
			}
		}
		else if (strcmp(argv[i], "-port") == 0)
		{
			if (i + 1 != argc)
			{
				port = argv[++i];
			} else {
				std::cerr << "Error: Enter an IP." << std::endl;
				exit(1);
			}
		}
	}
	std::cout << "Network address is " << netIP << std::endl;
	std::cout << "Network port is " << port << std::endl;
	SpiOpenPort(0, spiSpeed);

	// Setting up UDP socket
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		std::cerr << "Socket creation failed" << std::endl;
		return -1;
	}

	memset(&servaddr, 0, sizeof(servaddr));

	// Set up server address
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = inet_addr(netIP);

	while(true)
	{
		//read data packets from lepton over SPI
		int resets = 0;
		int segmentNumber = -1;
		for(int j=0;j<PACKETS_PER_FRAME;j++)
		{
			//if it's a drop packet, reset j to 0, set to -1 so he'll be at 0 again loop
			read(spi_cs0_fd, result+sizeof(uint8_t)*PACKET_SIZE*j, sizeof(uint8_t)*PACKET_SIZE);
			int packetNumber = result[j*PACKET_SIZE+1];
			if(packetNumber != j)
			{
				j = -1;
				resets += 1;
				usleep(1000);
				//Note: we've selected 750 resets as an arbitrary limit, since there should never be 750 "null" packets between two valid transmissions at the current poll rate
				//By polling faster, developers may easily exceed this count, and the down period between frames may then be flagged as a loss of sync
				if(resets == 750)
				{
					SpiClosePort(0);
					lepton_reboot();
					n_wrong_segment = 0;
					n_zero_value_drop_frame = 0;
					usleep(750000);
					SpiOpenPort(0, spiSpeed);
				}
				continue;
			}
			if (packetNumber == 20)
			{
				segmentNumber = (result[j*PACKET_SIZE] >> 4) & 0x0f;
				if ((segmentNumber < 1) || (4 < segmentNumber))
				{
					break;
				}
			}
		}

		int iSegmentStart = 1;
		int iSegmentStop;
		if ((segmentNumber < 1) || (4 < segmentNumber))
		{
			n_wrong_segment++;
			continue;
		}
		if (n_wrong_segment != 0)
		{
			n_wrong_segment = 0;
		}
		memcpy(shelf[segmentNumber - 1], result, sizeof(uint8_t) * PACKET_SIZE*PACKETS_PER_FRAME);
		if (segmentNumber != 4)
		{
			continue;
		}
		iSegmentStop = 4;

		for (int i = 0; i < 4; ++i)
		{
			ssize_t sent_bytes = sendto(sockfd, shelf[i], sizeof(shelf[i]), 0, (const struct sockaddr *)&servaddr, sizeof(servaddr));
			if (sent_bytes < 0)
			{
				std::cerr << "Send failed" << std::endl;
				close(sockfd);
				return -1;
			}
		}
		usleep(10000);
	}
	// Close the socket
	close(sockfd);
	//finally, close SPI port just bcuz
	SpiClosePort(0);
}