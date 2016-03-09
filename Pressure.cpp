/*
 * Pressure.cpp
 *
 *  Created on: 9.3.2016
 *      Author: Santtu
 */

#include "Pressure.h"

Pressure::Pressure() {
	// TODO Auto-generated constructor stub

}

Pressure::~Pressure() {
	// TODO Auto-generated destructor stub
}

int Pressure::GetPressure(){


	double pressureReading=0;


	I2C i2c(0, 100000);

	while(1) {

		uint8_t pressureData[3];
		uint8_t readPressureCmd = 0xF1;
		int16_t pressure = 0;


		if (i2c.transaction(0x40, &readPressureCmd, 1, pressureData, 3)) {
			/* Output temperature. */
			pressure = (pressureData[0] << 8) | pressureData[1];
			pressureReading = (pressure/240.0)*0.95;
			DEBUGOUT("Pressure read over I2C is %.1f Pa\r\n",	(pressure/240.0)*0.95);
		}
		else {
			DEBUGOUT("Error reading pressure.\r\n");
		}
		//Sleep(500);
	}

	return pressureReading;

}
