#ifndef __UNIVERSAL_H__
#define __UNIVERSAL_H__

#include <SPI.h>
#include <BLEPeripheral.h>

typedef void (*UniversalDidReceiveDataHandler)(const unsigned char * data, unsigned char dataSize);

class Universal
{
  public:
    Universal(unsigned char req, unsigned char rdy, unsigned char rst);
    virtual ~Universal();

    void setup(UniversalDidReceiveDataHandler didReceiveDataHandler);
    void loop();
		
		void sendData(const unsigned char * data, unsigned char dataSize);
		
		void peripheralDidConnect(BLECentral& central);
		void peripheralDidDisconnect(BLECentral& central);
		void characteristicDidUpdate(BLECharacteristic& characteristic);

  protected:
		BLEPeripheral _peripheral;
		BLEService _service;
		BLECharacteristic _rxCharacteristic;
		BLECharacteristic _txCharacteristic;
		UniversalDidReceiveDataHandler _didReceiveDataHandler;
		
	private:
		void setupEventHandlers();
};

#endif
