#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <bb_proximity.h>

const char *szProxType[] = {"None", "APDS9930", "APDS9960", "LTR553"};
const char *szBoost[] = {"min", "low", "med", "high", "max"};

BBProximity bbp;

extern "C" {
void app_main(void);
}

// I2C Bus on Xiao ESP32C3
#define SDA_PIN 6
#define SCL_PIN 7

void app_main(void)
{
    printf("bb_proximity show distance example\n");
    printf("Initializes, configures, then loops displaying the distance.\n");
    if (bbp.init(SDA_PIN, SCL_PIN) == BB_PROX_SUCCESS) { // initialize sensor with default options
        bbp.start(BBP_CAPS_PROXIMITY);
    } else {
        printf("Error initializing sensor!\n");
        while(1) {vTaskDelay(1); };
    }
    printf("Proximity sensor detected and initialized\n");
    printf("device type = %s\n", szProxType[bbp.type()]);
    printf("The distance value is the number of detected photons.\n");
    printf("This is a non-linear value which is inversely related to the distance.\n");
    for (int iBoost = BBP_BOOST_MIN; iBoost <= BBP_BOOST_MAX; iBoost++) {
      bbp.setLEDBoost(iBoost);
      for (int i=0; i<20; i++) { // take 20 readings at each boost level
        int iPhotons = bbp.getProximity();
        printf("Boost: %s, photos: %d\n", szBoost[iBoost], iPhotons);
        vTaskDelay(25);
      } // for i
    } // for each boost level
} /* app_main() */
