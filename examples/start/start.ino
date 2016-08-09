#include <Universal.h>
#include <BLEPeripheral.h>

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
	universal.setup();

	Serial.println(F("universal-ble-arduino (example/start)"));
}

void loop() {
	
	// Loop
	universal.loop();
}