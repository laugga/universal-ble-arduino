#include "Universal.h"

// Used for the event handlers
static Universal * _this;

static void peripheralConnectedHandler(BLECentral& central);
static void peripheralDisconnectedHandler(BLECentral& central);
static void characteristicWrittenHandler(BLECentral& central, BLECharacteristic& characteristic);

Universal::Universal(unsigned char req, unsigned char rdy, unsigned char rst) :
	_peripheral(req, rdy, rst),
	_service("8ebdb2f3-7817-45c9-95c5-c5e9031aaa47"),
	_txCharacteristic("08590F7E-DB05-467E-8757-72F6FAEB13D4", BLERead | BLEWrite | BLENotify, 20), // 20 characters max. for Blend micro nRF8001
	_rxCharacteristic("08590F7E-DB05-467E-8757-72F6FAEB13D5", BLERead | BLEWrite | BLENotify, 20)
{
}

Universal::~Universal() 
{
}

void Universal::setup(UniversalDidReceiveDataHandler didReceiveDataHandler) 
{
	Serial.println(F("Universal::setup"));

  this->_peripheral.setLocalName("universal-arduino"); // optional
  this->_peripheral.setAdvertisedServiceUuid(this->_service.uuid()); // optional

	// add attributes (services, characteristics, descriptors) to peripheral
  this->_peripheral.addAttribute(this->_service);
  this->_peripheral.addAttribute(this->_txCharacteristic);
 	this->_peripheral.addAttribute(this->_rxCharacteristic);
	
	this->_didReceiveDataHandler = didReceiveDataHandler;
	
  setupEventHandlers();
	
	this->_peripheral.begin();
}

void Universal::loop() 
{
	this->_peripheral.poll();
}

void Universal::sendData(const unsigned char * data, unsigned char dataSize)
{
	this->_txCharacteristic.setValue(data, dataSize);
}

void Universal::setupEventHandlers() 
{
	_this = this;
	
  this->_peripheral.setEventHandler(BLEConnected, peripheralConnectedHandler);
  this->_peripheral.setEventHandler(BLEDisconnected, peripheralDisconnectedHandler);
  this->_rxCharacteristic.setEventHandler(BLEWritten, characteristicWrittenHandler);
}

void Universal::peripheralDidConnect(BLECentral& central) 
{
  // central connected event handler
  Serial.print(F("Universal::peripheralDidConnect, central: "));
  Serial.println(central.address());
}

void Universal::peripheralDidDisconnect(BLECentral& central) 
{
  // central disconnected event handler
  Serial.print(F("Universal::peripheralDidDisconnect, central: "));
  Serial.println(central.address());
}

void Universal::characteristicDidUpdate(BLECharacteristic& characteristic) 
{
	// Only interested in the rxCharacteristic
	Serial.println(F("[rxCharacteristic] characteristicValueDidUpdate"));
	if(this->_didReceiveDataHandler) {
		this->_didReceiveDataHandler(characteristic.value(), characteristic.valueLength());
	}
}

static void peripheralConnectedHandler(BLECentral& central) 
{
	_this->peripheralDidConnect(central);
}

static void peripheralDisconnectedHandler(BLECentral& central) 
{
	_this->peripheralDidDisconnect(central);
}

static void characteristicWrittenHandler(BLECentral& central, BLECharacteristic& characteristic) 
{
	_this->characteristicDidUpdate(characteristic);
}

