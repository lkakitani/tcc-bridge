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
#include <sstream>
#include <string>
#include "../../RF24.h"
// #include "parser_mod.h"
#include <bitset>

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


const int min_payload_size = 4;
const int max_payload_size = 32;
const int payload_size_increments_by = 1;
uint32_t color = 0;
int next_payload_size = min_payload_size;
char receive_payload[max_payload_size+1]; // +1 to allow room for a terminating NULL char

int main(int argc, char** argv){

  bool role_ping_out = 1, role_pong_back = 0;
  bool role = 0;

  // Print preamble:
  printf("RF24/examples/pingpair_dyn/\n");

  // Setup and configure rf radio
  radio.begin();
  //radio.enableDynamicPayloads();
  radio.setRetries(15,15);
  radio.setPayloadSize(32);
  radio.printDetails();


/********* Role chooser ***********/

  printf("\n ************ Role Setup ***********\n");
  string input = "";
  char myChar = {0};
  cout << "Choose a role: Enter 0 for receiver, 1 for transmitter (CTRL+C to exit) \n>";
  getline(cin,input);

  if(input.length() == 1) {
	myChar = input[0];
	if(myChar == '0'){
		cout << "Role: Pong Back, awaiting transmission " << endl << endl;
	}else{  cout << "Role: Ping Out, starting transmission " << endl << endl;
		role = role_ping_out;
	}
  }
/***********************************/

    if ( role == role_ping_out )    {
      radio.openWritingPipe(pipes[0]);
      radio.openReadingPipe(1,pipes[1]);
    } else {
      radio.openWritingPipe(pipes[1]);
      radio.openReadingPipe(1,pipes[0]);
      radio.startListening();
    }


// forever loop
	while (1)
	{
  //
  // Ping out role.  Repeatedly send the current time
  //

  if (role == role_ping_out)
  {
    // First, stop listening so we can talk.
    radio.stopListening();

    // Take the time, and send it.  This will block until complete
    uint8_t mensagem[32];
    for(int i = 0; i < 32; i++) {
      mensagem[i] = 0;
    }
    mensagem[0] = 0x11;
    mensagem[14] = (uint8_t)(color & 0xFF);
    mensagem[15] = (uint8_t)((color >> 8) & 0xFF);
    mensagem[16] = (uint8_t)((color >> 16) & 0xFF);
    bool ok = radio.write( mensagem, 32 );
    
    //color++;
    color = (color << 1) + 1;
    if(color > 0xFFFFFF) {
      color = 0;
    }
    if (ok)
      printf("ok...");
    else
      printf("failed.\n\r");


    // Try again 1s later
    //while(1);
    delay(50);
  }

  //
  // Pong back role.  Receive each packet, dump it out, and send it back
  //

  if ( role == role_pong_back )
  {
    // if there is data ready
    if ( radio.available() )
    {
      // Dump the payloads until we've gotten everything
      unsigned long got_time;
      
        // Fetch the payload, and see if this was the last one.
        radio.read( &got_time, sizeof(unsigned long) );

        // Spew it
        printf("Got payload %lu...",got_time);

  // Delay just a little bit to let the other unit
  // make the transition to receiver
  //delay(20);
      

      // First, stop listening so we can talk
      radio.stopListening();

      // Send the final one back.
      radio.write( &got_time, sizeof(unsigned long) );
      printf("Sent response.\n\r");

      // Now, resume listening so we catch the next packets.
      radio.startListening();
    }
  }

}
}


