#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <getopt.h>
#include <cstdlib>
#include <iostream>
#include "/home/pi/RF24/RPi/RF24/RF24.h"

using namespace std;
#define DEBUG 1
//RF24 radio("/dev/spidev0.0",8000000 , 25);  //spi device, speed and CSN,only CSN is NEEDED in RPI
RF24 radio(RPI_V2_GPIO_P1_22, RPI_V2_GPIO_P1_24, BCM2835_SPI_SPEED_1MHZ);
const int role_pin = 7;
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };



void setup(void){
	//Prepare the radio module
	printf("\nPreparing interface\n");
	radio.begin();
	//radio.setRetries( 15, 15);
	radio.setChannel(0x4c);
	//radio.setPALevel(RF24_PA_MAX);
	radio.setPALevel(RF24_PA_MAX);

	//radio.setPALevel(RF24_PA_MIN);
        radio.setDataRate(RF24_250KBPS);
        radio.setAutoAck(false);
	radio.setRetries( 1, 1);

	radio.openWritingPipe(pipes[0]);
	radio.openReadingPipe(1,pipes[1]);

	radio.startListening();
	radio.printDetails();
        printf("\nFinished interface\nRadio Data Available:%s", radio.available()?"Yes":"No");

}

bool switchLight(int action){
	//This function send a message, the 'action', to the arduino and wait for answer
	//Returns true if ACK package is received
	//Stop listening
	radio.stopListening();
	unsigned long message = action;
	printf("Now sending %lu...", message);
	
	//Send the message
	bool ok = radio.write( &message, sizeof(unsigned long) );
	if (ok)
		printf("ok...");
	else
		printf("failed.\n\r");
	//Listen for ACK
	radio.startListening();
	//Let's take the time while we listen
	unsigned long started_waiting_at = millis();
	bool timeout = false;
	while ( ! radio.available() && ! timeout ) {
		delayMicroseconds(10);
		if (millis() - started_waiting_at > 1000 )
			timeout = true;

	}

	if( timeout ){
		//If we waited too long the transmission failed
			printf("Failed, response timed out.\n\r");
			return false;
	}else{
		//If we received the message in time, let's read it and print it
		unsigned long got_time;
		radio.read( &got_time, sizeof(unsigned long) );
		printf("Got response %lu, round-trip delay: %lu\n\r",got_time,millis()-got_time);
		return true;
	}
	 
}  

int main( int argc, char ** argv){

        char choice;
	setup();
	bool switched = false;
	int counter = 0;

//Define the options

        radio.startListening();
        //Let's take the time while we listen
        unsigned long started_waiting_at = millis();
        bool timeout = false;
//        while ( ! radio.available() && ! timeout ) {
        while(1){
            delayMicroseconds(10000);
                //if (millis() - started_waiting_at > 1000 )
                        //timeout = true;
            if(radio.available())
            {
                //printf("Radio Data Available!!!\n");
                char abc[100];
                radio.read( abc, sizeof(abc) );
                printf("Got response %s, round-trip delay: \n\r", abc);

                //unsigned long got_time;
                //radio.read( &got_time, sizeof(unsigned long) );
                //printf("Got response %lu, round-trip delay: \n\r",got_time);

                delayMicroseconds(20*10);
                radio.stopListening();
/*                
                //Send the message
                bool ok = radio.write( &(got_time), sizeof(unsigned long) );
                if (ok)
                    printf("ok...");
                else
                    printf("failed.\n\r");
                    //Listen for ACK
*/
                radio.startListening();
                delayMicroseconds(20*10);
            }
        }

/*
		while(( choice = getopt( argc, argv, "m:")) != -1){

				if (choice == 'm'){

					 
					printf("\nOpening the gates...\n");
					while(switched == false && counter < 5){

						switched = switchLight(atoi(optarg));
						counter ++; 
						
					}


				}else{
					// A little help:
							printf("\n\rIt's time to make some choices...\n");
							printf("\n\rUse -f option: ");
							printf("\n[on|On|ON] - will turn the light on.");
							printf("\n[Off|*] -  guess what? It will turns the light off.\n ");
							printf("\n\rExample: ");
							printf("\nsudo ./switch -f on\n");
				}
	
			//return 0 if everything went good, 2 otherwise
			 if (counter < 5)
				 return 0;
			 else
				 return 2;
	 }
*/
}

