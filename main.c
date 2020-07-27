
#include "DE1SoC_LT24/DE1SoC_LT24.h"
#include "HPS_Watchdog/HPS_Watchdog.h"
#include "HPS_usleep/HPS_usleep.h"
#include "HPS_PrivateTimer/HPS_PrivateTimer.h"
#include "Graphics.h"
#include "HPS_IRQ/HPS_IRQ.h"

//Array Length Helper
#define ARRAY_LEN(arr) (sizeof(arr)/sizeof(arr[0]))

//colours are defined globally since they are to be accessed by interrupts also
unsigned short colours[6] = {LT24_RED,LT24_YELLOW,LT24_GREEN,LT24_CYAN,LT24_BLUE,LT24_MAGENTA};

//Debugging Function
#include <stdlib.h>
void exitOnFail(signed int status, signed int successStatus){
    if (status != successStatus) {
        exit((int)status); //Add breakpoint here to catch failure
    }
}

//Key Released Interrupt Displays Last Button Released
void pushbuttonISR(HPSIRQSource interruptID, bool isInit, void* initParams) {
    if (!isInit) {
		volatile unsigned int * LEDR_ptr     = (unsigned int *) 0xFF200000;
        volatile unsigned int * KEY_ptr     = (unsigned int *) 0xFF200050;
        volatile unsigned int * HEX0to3_ptr = (unsigned int *) 0xFF200020;
        unsigned int press;
        unsigned int i;
        unsigned int hexVal;
        unsigned char* hexValPtr = (unsigned char*)&hexVal;
        //Read the Push-button interrupt register
        press = KEY_ptr[3];
        * LEDR_ptr= press;
        //Then clear the interrupt flag by writing the value back
        KEY_ptr[3] = press;
        //Prepare to display 1 or 0 on 7-seg corresponding to whichever keys are pressed.
        for ( i = 0; i < 4; i++) {
            hexValPtr[i] = (press & (1 << i)) ? 0x06 : 0x3F;
        }
        HPS_ResetWatchdog();
        // Display on 7-seg LEDs
        HEX0to3_ptr[0] = hexVal;
        //if KEY0 is released
        if(press == 0x1){
        	LT24_clearDisplay(LT24_BLACK);
        	Graphics_drawBox(rand()%240,rand()%320,rand()%240,rand()%320,colours[rand()%6],0,colours[rand()%6]);
        }
        //if KEY1 is released
        else if(press==0x2){
        	LT24_clearDisplay(LT24_BLACK);
        	Graphics_drawCircle(rand()%240,rand()%320, rand()%50, colours[rand()%6], 0, colours[rand()%6]);
        }
        //if KEY2 is released
        else if(press==0x4){
        	LT24_clearDisplay(LT24_BLACK);
        	Graphics_drawTriangle(rand()%240,rand()%320,rand()%240,rand()%320,rand()%240,rand()%320,colours[rand()%6],0,colours[rand()%6]); ResetWDT();
        }
        //if KEY3 is released
        else{
        	LT24_clearDisplay(LT24_BLACK);
        }
    }
    //Reset watchdog.
    HPS_ResetWatchdog();
}

