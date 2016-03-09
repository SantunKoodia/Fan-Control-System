/*
 * Sensors.h
 *
 *  Created on: 9 Mar 2016
 *      Author: Svetlana
 */
#include "chip.h"
#include "board.h"
#ifndef SENSORS_H_
#define SENSORS_H_

class Sensors {
public:
	Sensors();
	virtual ~Sensors();
	int get_temp();
	int get_co2();
	void set_temp(int arvo);
	void set_co2(int arvo);
private:
	int temperature;
	int co2;
};

#endif /* SENSORS_H_ */
