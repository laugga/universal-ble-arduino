// Sensor
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2591.h>

Adafruit_TSL2591 tsl = Adafruit_TSL2591(2591); // pass in a number for the sensor identifier (for your use later)

void sensorSetup(void)
{
  tsl.setGain(TSL2591_GAIN_MED);      // 25x gain
  tsl.setTiming(TSL2591_INTEGRATIONTIME_100MS);  // shortest integration time (bright light)
}

float sensorValue()
{
  sensors_event_t event;
  tsl.getEvent(&event);
  
  if ((event.light == 0) | (event.light > 4294966000.0) | (event.light <-4294966000.0)) {
    /* If event.light = 0 lux the sensor is probably saturated */
    /* and no reliable data could be generated! */
    /* if event.light is +/- 4294967040 there was a float over/underflow */
    return -1;
  }
  
  return event.light;
} 

// Communication
#include <Universal.h>
#include <BLEPeripheral.h>
#include <msgpack-embedded.h>

// BLE pins (varies per shield/board)
#define BLE_REQ_PIN 6
#define BLE_RDY_PIN 7
#define BLE_RST_PIN 4

// Create a universal instance
Universal universal = Universal(BLE_REQ_PIN, BLE_RDY_PIN, BLE_RST_PIN);

void setup() {
  
  Serial.begin(115200);
  
#if defined (__AVR_ATmega32U4__)
  delay(5000);  // 5 seconds delay for enabling to see the start up comments on the serial board
#endif
  
  // Initialize 
  universal.setup(universalDidReceiveData);
  tsl.begin();
  
  sensorSetup();
  
  Serial.println(F("universal-ble-arduino (example/lux)"));
}

int loopCount = 0;

void loop() {
  
  // Loop
  universal.loop();
  
  if (loopCount % 1000 == 0) {
    sendLuxMessage(sensorValue());
  }
  
  ++loopCount;
}

void universalDidReceiveData(const unsigned char * data, unsigned char dataSize) {
  // Ignore...
}

void sendLuxMessage(float luxValue) {
  
  msgpack_sbuffer sbuf;
  msgpack_packer pk;

  msgpack_sbuffer_init(&sbuf);  
  msgpack_packer_init(&pk, &sbuf, msgpack_sbuffer_write);
   
  msgpack_pack_map(&pk, 1);
  msgpack_pack_str(&pk, 3);
  msgpack_pack_str_body(&pk, "lux", 3);
  msgpack_pack_float(&pk, luxValue);
  
  universal.sendData((const unsigned char *)sbuf.data, sbuf.size);
  
  msgpack_sbuffer_destroy(&sbuf);  
}