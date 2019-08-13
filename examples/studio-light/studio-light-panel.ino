
// Adafruit Feather M0 BLE
// 24kHz frequency PWM with 10-bit resolution on D10 and D11
// Gamma-corrected for human vision

#include <string.h>
#include <Arduino.h>
#include <SPI.h>
#include <ArduinoJson.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"

#include "BluefruitConfig.h"

// HV (declaration)

#define HV_HUE_MAX 255
#define HV_VALUE_MAX 255

typedef struct {
  int h; // [0,255]
  int v; // [0,255]
} HV_t;

HV_t _hv = {0, 0};

void setupHV();
void updateHV(HV_t hv);
void testHV();

// BLE (declaration)

// Create the bluefruit object, either software serial
// hardware SPI, using SCK/MOSI/MISO hardware SPI pins and then user selected CS/IRQ/RST
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

#define MSG_OBJECT_BUFFER_SIZE 128

void bleWaitConnection();
void bleWaitMessage();
void bleReceiveMessage(JsonDocument& object);
void bleSendMessage(JsonDocument& object);

// PWM (declaration)

void setupPWM();
void updatePWM(int a, int b);

// DEBUG (declaration)

void setupSerial();
void serialPrintf(const char * format, ...);

// SETUP and LOOP

void setup()
{
  // Debug
  setupSerial();
  
  // Setup PWM
  setupPWM();

  // Setup State
  setupHV();
    
  // Setup BLE
  bleWaitConnection();
}

void loop()
{
//  StaticJsonDocument<128> object;
//  object["h"] = _hv.h; // hue 
//  object["v"] = _hv.v; // value
//  
//  updateHV({_hv.h+1, _hv.v+1});
//  
//  bleSendMessage(object);
//  delay(1000);

  bleWaitMessage();
}

// STATE (implementation) 

const int GAMMA_MAX = 256;
const int GAMMA[] = {
    0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  2,  2,  2,  3,  3,  4,
    4,  5,  5,  6,  6,  7,  8,  8,  9, 10, 11, 11, 12, 13, 14, 15,
   16, 17, 18, 19, 20, 21, 23, 24, 25, 26, 28, 29, 30, 32, 33, 35,
   36, 38, 39, 41, 42, 44, 46, 47, 49, 51, 53, 54, 56, 58, 60, 62,
   64, 66, 68, 70, 72, 74, 77, 79, 81, 83, 86, 88, 90, 93, 95, 98,
  100,103,105,108,110,113,116,118,121,124,127,129,132,135,138,141,
  144,147,150,153,156,159,163,166,169,172,176,179,182,186,189,193,
  196,200,203,207,210,214,218,221,225,229,233,236,240,244,248,252,
  256,260,264,268,272,276,281,285,289,293,298,302,306,311,315,320,
  324,329,333,338,342,347,352,356,361,366,371,375,380,385,390,395,
  400,405,410,415,420,425,431,436,441,446,452,457,462,468,473,479,
  484,490,495,501,506,512,518,523,529,535,541,546,552,558,564,570,
  576,582,588,594,600,606,613,619,625,631,638,644,650,657,663,670,
  676,683,689,696,702,709,716,722,729,736,743,749,756,763,770,777,
  784,791,798,805,812,819,827,834,841,848,856,863,870,878,885,893,
  900,908,915,923,930,938,946,953,961,969,977,984,992,1000,1008,1016,
  1024 };
  
void setupHV() {
  updateHV({0,0});
}

void updateHV(HV_t hv) {

  serialPrintf("Update HV (h = %d, v = %d)", hv.h, hv.v);

  int hue = max(0, min(hv.h, HV_HUE_MAX));
  int value = max(0, min(hv.v, HV_VALUE_MAX));
  
  // [0, 65025] -> [0, GAMMA_MAX]
  int a = map((HV_HUE_MAX - hue) * value, 0, 65025, 0, GAMMA_MAX);
  int b = map(hue * value, 0, 65025, 0, GAMMA_MAX);

  serialPrintf("(a = %d, b = %d)", a, b);

  int gamma_a = GAMMA[a];
  int gamma_b = GAMMA[b];

  serialPrintf("GAMMA (a = %d, b = %d)", gamma_a, gamma_b);

  updatePWM(gamma_a, gamma_b);
  
  _hv = {hue, value};
}

void testHV()
{
  for (int hue = 0; hue <= 255; hue += 2) {
    for (int value = 0; value <= 255; value += 1) {
      updateHV({hue, value});
      delay(1);
    }
    for (int value = 255; value >= 0; value -= 1) {
      updateHV({hue, value});
      delay(1);
    }
  }
}

// BLE (implementation)

void bleWaitConnection()
{
  ble.begin(VERBOSE_MODE);
  ble.factoryReset();
  ble.echo(false);
  ble.verbose(false); 
  
  while (!ble.isConnected()) {
    delay(500);
  }
  
  ble.setMode(BLUEFRUIT_MODE_DATA);
}

