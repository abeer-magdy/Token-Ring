#include "FreeRTOS.h"
#include "task.h"
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "MCAL/CAN/can.h"
#include "ECUAL/LEDPF3/led.h"
#include "semphr.h"


#define SendMesgID  2
#define recievedMesgID 1
#define MsgData     1
#define msgMaskTx     3
#define msgMaskRx     0xFFFF


BaseType_t Returned;
TaskHandle_t ledInitHandle = NULL;
TaskHandle_t ledToggleHandle = NULL;
TaskHandle_t sendTokenHandle = NULL;
TaskHandle_t taskLedHandle = NULL;
TaskHandle_t receiveTokenHandle = NULL;




TaskHandle_t InitHandle = NULL;
TaskHandle_t TokenRingHandle = NULL;

SemaphoreHandle_t Semaphore_Tx;
SemaphoreHandle_t Semphore_Rx =0;


vApplicationStackOverflowHook(xTaskHandle *pxTask, char *pcTaskName)
{
    /*
     * This function can not return, so loop forever.  Interrupts are disabled
     * on entry to this function, so no processor interrupts will interrupt
     * this loop.
     */
    while(1)
    {}
}

void TokenRingInit( void * pvParameters )
{
    configASSERT( ( ( uint32_t ) pvParameters ) == 1 );
    ledInit();
    CAN0_init ();
    vTaskSuspend( InitHandle );

    for( ;; )
    {

    }
}


void sendToken( void * pvParameters )
{
    configASSERT( ( ( uint32_t ) pvParameters ) == 1 );


    uint32_t mesgID = SendMesgID ;
    uint32_t ui32MsgData = MsgData;
    uint8_t *pui8MsgData;
    pui8MsgData = (uint8_t *)&ui32MsgData;
    CANsend(mesgID, pui8MsgData , msgMaskTx); /* Only For the Starter Node */

    for(;;)
    {
        if( xSemaphoreTake( Semaphore_Tx, 0) )
        {

            CANsend(mesgID, pui8MsgData , msgMaskTx);
        }
        else{}

        vTaskDelay( 1000 );
    }
}

void receiveToken( void * pvParameters )
{
    configASSERT( ( ( uint32_t ) pvParameters ) == 1 );
    uint8_t received;

    for(;;)
    {
        if(Semphore_Rx != NULL )
        {
            received = CANrec (recievedMesgID, msgMaskRx);
            if (received)
            {
                xSemaphoreGive( Semphore_Rx );
            }
            else
            {

            }
        }
        else
        {

        }
        vTaskDelay( 100 );
    }
}



void taskLed( void * pvParameters )
{
    configASSERT( ( ( uint32_t ) pvParameters ) == 1 );

    for( ;; )
    {
        if(Semaphore_Tx != NULL )
        {
            if (xSemaphoreTake( Semphore_Rx, 0) )
            {
                ledOn();
                vTaskDelay( 1000 );
                ledOff();
                xSemaphoreGive( Semaphore_Tx );

            }

            else
            {
            }
        }
        else
        {

        }
        vTaskDelay( 20 );
    }
}

void main(void)
{
    /*SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ); */
    SysCtlClockFreqSet(SYSCTL_OSC_INT | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_320,40000000);

     Semaphore_Tx = xSemaphoreCreateBinary();

     Semphore_Rx = xSemaphoreCreateBinary();

    Returned = xTaskCreate( TokenRingInit,       /* Function that implements the task. */
                            "TokenRingInit",          /* Text name for the task. */
                            configMINIMAL_STACK_SIZE,      /* Stack size in words, not bytes. */
                            ( void * ) 1,    /* Parameter passed into the task. */
                            5,/* Priority at which the task is created. */
                            &InitHandle );



    Returned = xTaskCreate( sendToken,       /* Function that implements the task. */
                            "sendToken",          /* Text name for the task. */
                            configMINIMAL_STACK_SIZE,      /* Stack size in words, not bytes. */
                            ( void * ) 1,    /* Parameter passed into the task. */
                            2,/* Priority at which the task is created. */
                            &sendTokenHandle );


    Returned = xTaskCreate( taskLed,       /* Function that implements the task. */
                            "taskLed",          /* Text name for the task. */
                            configMINIMAL_STACK_SIZE,      /* Stack size in words, not bytes. */
                            ( void * ) 1,    /* Parameter passed into the task. */
                            3,/* Priority at which the task is created. */
                            &taskLedHandle );

    Returned = xTaskCreate( receiveToken,       /* Function that implements the task. */
                            "receiveToken",          /* Text name for the task. */
                            configMINIMAL_STACK_SIZE,      /* Stack size in words, not bytes. */
                            ( void * ) 1,    /* Parameter passed into the task. */
                            4,/* Priority at which the task is created. */
                            &receiveTokenHandle );





    /* Start Scheduler */
    vTaskStartScheduler();

    while(1)
    {

    }
}
