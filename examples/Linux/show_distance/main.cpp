//
// bb_proximity "show distance" example
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <bb_proximity.h>

const char *szProxType[] = {"None", "APDS9930", "APDS9960", "LTR553"};

BBProximity bbp;

int main(int argc, char *argv[])
{
int i;

        printf("bb_proximity show distance example\n");
        printf("Initializes, configures, then loops displaying the distance.\n");
	// I2C bus 1 is the default on RPI hardware
        // Other Linux systems can use any number from 0 to 10 (usually)
	// Bus 4 is the default I2C bus (pins 3+5) on the OrangePi RV2
        i = bbp.init(4); // find a supported proximity sensor 
        if (i != BB_PROX_SUCCESS) {
            printf("NB: by default your system may require root access for I2C\n");
	    printf("No supported device found\n");
            return -1; // problem - quit
        }
        printf("Proximity sensor detected and initialized\n");
        printf("device type = %s\n", szProxType[bbp.type()]);
	printf("The distance value is the number of detected photons\n");
	printf("This is a non-linear value which is inversely related to the distance.\n");
	bbp.start(BBP_CAPS_PROXIMITY); // start reading proximity values
	while (1) {
		int iPhotons = bbp.getProximity();
		printf("Photons: %d\n", iPhotons);
		usleep(250000); // show 4 samples per second
	}
return 0;
} /* main() */