void bleWaitMessage()
{
  if(ble.isConnected())
  {
    char buffer[MSG_OBJECT_BUFFER_SIZE];
    size_t bufferSize = 0;
    
    while(ble.available()) {
      *(buffer+bufferSize) = ble.read();
      bufferSize += 1;
    }

    if(bufferSize > 0) {
      StaticJsonDocument<MSG_OBJECT_BUFFER_SIZE> object;
      DeserializationError error = deserializeMsgPack(object, buffer, bufferSize);
      if(!error) {
        
        serialPrintf("Receive message: %d bytes", bufferSize);
        bleReceiveMessage(object);
      }
    }
  }
}

void bleReceiveMessage(JsonDocument& object) 
{
  int hue = object["h"];
  int value = object["v"];

  updateHV({hue, value});
}

void bleSendMessage(JsonDocument& object) 
{
  if(ble.isConnected())
  {
    char buffer[MSG_OBJECT_BUFFER_SIZE];
    size_t bufferSize = serializeMsgPack(object, buffer, MSG_OBJECT_BUFFER_SIZE);
    ble.write(buffer, bufferSize);
    
    serialPrintf("Send message: %d bytes", bufferSize);
  }
}

// PWM  (implementation)

/*
HIGH FREQ PWM SETUP FOR MKR1000 - SAMW25 = SAMD21 with some other pinning
 
MKR1000's pins than can be defined as TCC timer pins and their associated channel (WO[X]) 
unless stated otherwise the TCC timers are on peripheral F:

A0 - PA02 - None
A1 - PB02 - None
A2 - PB03 - None
A3 - PA04 - TCC0/WO[0] (same channel as TCC0/WO[4])
A4 - PA05 - TCC0/WO[1] (same channel as TCC0/WO[5])
A5 - PA06 - TCC1/WO[0]
A6 - PA07 - TCC1/WO[1]
D0 - PA22 - TCC0/WO[4] (same channel as TCC0/WO[0])
D1 - PA23 - TCC0/WO[5] (same channel as TCC0/WO[1])
D2 - PA10 - TCC1/WO[0]
*D3 - PA11 - TCC1/WO[1]
D4 - PB10 - TCC0/WO[4] (same channel as TCC0/WO[0])
D5 - PB11 - TCC0/WO[5] (same channel as TCC0/WO[1])
D6 - PA20 - TCC0/WO[6] (same channel as TCC0/WO[2])
D7 - PA21 - TCC0/WO[7] (same channel as TCC0/WO[3])
D8 - PA16 - TCC0/WO[6] (same channel as TCC0/WO[2]) on peripheral F, TCC2/WO[0] on peripheral E
D9 - PA17 - TCC0/WO[7] (same channel as TCC0/WO[3]) on peripheral F, TCC2/WO[1] on peripheral E
D10 - PA19 - TCCO/WO[3] (same channel as TCC0/WO[7])
*D11 - PA08 - TCC1/WO[2] (same channel as TCC1/WO[0]) on peripheral F, TCC0/WO[0] on peripheral E
D12 - PA09 - TCC1/WO[3] (same channel as TCC1/WO[1]) on peripheral F, TCC0/WO[1] on peripheral E
D13 - PB22 - None
D14 - PB23 - None

Note the timer TCC0 has only 4 channels (0-3 and 4-7 are the same), 
while TCC1 and TCC2 each have 2, giving you 8 channels in total.
*/

