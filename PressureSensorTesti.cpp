/*
===============================================================================
 Name        : main.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

#include <cr_section_macros.h>

// TODO: insert other include files here

#include <cstring>
#include <cstdio>

#include "ModBusMaster.h"
#include "SerialPort.h"
#include "word.h"
#include "crc16.h"
#include "i2c_common_15xx.h"
#include "i2cm_15xx.h"
#include "i2cs_15xx.h"
#include "I2C.h"

// TODO: insert other definitions and declarations here

ModbusMaster node(2);

static volatile int counter;
static volatile uint32_t systicks;

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief	Handle interrupt from SysTick timer
 * @return	Nothing
 */
void SysTick_Handler(void)
{
	systicks++;
	if(counter > 0) counter--;
}
#ifdef __cplusplus
}
#endif

void Sleep(int ms)
{
	counter = ms;
	while(counter > 0) {
		__WFI();
	}
}

/* this function is required by the modbus library */
uint32_t millis() {
	return systicks;
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

void i2cTest() {

	int speed=0;
	double pressureReading=0;


	I2C i2c(0, 100000);

	while(1) {

		if((60>(pressureReading-2.5)) || 60>(pressureReading+2.5)){
			if(speed<10000){
				speed=speed+100;
			}
		}
		if((60<(pressureReading-2.5)) || 60<(pressureReading+2.5)){
			if(speed>500){
				speed=speed-100;
			}
		}
		printf("speed= %d\n",speed);
		SetFanSpeed(speed);

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
		Sleep(500);
	}
}




int main(void) {

#if defined (__USE_LPCOPEN)
    // Read clock settings and update SystemCoreClock variable
    SystemCoreClockUpdate();
#if !defined(NO_BOARD_LIB)
    // Set up and initialize all required blocks and
    // functions related to the board hardware
    Board_Init();
    // Set the LED to the state of "On"
    Board_LED_Set(0, true);
#endif
#endif

    // TODO: insert code here

    Chip_SWM_MovablePortPinAssign(SWM_SWO_O, 1, 2);

	SysTick_Config(SystemCoreClock / 1000);

	printf("Started\n");
	Setup();

    i2cTest();

    // Force the counter to be placed into memory
    volatile static int i = 0 ;
    // Enter an infinite loop, just incrementing a counter
    while(1) {
        i++ ;
    }
    return 0 ;
}
