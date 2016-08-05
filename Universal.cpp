#include "Universal.h"

#define DEFAULT_DEVICE_NAME "Arduino"
#define DEFAULT_APPEARANCE  0x0000

/* Define how assert should function in the BLE library */
void __ble_assert(const char *file, uint16_t line)
{
  // Serial.print("ERROR ");
  // Serial.print(file);
  // Serial.print(": ");
  // Serial.print(line);
  // Serial.print("\n");
  // while(1);
}

Universal::Universal(unsigned char req, unsigned char rdy, unsigned char rst) 
{
}

Universal::~Universal() 
{
}

void Universal::setup() 
{
	static struct aci_state_t aci_state;
    lib_aci_init(&aci_state, false);
}

void Universal::loop() 
{
}
