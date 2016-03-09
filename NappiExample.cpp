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
#include "lcd_port.h"
#include "BarGraph.h"
#include "SliderEdit.h"
#include "LiquidCrystal.h"
#include "SimpleMenu.h"
#include "IntegerEdit.h"
#include "DecimalEdit.h"
#include <cstdio>
bool fast=false;

// TODO: insert other definitions and declarations here
	//const int buttonPort[] = {0,0,1,1};
	//const int buttonPin[] = {0,16,9,10};

	const int buttonPort[] = {0,0,1,0};
	const int buttonPin[] = {10,16,3,0};

void SetButtons(){

	int buttonAmount=3;
	int idx;
	for(idx = 0;idx<buttonAmount;idx++){
		Chip_IOCON_PinMuxSet(LPC_IOCON, buttonPort[idx],buttonPin[idx] , (IOCON_MODE_PULLUP | IOCON_DIGMODE_EN));
		Chip_GPIO_SetPinDIRInput(LPC_GPIO, buttonPort[idx],buttonPin[idx]);
	}


}

int CheckButtons(){



	if(!Chip_GPIO_GetPinState(LPC_GPIO,buttonPort[0],buttonPin[0])){
		while(!Chip_GPIO_GetPinState(LPC_GPIO,buttonPort[0],buttonPin[0])){

		}
		return 1;
	}
	if(!Chip_GPIO_GetPinState(LPC_GPIO,buttonPort[1],buttonPin[1])){
		while(!Chip_GPIO_GetPinState(LPC_GPIO,buttonPort[1],buttonPin[1])){

		}
		return 2;
	}
	if(!Chip_GPIO_GetPinState(LPC_GPIO,buttonPort[2],buttonPin[2])){
		while(!Chip_GPIO_GetPinState(LPC_GPIO,buttonPort[2],buttonPin[2])){
			if(fast==true){
				return 3;
			}
		}
		return 3;
	}
	if(!Chip_GPIO_GetPinState(LPC_GPIO,buttonPort[3],buttonPin[3])){
		while(!Chip_GPIO_GetPinState(LPC_GPIO,buttonPort[3],buttonPin[3])){
			if(fast==true){
				return 4;
			}
		}
		return 4;
	}

	return 0;


}



int main(void) {

#if defined (__USE_LPCOPEN)
    // Read clock settings and update SystemCoreClock variable
    SystemCoreClockUpdate();
#if !defined(NO_BOARD_LIB)
    // Set up and initialize all required blocks and
    // functions related to the board hardware
    Board_Init();
    Chip_RIT_Init(LPC_RITIMER);
    NVIC_EnableIRQ(RITIMER_IRQn);
    // Set the LED to the state of "On"
    Board_LED_Set(0, true);
#endif
#endif

    // TODO: insert code here

    LiquidCrystal lcd(8, 9, 10, 11, 12, 13);

    lcd.begin(16,2);
    lcd.setCursor(0,0);

	for(int i=4;i<8;i++){
		pinMode(i,LOW);
	}

    //IntegerEdit::IntegerEdit(LiquidCrystal& lcd_, int upperMax, int lowerMax, std::string editTitle): lcd(lcd_), title(editTitle)

    SimpleMenu menu;
    //IntegerEdit temperature(lcd,30,15, std::string("Temperature"));
    IntegerEdit pressure(lcd,99,0, std::string("Pressure"));
    IntegerEdit humidity(lcd,50,20, std::string("Humidity"));
    DecimalEdit temperature(lcd,30,20,std::string("Temperature"));
    SliderEdit fanSpeed(lcd,50,0,std::string("Fan"));
    menu.addItem(new MenuItem(temperature));
    menu.addItem(new MenuItem(pressure));
    menu.addItem(new MenuItem(humidity));
    menu.addItem(new MenuItem(fanSpeed));
    //menu.addItem(new MenuItem(testi));
    temperature.setValue(25.5);
    pressure.setValue(89);
    humidity.setValue(42);
    fanSpeed.setValue(25);
    //testi.setValue(25.5);


    /*
    menu.event(MenuItem::down);
    menu.event(MenuItem::ok);
    menu.event(MenuItem::up);
    menu.event(MenuItem::back);
     */
    //SetButtons();
    menu.event(MenuItem::show);
    int button=0;
    while(true){
		delayMicroseconds(1000);
    	button=CheckButtons();
    	//printf("nappi:%d\n",button);


    	switch(button){
    	case 4:
    		menu.event(MenuItem::up);
    		break;
    	case 3:
    		 menu.event(MenuItem::down);
    		 break;
    	case 2:
    		fast=!fast;
    		menu.event(MenuItem::ok);
    		delayMicroseconds(1000);
    		break;
    	case 1:
    		fast=false;
    		menu.event(MenuItem::back);
    		break;
    	}


    }

    // Force the counter to be placed into memory
    volatile static int i = 0 ;
    // Enter an infinite loop, just incrementing a counter
    while(1) {
        i++ ;
    }
    return 0 ;
}
