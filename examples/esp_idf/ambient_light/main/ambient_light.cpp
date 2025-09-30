#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <bb_proximity.h>

const char *szProxType[] = {"None", "APDS9930", "APDS9960", "LTR553"};
const char *szCaps[] = {"ALS", "PROXIMITY", "GESTURE", "COLORS"};

BBProximity bbp;

extern "C" {
void app_main(void);
}

// I2C Bus on Xiao ESP32C3
#define SDA_PIN 6
#define SCL_PIN 7

void app_main(void)
{
  printf("bb_proximity ambient light example\n");
  printf("Initializes, configures, then loops displaying the ambient light measurement.\n");
  if (bbp.init(SDA_PIN, SCL_PIN) == BB_PROX_SUCCESS) {
    // Show type and capabilities
    printf("Proximity sensor detected and initialized\n");
    printf("device type = %s\n", szProxType[bbp.type()]);
    uint32_t u32Caps = bbp.caps();
    printf("Capabilities: ");
    for (int i=0; i<4; i++) { // there are only 4 supported capabilities
      if (u32Caps & (1 << i)) {
        if (i != 0) printf(", ");
        printf(szCaps[i]);
      }
    }
    printf("\n");
    bbp.start(BBP_CAPS_ALS);
  } else {
    printf("Error initializing sensor!\n");
    while (1) {};
  }
  while (1) {
    int iALS = bbp.getLight();
    printf("ALS: %d\n", iALS);
    vTaskDelay(25);
  }
} /* app_main() */
