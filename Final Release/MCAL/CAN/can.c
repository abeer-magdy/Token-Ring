/*
 * can.c
 *
 *  Created on: Jul 3, 2019
 *      Author: AVE-LAP-005
 */

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_can.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/can.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"


void CAN0_init ()
{
    /*
     * For this example CAN0 is used with RX and TX pins on port B4 and B5.
     * GPIO port B needs to be enabled so these pins can be used.
     */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    /*
     * Configure the GPIO pin muxing to select CAN0 functions for these pins.
     * This step selects which alternate function is available for these pins.
     */
    GPIOPinConfigure(GPIO_PB4_CAN0RX);
    GPIOPinConfigure(GPIO_PB5_CAN0TX);

    /*
     * Enable the alternate function on the GPIO pins.  The above step selects
     * which alternate function is available.  This step actually enables the
     * alternate function instead of GPIO for these pins.
     */
    GPIOPinTypeCAN(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5);

    /*
       // The GPIO port and pins have been set up for CAN.  The CAN peripheral
       // must be enabled.
     */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN0);

    /*
     * Initialize the CAN controller
     */
    CANInit(CAN0_BASE);

    /*
       // Set up the bit rate for the CAN bus.  This function sets up the CAN
       // bus timing for a nominal configuration.  You can achieve more control
       // over the CAN bus timing by using the function CANBitTimingSet() instead
       // of this one, if needed.
       // In this example, the CAN bus is set to 500 kHz.  In the function below,
       // the call to SysCtlClockGet() or ui32SysClock is used to determine the
       // clock rate that is used for clocking the CAN peripheral.  This can be
       // replaced with a  fixed value if you know the value of the system clock,
       // saving the extra function call.  For some parts, the CAN peripheral is
       // clocked by a fixed 8 MHz regardless of the system clock in which case
       // the call to SysCtlClockGet() or ui32SysClock should be replaced with
       // 8000000.  Consult the data sheet for more information about CAN
       // peripheral clocking.
     */

    CANBitRateSet(CAN0_BASE, SysCtlClockGet(), 500000);

    /*
     Enable the CAN for operation.
     */
    CANEnable(CAN0_BASE);


}

void CANsend(uint32_t mesgID, uint8_t * pui8MsgData , uint32_t Mask)
{
    tCANMsgObject sCANMessage;

    /*
     * Initialize the message object that will be used for sending CAN
     * messages.  The message will be 4 bytes that will contain an incrementing
     * value.  Initially it will be set to 0.
     */
    sCANMessage.ui32MsgID = mesgID;
  //  sCANMessage.ui32MsgIDMask = Mask;
    sCANMessage.ui32Flags =0;
    sCANMessage.ui32MsgLen = 1;
    sCANMessage.pui8MsgData = pui8MsgData;

    CANMessageSet(CAN0_BASE, 1, &sCANMessage, MSG_OBJ_TYPE_TX);

}

uint8_t CANrec (uint32_t MsgID, uint32_t msgIDMask)
{
    uint8_t ui8MsgCorrect=0;
    unsigned long ui32Status;
    tCANMsgObject sCANMessage;
    uint8_t pui8MsgData; /* MACRO for the MSG len. */

    /* Initialize a message object to be used for receiving CAN messages with
     any CAN ID.  In order to receive any CAN ID, the ID and mask must both
     be set to 0, and the ID filter enabled.*/

    sCANMessage.ui32MsgID=MsgID;
    sCANMessage.ui32MsgIDMask= msgIDMask;
    sCANMessage.ui32Flags = MSG_OBJ_USE_ID_FILTER;
    sCANMessage.ui32MsgLen = 1; /* MACRO for the MSG len */
    sCANMessage.pui8MsgData = &pui8MsgData;


    /* Now load the message object into the CAN peripheral.  Once loaded the
    CAN will receive any message on the bus */
    CANMessageSet(CAN0_BASE, 1, &sCANMessage, MSG_OBJ_TYPE_RX);



        /* Read the controller status.  This will return a field of status*/
       // ui32Status = CANIntStatus(CAN0_BASE, CAN_INT_STS_CAUSE);
    ui32Status = CANStatusGet(CAN0_BASE, CAN_STS_CONTROL);

        /* If the flag is set, that means that  there is a message ready to be read from the CAN*/

        if(ui32Status & CAN_STATUS_RXOK)
        {


            /* Reuse the same message object that was used earlier to configure
            the CAN for receiving messages.  A buffer for storing the
             received data must also be provided, so set the buffer pointer
             within the message object.*/




            /* Read the message from the CAN. */
            CANMessageGet(CAN0_BASE, 1, &sCANMessage, 0);
            if (sCANMessage.ui32MsgID==MsgID)
            {
                ui8MsgCorrect = 1;
            }
            else
            {
                ui8MsgCorrect = 0;
            }


        }
        else{

            ui8MsgCorrect = 0;

        }
        return ui8MsgCorrect;
}
