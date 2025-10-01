//
// bb_proximity "LED boost" example
// This example shows how to control the IR LED boost current to control
// the reach (maximum distance) that the proximity sensor can reach
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <dirent.h>
#include <bb_proximity.h>

const char *szProxType[] = {"None", "APDS9930", "APDS9960", "LTR553"};
const char *szBoost[] = {"min", "low", "med", "high", "max"};

BBProximity bbp;

int main(int argc, char *argv[])
{
int i, iBus;
DIR *pDir;
struct dirent *pDE;
uint32_t u32Buses = 0; // available I2C bus numbers (0-31)

        printf("bb_proximity LED boost example\n");
        printf("Finds which I2C bus has the supported sensor, then\ninitializes, configures, and loops displaying the distance.\n");
	// I2C buses in Linux are defined as a file in the /dev directory
        pDir = opendir("/dev");
	if (!pDir) {
		printf("Error searching /dev directory; aborting.\n");
		return -1;
	}
	// Search all names in the /dev directory for those starting with i2c-
        while ((pDE = readdir(pDir)) != NULL) {
		if (memcmp(pDE->d_name, "i2c-", 4) == 0) { // found one!
                    iBus = atoi(&pDE->d_name[4]);
		    u32Buses |= (1 << iBus); // collect the bus numbers
		}
	}
	closedir(pDir);
	if (u32Buses == 0) { // Something went wrong; no I2C buses
	    printf("No I2C buses found!\n");
	    printf("Check your system configuration (e.g. raspi-config)\n");
	    printf("to ensure that I2C is enabled.\n");
	    return -1;
	}
	// Search each I2C bus for a supported proximited sensor
        for (iBus=0; iBus<32; iBus++) {
	    if (u32Buses & (1<<iBus)) { // a bus that we found in /dev
                i = bbp.init(iBus); // scan for supported sensors
                if (i == BB_PROX_SUCCESS) {
                    printf("Found a device on i2c-%d!\n", iBus);
		    break;
		}
	    }
        } // for each possible bus
	if (iBus == 32) { // scanned all buses and didn't find anything
            printf("No supported device found\n");
	    printf("Your system may require sudo to access I2C.\n");
            return -1; // problem - quit
	}
        printf("Proximity sensor detected and initialized\n");
        printf("device type = %s\n", szProxType[bbp.type()]);
	printf("The distance value is the number of detected photons\n");
	printf("This is a non-linear value which is inversely related to the distance.\n");
	bbp.start(BBP_CAPS_PROXIMITY); // start reading proximity values
    for (int iBoost = BBP_BOOST_MIN; iBoost <= BBP_BOOST_MAX; iBoost++) {
      bbp.setLEDBoost(iBoost);
      for (int i=0; i<20; i++) { // take 20 readings at each boost level
        int iPhotons = bbp.getProximity();
        printf("Boost: %s, photos: %d\n", szBoost[iBoost], iPhotons);
        usleep(250000);
      } // for i
    } // for each boost level
return 0;
} /* main() */
