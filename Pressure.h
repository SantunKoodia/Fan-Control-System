/*
 * Pressure.h
 *
 *  Created on: 9.3.2016
 *      Author: Santtu
 */

#ifndef PSINCLUDES_PRESSURE_H_
#define PSINCLUDES_PRESSURE_H_

#include "I2C.h"
#include <cstring>
#include <cstdio>

#include "../includit/ModBusMaster.h"
#include "../includit/SerialPort.h"
#include "../includit/word.h" //tarviikk??
#include "../includit/crc16.h" //tarviiko??
#include "I2C.h"

class Pressure {
public:
	Pressure();
	virtual ~Pressure();
	int GetPressure();
};

#endif /* PSINCLUDES_PRESSURE_H_ */
