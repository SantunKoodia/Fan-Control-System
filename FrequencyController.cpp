/*
 * FreqContrl.cpp
 *
 *  Created on: 10.3.2016
 *      Author: Santtu
 */

#include "FreqContrl.h"
#include "ModBusMaster.h"
#include "SerialPort.h"
#include "word.h"
#include "crc16.h"

FreqContrl::FreqContrl() {
	// TODO Auto-generated constructor stub

}

FreqContrl::~FreqContrl() {
	// TODO Auto-generated destructor stub
}


void printRegister(ModbusMaster& node, uint16_t reg) {
	uint8_t result;
	// slave: read 16-bit registers starting at reg to RX buffer
	result = node.readHoldingRegisters(reg, 1);

	// do something with data if read is successful
	if (result == node.ku8MBSuccess)
	{
		printf("R%d=%04X\n", reg, node.getResponseBuffer(0));
	}
	else {
		printf("R%d=???\n", reg);
	}
}

bool setFrequency(ModbusMaster& node, uint16_t freq) {
	uint8_t result;
	int ctr;
	bool atSetpoint;
	const int delay = 500;

	node.writeSingleRegister(1, freq); // set motor frequency

	printf("Set freq = %d\n", freq/40); // for debugging

	// wait until we reach set point or timeout occurs
	ctr = 0;
	atSetpoint = false;
	do {
		Sleep(delay);
		// read status word
		result = node.readHoldingRegisters(3, 1);
		// check if we are at setpoint
		if (result == node.ku8MBSuccess) {
			if(node.getResponseBuffer(0) & 0x0100) atSetpoint = true;
		}
		ctr++;
	} while(ctr < 20 && !atSetpoint);

	printf("Elapsed: %d\n", ctr * delay); // for debugging

	return atSetpoint;
}

void Setup(){
	node.begin(9600); // set transmission rate - other parameters are set inside the object and can't be changed here

		printRegister(node, 3); // for debugging

		node.writeSingleRegister(0, 0x0406); // prepare for starting

		printRegister(node, 3); // for debugging

		Sleep(1000); // give converter some time to set up
			// note: we should have a startup state machine that check converter status and acts per current status
			//       but we take the easy way out and just wait a while and hope that everything goes well

		printRegister(node, 3); // for debugging

		node.writeSingleRegister(0, 0x047F); // set drive to start mode

		printRegister(node, 3); // for debugging

		Sleep(1000); // give converter some time to set up
			// note: we should have a startup state machine that check converter status and acts per current status
			//       but we take the easy way out and just wait a while and hope that everything goes well

		printRegister(node, 3); // for debugging

		int j = 0;


				uint8_t result;

						// slave: read (2) 16-bit registers starting at register 102 to RX buffer
						j = 0;
						do {
							result = node.readHoldingRegisters(102, 2);
							j++;
						} while(j < 3 && result != node.ku8MBSuccess);
						// note: sometimes we don't succeed on first read so we try up to threee times
						// if read is successful print frequency and current (scaled values)
						if (result == node.ku8MBSuccess) {
							printf("F=%4d, I=%4d  (ctr=%d)\n", node.getResponseBuffer(0), node.getResponseBuffer(1),j);
						}
						else {
							printf("ctr=%d\n",j);
						}
}



void SetFanSpeed(int inputSpeed){

	int speed = inputSpeed;

		Sleep(500);
		setFrequency(node, speed);



}
