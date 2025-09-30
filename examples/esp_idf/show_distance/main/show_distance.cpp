#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <bb_proximity.h>

const char *szProxType[] = {"None", "APDS9930", "APDS9960", "LTR553"};

BBProximity bbp;

extern "C" {
void app_main(void);
}

// I2C Bus on Xiao ESP32C3
#define SDA_PIN 6
#define SCL_PIN 7

void app_main(void)
{
int i;

        printf("bb_proximity show distance example\n");
        printf("Initializes, configures, then loops displaying the distance.\n");
        i = bbp.init(SDA_PIN, SCL_PIN); // find a supported proximity sensor
        if (i != BB_PROX_SUCCESS) {
            printf("NB: by default your system may require root access for I2C\n");
            printf("No supported device found\n");
            return; // problem - quit
        }
        printf("Proximity sensor detected and initialized\n");
        printf("device type = %s\n", szProxType[bbp.type()]);
        printf("The distance value is the number of detected photons\n");
        printf("This is a non-linear value which is inversely related to the distance.\n");
        bbp.start(BBP_CAPS_PROXIMITY); // start reading proximity values
        while (1) {
                int iPhotons = bbp.getProximity();
                printf("Photons: %d\n", iPhotons);
                vTaskDelay(25); // show 4 samples per second
        }

} /* app_main() */
