/******************************************************************************
* File Name:   main.c
*
* Description: This is the source code for Hello World Example using HAL APIs.
*
* Related Document: See README.md
*
*
*******************************************************************************
* Copyright 2022-2024, Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
*
* This software, including source code, documentation and related
* materials ("Software") is owned by Cypress Semiconductor Corporation
* or one of its affiliates ("Cypress") and is protected by and subject to
* worldwide patent protection (United States and foreign),
* United States copyright laws and international treaty provisions.
* Therefore, you may use this Software only as provided in the license
* agreement accompanying the software package from which you
* obtained this Software ("EULA").
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software
* source code solely for use in connection with Cypress's
* integrated circuit products.  Any reproduction, modification, translation,
* compilation, or representation of this Software except as specified
* above is prohibited without the express written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer
* of such system or application assumes all risk of such use and in doing
* so agrees to indemnify Cypress against all liability.
*******************************************************************************/

#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"

/* WOLFSSL headers */
#include "wolfcrypt/random.h"
#include "wolfcrypt/dilithium.h"


/*******************************************************************************
* Macros
*******************************************************************************/


/*******************************************************************************
* Global Variables
*******************************************************************************/
uint8_t message[10] = "MyMessage";
uint8_t signature[DILITHIUM_LEVEL2_SIG_SIZE];
unsigned int signature_length = sizeof signature;
dilithium_key key;
byte level = 2;     /* DILITHIUM encryption level (2,3,5)*/
WC_RNG rng;
int verRes;

/*******************************************************************************
* Function Prototypes
*******************************************************************************/
cy_rslt_t cy_dilithium_genKey()
{
    int rc;

    /* Initialize RNG */
    rc = wc_InitRng(&rng);
    if (rc != 0)
        goto exit;

    rc = wc_dilithium_init(&key);
    if (rc != 0)
        goto exit;

    rc = wc_dilithium_set_level(&key, level);
    if (rc != 0)
        goto exit;

    rc = wc_dilithium_make_key(&key, &rng);
    if (rc != 0)
        goto exit;

exit:
    return rc;
}


/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
* This is the main function. It sets up a timer to trigger a periodic interrupt.
* The main while loop checks for the status of a flag set by the interrupt and
* toggles an LED at 1Hz to create an LED blinky. Will be achieving the 1Hz Blink
* rate based on the The LED_BLINK_TIMER_CLOCK_HZ and LED_BLINK_TIMER_PERIOD
* Macros,i.e. (LED_BLINK_TIMER_PERIOD + 1) / LED_BLINK_TIMER_CLOCK_HZ = X ,Here,
* X denotes the desired blink rate. The while loop also checks whether the
* 'Enter' key was pressed and stops/restarts LED blinking.
*
* Parameters:
*  none
*
* Return:
*  int
*
*******************************************************************************/
int main(void)
{
    cy_rslt_t result;

#if defined (CY_DEVICE_SECURE)
    cyhal_wdt_t wdt_obj;

    /* Clear watchdog timer so that it doesn't trigger a reset */
    result = cyhal_wdt_init(&wdt_obj, cyhal_wdt_get_max_timeout_ms());
    CY_ASSERT(CY_RSLT_SUCCESS == result);
    cyhal_wdt_free(&wdt_obj);
#endif /* #if defined (CY_DEVICE_SECURE) */

    /* Initialize the device and board peripherals */
    result = cybsp_init();

    /* Board init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Enable global interrupts */
    __enable_irq();

    /* Initialize retarget-io to use the debug UART port */
    result = cy_retarget_io_init_fc(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX,
            CYBSP_DEBUG_UART_CTS,CYBSP_DEBUG_UART_RTS,CY_RETARGET_IO_BAUDRATE);

    /* retarget-io init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    // /* Crypto core enable*/
    // if(Cy_Crypto_Core_Enable(CRYPTO)!=CY_CRYPTO_SUCCESS)
    //     CY_ASSERT(0);

    /* \x1b[2J\x1b[;H - ANSI ESC sequence for clear screen */
    printf("\x1b[2J\x1b[;H");

    result = cy_dilithium_genKey();
    if (result != CY_RSLT_SUCCESS) 
    {
        CY_HALT();
    }

    /* Sign message */
    result = wc_dilithium_sign_msg(message, sizeof message, signature, &signature_length, &key, &rng);
    if (result != 0)
    {
        CY_HALT();
    }

    /* Verify message */
    result = wc_dilithium_verify_msg(signature, signature_length, message, sizeof message, &verRes, &key);
    if (result != 0)
    {
        CY_HALT();
    }

    if (verRes == 1)
    {
        printf("Signature verification succeeded!\r\n");
    }
    else
    {
        printf("Signature verification failed!\r\n");
    }

    while(1);
}




/* [] END OF FILE */
