# Universal Arduino Library

## Getting Started

### Using the Arduino IDE Library Manager

1. Choose ```Sketch``` -> ```Include Library``` -> ```Manage Libraries...```
2. Type ```Universal``` into the search box.
3. Click the row to select the library.
4. Click the ```Install``` button to install the library.

__Dependencies__

Universal has the following dependencies:

* [ble-sdk-arduino](https://github.com/NordicSemiconductor/ble-sdk-arduino)

## Documentation

## Contributing

## Compatible Hardware

### [Nordic Semiconductor nRF8001](http://www.nordicsemi.com/eng/Products/Bluetooth-R-low-energy/nRF8001)

 * [Adafruit](http://www.adafruit.com)
   * [Bluefruit LE - nRF8001 Breakout](http://www.adafruit.com/products/1697)
 * [RedBearLab](http://redbearlab.com)
   * [BLE Shield](http://redbearlab.com/bleshield/)
   * [Blend Micro](http://redbearlab.com/blendmicro/)
   * [Blend](http://redbearlab.com/blend/)

#### Pinouts

| Chip | Shield/Board | REQ Pin | RDY Pin | RST Pin |
| ---- | ------------ | ------- | ------- | ------- |
| nRF8001|
| | Bluefruit LE | 10 | 2 | 9 |
| | BLE Shield 1.x | 9 | 8 | UNUSED |
| | BLE Shield 2.x | 9 | 8 | UNUSED or 4/7 via jumper|
| | Blend | 9 | 8 | UNUSED or 4/5 via jumper |
| | Blend Micro | 6 | 7 | UNUSED or 4 |

## Related Projects

* Sandeep Mistry's [BLEPeripheral](https://github.com/sandeepmistry/arduino-BLEPeripheral)
* RedBearLab's [nRF8001](https://github.com/RedBearLab/nRF8001)