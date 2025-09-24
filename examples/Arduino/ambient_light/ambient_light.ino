//
// Ambient light example
//
#include <bb_proximity.h>
BBProximity bbp;
const char *szProxType[] = {"None", "APDS9930", "APDS9960", "LTR553"};
const char *szCaps[] = {"ALS", "PROXIMITY", "GESTURE", "COLORS"};

void setup()
{
  Serial.begin(115200);
  delay(3000);
  Serial.println("bb_proximity ambient light example");
  Serial.println("Initializes, configures, then loops displaying the ambient light measurement.");
  if (bbp.init() == BB_PROX_SUCCESS) {
    // Show type and capabilities
    Serial.println("Proximity sensor detected and initialized");
    Serial.print("device type = ");
    Serial.println(szProxType[bbp.type()]);
    uint32_t u32Caps = bbp.caps();
    Serial.print("Capabilities: ");
    for (int i=0; i<4; i++) { // there are only 4 supported capabilities
      if (u32Caps & (1 << i)) {
        if (i != 0) Serial.print(", ");
        Serial.print(szCaps[i]);
      }
    }
    Serial.println(" ");
    bbp.start(BBP_CAPS_ALS);
  } else {
    Serial.println("Error initializing sensor!");
    while (1) {};
  }
  while (1) {
    int iALS = bbp.getLight();
    Serial.print("ALS: ");
    Serial.println(iALS, DEC);
    delay(250);
  }
} /* setup() */

void loop()
{

} /* loop() */