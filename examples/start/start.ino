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

	Serial.println(F("universal-ble-arduino (example/start)"));
}

void loop() {
	
	// Loop
	universal.loop();
}

void universalDidReceiveData(const unsigned char * data, unsigned char dataSize) {
	didReceivePingMessage();
	// int no_err = 0;
	// msgpack_unpacked msg;
	// msgpack_unpacked_init(&msg);
	// if(msgpack_unpack_next(&msg, (const char *)data, dataSize, NULL)){
	// 	msgpack_object obj = msg.data;
	// 	if(obj.type == MSGPACK_OBJECT_MAP)
	// 	{
	// 		msgpack_object_kv* const pend = obj.via.map.ptr + obj.via.map.size;
	//       for(msgpack_object_kv* p = obj.via.map.ptr; p < pend; p++)
	// 		{
	// 			const char * key = (p->key).via.str.ptr;
	// 			const char * value = (p->val).via.str.ptr;
	//
	// 			if (strcmp(key, "msg") == 0) {
	// 				if (strcmp(value, "ping") == 0) {
	// 					didReceivePingMessage();
	// 					break;
	// 				}
	// 			}
	//       }
	// 	}
	// }
}

void didReceivePingMessage() {
	sendPongMessage();
}

void sendPongMessage() {
  
	msgpack_sbuffer sbuf;
  msgpack_packer pk;

  msgpack_sbuffer_init(&sbuf);  
  msgpack_packer_init(&pk, &sbuf, msgpack_sbuffer_write);
   
  msgpack_pack_map(&pk, 1);
  msgpack_pack_str(&pk, 3);
  msgpack_pack_str_body(&pk, "msg", 3);
  msgpack_pack_str(&pk, 4);
  msgpack_pack_str_body(&pk, "pong", 4);
  
	universal.sendData((const unsigned char *)sbuf.data, sbuf.size);
  
	msgpack_sbuffer_destroy(&sbuf);  
}