//
// bb_proximity
// LED boost adjustment example
// This example shows how different IR LED boost settings
// affect the range and values of the proximity sensor.
// Try holding your hand near the sensor and watch as the
// boost value steps through the range. The photo value
// will increase with each step as the IR LED intensity
// is increased. This will increase the maximum detectable
// distance as well.
//
#include <bb_proximity.h>
BBProximity bbp;
const char *szProxType[] = {"None", "APDS9930", "APDS9960", "LTR553"};
const char *szBoost[] = {"min", "low", "med", "high", "max"};
// These are the I2C GPIO numbers for the nRF52840 "BBBadge" board
// Set them to the appropriate value for your specific device
#define SDA_PIN 43
#define SCL_PIN 42

void setup()
{
    Serial.begin(115200);
    delay(3000); // allow time for CDC-Serial to start
    Serial.println("bb_proximity show distance example");
    Serial.println("Initializes, configures, then loops displaying the distance.");
    if (bbp.init(SDA_PIN, SCL_PIN, true) == BB_PROX_SUCCESS) { // initialize sensor with default options
        bbp.start(BBP_CAPS_PROXIMITY);
    } else {
        Serial.println("Error initializing sensor!");
        while(1) {};
    }
    Serial.println("Proximity sensor detected and initialized");
    Serial.print("device type = ");
    Serial.println(szProxType[bbp.type()]);
    Serial.println("The distance value is the number of detected photons");
    Serial.println("This is a non-linear value which is inversely related to the distance.");
    for (int iBoost = BBP_BOOST_MIN; iBoost <= BBP_BOOST_MAX; iBoost++) {
      bbp.setLEDBoost(iBoost);
      for (int i=0; i<20; i++) { // take 20 readings at each boost level
        int iPhotons = bbp.getProximity();
        Serial.print("Boost: ");
        Serial.print(szBoost[iBoost]);
        Serial.print(", photons: ");
        Serial.println(iPhotons, DEC);
        delay(250);
      }
    } // for each boost level
}

void loop()
{
}

