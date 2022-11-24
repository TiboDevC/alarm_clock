//**************************************************************************
// FreeRtos on Samd21
// By Scott Briscoe
//
// Project is a simple example of how to get FreeRtos running on a SamD21 processor
// Project can be used as a template to build your projects off of as well
//
// This projects sets up a external gpio interrupt to send task messages to a task
//
//**************************************************************************

#include <Arduino.h> //required to prevent a compiling error when a cpp header file tries to include this library

#include <FreeRTOS.h>
#include <task.h>

#include "button.hpp"
#include "wifi_tool.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "DEV_Config.h"
#include "EPD.h"
#include "GUI_Paint.h"
#include "alarm_clock_fsm.h"
#include "rtc_tool.h"
#include "screen.h"
#include "timer.h"

#ifdef __cplusplus
}
#endif


//**************************************************************************
// Type Defines and Constants
//**************************************************************************

#define ERROR_LED_PIN 13 // Led Pin: Typical Arduino Board

#define BUTTON_PIN 16

#define ERROR_LED_LIGHTUP_STATE \
	HIGH // the state that makes the led light up on your board, either low or high

// Select the serial port the project should use and communicate over
// Some boards use SerialUSB, some use Serial
// #define SERIAL SerialUSB // Sparkfun Samd51 Boards

//**************************************************************************
// global variables
//**************************************************************************
TaskHandle_t Handle_aTask;
TaskHandle_t Handle_bTask;
TaskHandle_t Handle_monitorTask;

//**************************************************************************
// Can use these function for RTOS delays
// Takes into account processor speed
// Use these instead of delay(...) in rtos tasks
//**************************************************************************
void myDelayUs(int us)
{
	vTaskDelay(us / portTICK_PERIOD_US);
}

void myDelayMs(int ms)
{
	vTaskDelay((ms * 1000) / portTICK_PERIOD_US);
}

void myDelayMsUntil(TickType_t *previousWakeTime, int ms)
{
	vTaskDelayUntil(previousWakeTime, (ms * 1000) / portTICK_PERIOD_US);
}

//*****************************************************************

void setup()
{
	SerialUSB.begin(115200);

	delay(1000); // prevents usb driver crash on startup, do not omit this
	while (!SerialUSB)
		; // Wait for serial terminal to open port before starting program

	SerialUSB.println("");
	SerialUSB.println("******************************");
	SerialUSB.println("        Program start         ");
	SerialUSB.println("******************************");
	SerialUSB.flush();

	alarm_clock_fsm();

	//    ui_start_task();

	// Start the RTOS, this function will never return and will schedule the tasks.
	vTaskStartScheduler();

	// error scheduler failed to start
	// should never get here
	while (1) {
		SerialUSB.print(".");
		SerialUSB.println("Scheduler Failed! \n");
		SerialUSB.flush();
		delay(1000);
	}
}

//*****************************************************************
// This is now the rtos idle loop
// No rtos blocking functions allowed!
//*****************************************************************
void loop()
{
	// Optional commands, can comment/uncomment below
	SerialUSB.print("."); // print out dots in terminal, we only do this when the RTOS is in the idle state
	delay(1000);       // delay is interrupt friendly, unlike vNopDelayMS
}


//*****************************************************************
