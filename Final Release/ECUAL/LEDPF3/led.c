/*
 * led.c
 *
 *  Created on: Jul 4, 2019
 *      Author: Abeer
 */

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"




void ledInit()
{
    /*
    * Enable the GPIO port that is used for the on-board LED on PF3
    */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    /*
    * Check if the peripheral access is enabled.
    */
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF))
    {
    }

    /*
    * Enable the GPIO pin for the LED (PF3).  Set the direction as output, and
    * enable the GPIO pin for digital function.
    */

    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3);

    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x0);
}

void ledOn()
{
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);
}

void ledOff()
{
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x0);
}
