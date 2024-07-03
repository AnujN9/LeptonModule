This example is meant for Raspberry Pi4 and Lepton 3.x.

First enable the SPI and I2C interfaces on the Pi.
```
sudo raspi-config
```

To build, cd to the *LeptonModule/sofware* folder, then run:
```
mkdir build
cd build
cmake ..
make
```

### Lepton 3.x

To run the program while still in the build directory, using a FLIR Lepton 3.x camera core use the following code:
```
./raspberrypi_video_network
```

----

In order for the application to run properly, a Lepton camera must be attached in a specific way to the SPI, power, and ground pins of the Raspi's GPIO interface, as well as the I2C SDA/SCL pins:

Lepton's GND pin should be connected to RasPi's ground.
Lepton's CS pin should be connected to RasPi's CE1 pin.
Lepton's MISO pin should be connected to RasPI's MISO pin.
Lepton's CLK pin should be connected to RasPI's SCLK pin.
Lepton's VIN pin should be connected to RasPI's 3v3 pin.
Lepton's SDA pin should be connected to RasPI's SDA pin.
Lepton's SCL pin should be connected to RasPI's SCL pin.