//
// Main Function
// =============
int main(void) {
    //Variables
    unsigned int shapeIdx = 0;
    volatile unsigned int initial_timer_value;
    volatile unsigned int initial_timer_value2;
    volatile unsigned int final_timer_value;
    volatile unsigned int  timer_diff;
    volatile unsigned int * KEY_ptr = (unsigned int *) 0xFF200050;

    HPS_ResetWatchdog();

    //Initialise IRQs
    HPS_IRQ_initialise(NULL);
    HPS_ResetWatchdog();

    // Configure Push Buttons to interrupt on press
    KEY_ptr[2] = 0xF;     // Enable interrupts for all four KEYs
    // Register interrupt handler for keys
    HPS_IRQ_registerHandler(IRQ_LSC_KEYS, pushbuttonISR);
    HPS_ResetWatchdog();
    exitOnFail(
        		Timer_initialise(0xFFFEC600),  //Initialise Audio Codec
                TIMER_SUCCESS);                //Exit if not successful
    //Initialise the Audio Codec.
    exitOnFail(
          		set_prescaler(249),  //Initialise Audio Codec
                TIMER_SUCCESS);                //Exit if not successful
    //Initialise the Audio Code
    exitOnFail(
           		set_load(0xFFFFFFFF),  //Initialise Audio Codec
                TIMER_SUCCESS);                //Exit if not successful
    //Initialise the Audio Codec.
    exitOnFail(
           		set_control(11),  //Initialise Audio Codec
                TIMER_SUCCESS);                //Exit if not successful
    //Wait a moment
    usleep(500000);
    //Initialise the LCD Display.
    exitOnFail(
    		Graphics_initialise(0xFF200060,0xFF200080),
			GRAPHICS_SUCCESS);
    ResetWDT();
    initial_timer_value=read_current_timer();
    //Rectangle. Red Border. Grey Fill.
    Graphics_drawBox(10,10,230,310,LT24_RED,false,0x39E7);
    final_timer_value=read_current_timer();
    timer_diff = initial_timer_value - final_timer_value;
    ResetWDT();
    //Circle. Yellow Border, White Fill. Centre of screen. 100px radius
    Graphics_drawCircle(120,160,100,LT24_YELLOW,false,LT24_WHITE); ResetWDT();
    //Circle. Blue Border, No Fill. Centre of screen. 102px radius
    Graphics_drawCircle(120,160,102,LT24_BLUE,true,0); ResetWDT();
    //Rectangle. Cyan Border, No Fill.
    Graphics_drawBox(49,89,191,231,LT24_CYAN,true,0); ResetWDT();
    //Line. Green. 45 degree Radius of circle.
    Graphics_drawLine(191,89,120,160,LT24_GREEN); ResetWDT();
    //Line. Magenta. 270 degree Radius of circle.
    Graphics_drawLine(120,160,20,160,LT24_MAGENTA); ResetWDT();
    //Triangle. Blue Border, No Fill. Bottom left corner. Right-angle triangle.
    Graphics_drawTriangle(18,283,18,302,37,302,LT24_BLUE,true,0); ResetWDT();
    //Triangle. Yellow Border, Green Fill. Bottom left corner Equilateral triangle.
    Graphics_drawTriangle(213,283,204,302,222,302,LT24_YELLOW,false,LT24_GREEN);
    //Done.
    //wait for a moment
    usleep(200000);
    //clear display
    LT24_clearDisplay(LT24_BLACK);
    HPS_ResetWatchdog();
    //Main Run Loop
    while (1) {
    	//if key is not pressed
    	if(*KEY_ptr == 0x00000000){
    		usleep(500000);
    		initial_timer_value=read_current_timer();
    		//random values are given as arguments
    		if(shapeIdx==0){
    			exitOnFail(
    			Graphics_drawLine(rand()%240,rand()%320,rand()%240,rand()%320,colours[rand()%6]),
				GRAPHICS_SUCCESS);
    		}
    		else if(shapeIdx==1){
    			exitOnFail(
    			Graphics_drawBox(rand()%240,rand()%320,rand()%240,rand()%320,colours[rand()%6],1,colours[rand()%6]),
    			GRAPHICS_SUCCESS);
    		}
    		else if(shapeIdx==2){
    			exitOnFail(
    			Graphics_drawCircle(rand()%240,rand()%320, rand()%50, colours[rand()%6], 1, colours[rand()%6]),
    			GRAPHICS_SUCCESS);
    		}
    		else if(shapeIdx==3){
    			exitOnFail(
    			Graphics_drawTriangle(rand()%240,rand()%320,rand()%240,rand()%320,rand()%240,rand()%320,colours[rand()%6],1,colours[rand()%6]),
    			GRAPHICS_SUCCESS);
    		}
    		else{
    			exitOnFail(
    			Graphics_drawRandom((rand()%4)+3,colours[rand()%6]),
				GRAPHICS_SUCCESS);
    		}
    		final_timer_value=read_current_timer();
    		timer_diff = initial_timer_value - final_timer_value;
    		//Update Shape Index
    		shapeIdx++;
    		if (shapeIdx >= 5) {
    			shapeIdx = 0;
    		}
    		//Finally reset the watchdog.
    		HPS_ResetWatchdog();
    	}
    }
}
