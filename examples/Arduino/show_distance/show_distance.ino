//
// Show the distance value (accumulated photons)
//
#include <bb_proximity.h>

BBProximity bbp;
const char *szProxType[] = {"None", "APDS9930", "APDS9960", "LTR553"};

void setup()
{
    Serial.begin(115200);
    delay(3000); // allow time for CDC-Serial to start
    Serial.println("bb_proximity show distance example");
    Serial.println("Initializes, configures, then loops displaying the distance.");
// I2C sensors each have a set of capabilities and options
// This example code initializes them with default options
// Check the library documentation to make use of all of their features
    pinMode(PIN_ENABLE_I2C_PULLUP, OUTPUT); // Nano 33 BLE has a power
    pinMode(PIN_ENABLE_SENSORS_3V3, OUTPUT); // enable for the built-in
    digitalWrite(PIN_ENABLE_I2C_PULLUP, 1); // sensors
    digitalWrite(PIN_ENABLE_SENSORS_3V3, 1);
    if (bbp.init(I2C_SDA1, I2C_SCL1) == BB_PROX_SUCCESS) { // initialize sensor with default options
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
    while (1) {
      int iPhotons = bbp.getProximity();
      Serial.print("Photons: ");
      Serial.println(iPhotons, DEC);
      delay(250);
    }
}

void loop()
{
}
