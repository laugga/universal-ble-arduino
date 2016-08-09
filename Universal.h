#ifndef __UNIVERSAL_H__
#define __UNIVERSAL_H__

#include <SPI.h>
#include <BLEPeripheral.h>

class Universal
{
  public:
    Universal(unsigned char req, unsigned char rdy, unsigned char rst);
    virtual ~Universal();

    void setup();
    void loop();
		
		void peripheralDidConnect(BLECentral& central);
		void peripheralDidDisconnect(BLECentral& central);
		void characteristicDidUpdate(BLECharacteristic& characteristic);

  protected:
		BLEPeripheral _peripheral;
		BLEService _service;
		BLECharacteristic _rxCharacteristic;
		BLECharacteristic _txCharacteristic;
		
	private:
		void setupEventHandlers();
};

#endif
