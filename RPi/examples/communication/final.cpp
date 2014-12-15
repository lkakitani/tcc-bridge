/*
TMRh20 2014 - Optimized RF24 Library Fork
*/

/**
* Example using Dynamic Payloads
*
* This is an example of how to use payloads of a varying (dynamic) size.
*/

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "./RF24.h"


using namespace std;

//
// Hardware configuration
//

// CE Pin, CSN Pin, SPI Speed

// Setup for GPIO 22 CE and CE1 CSN with SPI Speed @ 1Mhz
//RF24 radio(RPI_V2_GPIO_P1_22, RPI_V2_GPIO_P1_26, BCM2835_SPI_SPEED_1MHZ);

// Setup for GPIO 22 CE and CE0 CSN with SPI Speed @ 4Mhz
//RF24 radio(RPI_V2_GPIO_P1_15, BCM2835_SPI_CS0, BCM2835_SPI_SPEED_4MHZ);

// Setup for GPIO 22 CE and CE0 CSN with SPI Speed @ 8Mhz
RF24 radio(RPI_V2_GPIO_P1_15, RPI_V2_GPIO_P1_24, BCM2835_SPI_SPEED_8MHZ);


// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };



const int min_payload_size = 32;
const int max_payload_size = 32;
const int payload_size_increments_by = 1;
int next_payload_size = min_payload_size;

char receive_payload[max_payload_size+1]; // +1 to allow room for a terminating NULL char

int main(int argc, char** argv){

	string messageFromAPI;
	string flag = "0";
	int i; //cont

	// Setup and configure rf radio
	radio.begin();
	radio.enableDynamicPayloads();
	radio.setRetries(5,15);
	radio.printDetails();

	for (;;) {


		// initial role: pong_back
 		radio.openWritingPipe(pipes[1]);
 		radio.openReadingPipe(1,pipes[0]);
 		radio.startListening();
		
		//
		// Pong back role.  Receive each packet, dump it out, and send it back
		//
		// if there is data ready

		delay(100);

		if ( radio.available() )
		{
			// Dump the payloads until we've gotten everything
			uint8_t len;

			while (radio.available())
			{
				// Fetch the payload, and see if this was the last one.
				len = radio.getDynamicPayloadSize();
				radio.read( receive_payload, len );

				// Put a zero at the end for easy printing
				receive_payload[len] = 0;

				// Spew it
				printf("Got payload size=%i value=%s\n\r",len,receive_payload);
				
				
// 				ofstream responseFile ("/dev/shm/response");
// 				if (responseFile.is_open()) {
// 					responseFile << receive_payload;
// 					responseFile.close();
// 				}

				// Write response to file
				FILE * pFile;
				pFile = fopen ("/dev/shm/response","w");
				if (pFile != NULL) {
					for (i = 0; i < 32; i++) {
						fprintf(pFile, "%x", receive_payload[i] & 0xff);
					}
					fclose (pFile);
				} else {
					printf("Could not open response file.\n");
				}
				
			}

			// First, stop listening so we can talk
			radio.stopListening();

			// Send the final one back.
			radio.write( receive_payload, len );
			printf("Sent response.\n\r");

			// Now, resume listening so we catch the next packets.
			radio.startListening();
		}


		// read file 
		ifstream flagFile ("/dev/shm/flag.txt");
		if (flagFile.good()) {
			getline(flagFile, flag);
			flagFile.close();
			if (flag == "1") {
				ofstream flagFile ("/dev/shm/flag.txt");
				if (flagFile.is_open()) {
					flagFile << "0\n";
					flagFile.close();
				}
			}
		}

		if (flag == "1") {

			// read file 
			ifstream messageFile ("/dev/shm/message");
			if (messageFile.good()) {
				getline(messageFile, messageFromAPI);
				messageFile.close();
			}
			cout << "Message: " << messageFromAPI << endl; 
					
			radio.openWritingPipe(pipes[0]);
			radio.openReadingPipe(1,pipes[1]);
		
// 			string destAddr = messageFromAPI.substr(0,10);
// 			string payload = messageFromAPI.substr(10,64);

// 			cout << "Destination address: " << destAddr << ", Payload: " << payload << endl;

			// Set destination address and payload here	
			uint64_t address = 0x000000000000;
			char send_payload[33];
			send_payload[32] = 0;

			const char* src;
			for (i = 0; i < 74; i = i + 2) {
				string byte = messageFromAPI.substr(i, 2); 
				src = byte.c_str();

				char hexArray[1] = {0};
				int hexLength = 0;

				// read in the string
				unsigned int hex = 0;
				sscanf(src, "%x", &hex);

				// write it out
				for (unsigned int mask = 0xff00, bitPos=8; mask; mask>>=8, bitPos-=8) {
					unsigned int currByte = hex & mask;
					if (currByte || hexLength) {
						hexArray[hexLength++] = currByte>>bitPos;
					}   
				}   


				if (i < 10) {
					address = address | hexArray[0];
					if (i != 8)
						address = address << 8;
				}   
				else {
					int index = 0.5*i - 5;
					send_payload[index] = hexArray[0];
				}

			}

			radio.openWritingPipe(address);

			//static char send_payload[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ789012";

			// First, stop listening so we can talk.
			radio.stopListening();

			// Take the time, and send it.  This will block until complete
			printf("Now sending length %i...",next_payload_size);
			radio.write( send_payload, next_payload_size );

			// Now, continue listening
			radio.startListening();

			// Wait here until we get a response, or timeout
			unsigned long started_waiting_at = millis();
			bool timeout = false;
			while ( ! radio.available() && ! timeout )
				if (millis() - started_waiting_at > 500 )
					timeout = true;

			// Describe the results
			if ( timeout ) {
				printf("Failed, response timed out.\n\r");
			} else {
				// Grab the response, compare, and send to debugging spew
				uint8_t len = radio.getDynamicPayloadSize();
				radio.read( receive_payload, len );

				// Put a zero at the end for easy printing
				receive_payload[len] = 0;

				// Spew it
				printf("Got response size=%i value=%s\n\r",len,receive_payload);
			}

			// Update size for next time.
			next_payload_size += payload_size_increments_by;
			if ( next_payload_size > max_payload_size )
				next_payload_size = min_payload_size;

			// Try again 1s later
			delay(100);


			flag = "0";

			if (remove("/dev/shm/message") != 0) {
				printf("Error deleting /dev/shm/message\n");
			} else {
				printf("/dev/shm/message deleted.\n");
			}

		}
	}
}