// Output PWM 24Khz on digital pin D10 and D11 (10-bit resolution)
void setupPWM()
{
  REG_GCLK_GENDIV = GCLK_GENDIV_DIV(1) |          // Divide the 48MHz clock source by divisor 1: 48MHz/1=48MHz
                    GCLK_GENDIV_ID(4);            // Select Generic Clock (GCLK) 4
  while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization

  REG_GCLK_GENCTRL = GCLK_GENCTRL_IDC |           // Set the duty cycle to 50/50 HIGH/LOW
                     GCLK_GENCTRL_GENEN |         // Enable GCLK4
                     GCLK_GENCTRL_SRC_DFLL48M |   // Set the 48MHz clock source
                     GCLK_GENCTRL_ID(4);          // Select GCLK4
  while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization

  // Enable the port multiplexer for the digital pin D11(PA20), D10(PA18)
  PORT->Group[g_APinDescription[11].ulPort].PINCFG[g_APinDescription[11].ulPin].bit.PMUXEN = 1;
  PORT->Group[g_APinDescription[10].ulPort].PINCFG[g_APinDescription[10].ulPin].bit.PMUXEN = 1;
 
  // Connect the TCC0 timer to digital output D13 = PA17 = ODD - port pins are paired odd PMUO and even PMUXE
  // F & E specify the timers: TCC0, TCC1 and TCC2
  PORT->Group[g_APinDescription[11].ulPort].PMUX[g_APinDescription[11].ulPin >> 1].reg = PORT_PMUX_PMUXO_E | PORT_PMUX_PMUXE_E;
  PORT->Group[g_APinDescription[10].ulPort].PMUX[g_APinDescription[10].ulPin >> 1].reg = PORT_PMUX_PMUXO_F | PORT_PMUX_PMUXE_F;

  // Feed GCLK4 to TCC2 (and TC3)
  REG_GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN |         // Enable GCLK4 to TCC2 (and TC3)
                     GCLK_CLKCTRL_GEN_GCLK4 |     // Select GCLK4
                     GCLK_CLKCTRL_ID_TCC2_TC3;    // Feed GCLK4 to TCC2 (and TC3)
  while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization

  // Feed GCLK4 to TCC0 and TCC1
  REG_GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN |         // Enable GCLK4 to TCC0 and TCC1
                     GCLK_CLKCTRL_GEN_GCLK4 |     // Select GCLK4
                     GCLK_CLKCTRL_ID_TCC0_TCC1;   // Feed GCLK4 to TCC0 and TCC1
  while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization

  // Dual slope PWM operation: timers countinuously count up to PER register value then down 0
  REG_TCC2_WAVE |= TCC_WAVE_POL(0xF) |      // Reverse the output polarity on all TCC2 outputs
                   TCC_WAVE_WAVEGEN_DSBOTH; // Setup dual slope PWM on TCC2
  while (TCC2->SYNCBUSY.bit.WAVE);          // Wait for synchronization

  // Dual slope PWM operation: timers countinuously count up to PER register value then down 0
  REG_TCC0_WAVE |= TCC_WAVE_POL(0xF) |      // Reverse the output polarity on all TCC0 outputs
                   TCC_WAVE_WAVEGEN_DSBOTH; // Setup dual slope PWM on TCC0
  while (TCC0->SYNCBUSY.bit.WAVE);          // Wait for synchronization

  // Each timer counts up to a maximum or TOP value set by the PER register,
  // this determines the frequency of the PWM operation: Freq = 48Mhz/(2*N*PER)
  REG_TCC2_PER = 1000;            // Set the FreqTcc of the PWM on TCC1 to 24Khz
  while (TCC2->SYNCBUSY.bit.PER); // Wait for synchronization

  // Each timer counts up to a maximum or TOP value set by the PER register,
  // this determines the frequency of the PWM operation: Freq = 48Mhz/(2*N*PER)
  REG_TCC0_PER = 1000;            // Set the FreqTcc of the PWM on TCC1 to 24Khz
  while (TCC0->SYNCBUSY.bit.PER); // Wait for synchronization

  // Set the PWM signal to output 1500us and 1500us
  // PWM ds = 2*N(TOP-CCx)/Freqtcc => PWM=0 => CCx=PER, PWM=50% => CCx = PER/2
  REG_TCC2_CC0 = 0;               // TCC2 CC0 - on D11
  REG_TCC2_CC1 = 0;               // TCC2 CC1 - on D13
  while (TCC2->SYNCBUSY.bit.CC1); // Wait for synchronization

  // Set the PWM signal to output 1500us and 1500us
  REG_TCC0_CC2 = 0;               // TCC0 CC0 - on D10
  REG_TCC0_CC3 = 0;               // TCC0 CC1 - on D12
  while (TCC0->SYNCBUSY.bit.CC3); // Wait for synchronization

  // Divide the GCLOCK signal by 1 giving  in this case 48MHz (20.83ns) TCC1 timer tick and enable the outputs
  REG_TCC2_CTRLA |= TCC_CTRLA_PRESCALER_DIV1 |    // Divide GCLK4 by 1
                    TCC_CTRLA_ENABLE;             // Enable the TCC2 output
  while (TCC2->SYNCBUSY.bit.ENABLE);              // Wait for synchronization

  // Divide the GCLOCK signal by 1 giving in this case 48MHz (20.83ns) TCC1 timer tick and enable the outputs
  REG_TCC0_CTRLA |= TCC_CTRLA_PRESCALER_DIV1 |    // Divide GCLK4 by 1
                    TCC_CTRLA_ENABLE;             // Enable the TCC0 output
  while (TCC0->SYNCBUSY.bit.ENABLE);              // Wait for synchronization
}

void updatePWM(int a, int b)
{
  serialPrintf("Update PWM (a = %d, b = %d)", a, b);
  
  // a -> D10
  // TCC0 CC2 - on D10
  REG_TCC0_CC2 = a;               // TCC0 CC2 = 6 * width in us
  while (TCC0->SYNCBUSY.bit.CC2); // Wait for synchronization
  
  // b -> D11
  // TCC2 CC0 - on D11
  REG_TCC2_CC0 = b;               // TCC2 CC0 = 6 * width in us
  while (TCC2->SYNCBUSY.bit.CC0); // Wait for synchronization
}

// DEBUG

void setupSerial()
{
  Serial.begin(115200);
}

void serialPrintf(const char * format, ...) 
{
  char buffer [256];
  va_list args;
  va_start(args, format);
  vsnprintf(buffer, sizeof(buffer), format, args);
  va_end(args);
  Serial.println(buffer);
}
