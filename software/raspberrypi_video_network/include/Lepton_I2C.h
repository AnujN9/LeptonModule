#ifndef LEPTON_I2C
#define LEPTON_I2C

/// \file lepton_i2c.h
/// \brief The I2C communication functions for the Lepton 3.1R thermal camera.

/// \brief Connects to the Lepton I2C port.
/// \return 0 if successful.
int lepton_connect();

/// \brief Performs the Flat Field Correction (FFC) on the Lepton camera.
void lepton_perform_ffc();

/// \brief Reboots the Lepton camera, resetting internal states.
void lepton_reboot();

#endif