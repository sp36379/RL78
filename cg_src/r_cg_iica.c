/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2015, 2021 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : r_cg_iica.c
* Version      : Code Generator for RL78/I1C V1.01.07.02 [08 Nov 2021]
* Device(s)    : R5F11TLG
* Tool-Chain   : CCRL
* Description  : This file implements device driver for IICA module.
* Creation Date: 21-09-2023
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_iica.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
volatile uint8_t   g_iica0_master_status_flag; /* iica0 master flag */
volatile uint8_t   g_iica0_slave_status_flag;  /* iica0 slave flag */
volatile uint8_t * gp_iica0_rx_address;        /* iica0 receive buffer address */
volatile uint16_t  g_iica0_rx_len;             /* iica0 receive data length */
volatile uint16_t  g_iica0_rx_cnt;             /* iica0 receive data count */
volatile uint8_t * gp_iica0_tx_address;        /* iica0 send buffer address */
volatile uint16_t  g_iica0_tx_cnt;             /* iica0 send data count */
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_IICA0_Create
* Description  : This function initializes the IICA0 module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_IICA0_Create(void)
{
    IICA0EN = 1U;   /* enables input clock supply */
    IICE0 = 0U;     /* stop operation */
    IICAMK0 = 1U;   /* disable INTIICA0 interrupt */
    IICAIF0 = 0U;   /* clear INTIICA0 interrupt flag */
    /* Set INTIICA0 low priority */
    IICAPR10 = 1U;
    IICAPR00 = 1U;
    /* Set SCLA0, SDAA0 pin */
    P6 &= 0xFCU;
    PM6 |= 0x03U;
    SMC0 = 1U;      /* operates in fast mode */
    IICWL0 = _68_IICA_LOW_WIDTH;
    IICWH0 = _60_IICA_HIGH_WIDTH;
    DFC0 = 0U;      /* digital filter off */
    IICCTL01 |= _00_IICA_FCLK_SELECTED;
    SVA0 = _10_IICA0_MASTERADDRESS;
    STCEN0 = 1U;    /* enable generation of a start condition without detecting a stop condition */
    IICRSV0 = 1U;   /* disable communication reservation */
    SPIE0 = 0U;     /* disable */
    WTIM0 = 1U;     /* interrupt request is generated at the ninth clock's falling edge */
    ACKE0 = 1U;     /* enable acknowledgment */
    IICAMK0 = 0U;   /* enable INTIICA0 interrupt */
    IICE0 = 1U;     /* enable operation */
    LREL0 = 1U;     /* this exits from the current communications and sets standby mode */
    /* Set SCLA0, SDAA0 pin */
    P6 &= 0xFCU;
    PM6 &= 0xFCU;
}
/***********************************************************************************************************************
* Function Name: R_IICA0_Reset
* Description  : This function resets the IICA0 module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_IICA0_Reset(void)
{
    IICA0RES = 1U;  /* reset serial interface IICA0 */
}
/***********************************************************************************************************************
* Function Name: R_IICA0_Stop
* Description  : This function stops IICA0 module operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_IICA0_Stop(void)
{
    IICE0 = 0U;     /* stop operation */
}
/***********************************************************************************************************************
* Function Name: R_IICA0_StopCondition
* Description  : This function sets IICA0 stop condition flag.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_IICA0_StopCondition(void)
{
    SPT0 = 1U;      /* stop condition is generated */
}
/***********************************************************************************************************************
* Function Name: R_IICA0_Master_Send
* Description  : This function starts to send data as master mode.
* Arguments    : adr -
*                    send address
*                tx_buf -
*                    transfer buffer pointer
*                tx_num -
*                    buffer size
*                wait -
*                    wait for start condition
* Return Value : status -
*                    MD_OK or MD_ERROR1 or MD_ERROR2
***********************************************************************************************************************/
MD_STATUS R_IICA0_Master_Send(uint8_t adr, uint8_t * const tx_buf, uint16_t tx_num, uint8_t wait)
{
    MD_STATUS status = MD_OK;

    IICAMK0 = 1U;   /* disable INTIICA0 interrupt */

    if ((1U == IICBSY0) && (0U == MSTS0))
    {
        /* Check bus busy */
        IICAMK0 = 0U;   /* enable INTIICA0 interrupt */
        status = MD_ERROR1;
    }
    else
    {
        STT0 = 1U;      /* generate a start condition */
        IICAMK0 = 0U;   /* enable INTIICA0 interrupt */
        
        /* Wait */
        while (wait--)
        {
            ;
        }
        
        if (0U == STD0)
        {
            status = MD_ERROR2;
        }
		
        /* Set parameter */
        g_iica0_tx_cnt = tx_num;
        gp_iica0_tx_address = tx_buf;
        g_iica0_master_status_flag = _00_IICA_MASTER_FLAG_CLEAR;
        adr &= (uint8_t)~0x01U; /* set send mode */
        IICA0 = adr; /* send address */
    }

    return (status);
}
/***********************************************************************************************************************
* Function Name: R_IICA0_Master_Receive
* Description  : This function starts to receive IICA0 data as master mode.
* Arguments    : adr -
*                    receive address
*                rx_buf -
*                    receive buffer pointer
*                rx_num -
*                    buffer size
*                wait -
*                    wait for start condition
* Return Value : status -
*                    MD_OK or MD_ERROR1 or MD_ERROR2
***********************************************************************************************************************/
MD_STATUS R_IICA0_Master_Receive(uint8_t adr, uint8_t * const rx_buf, uint16_t rx_num, uint8_t wait)
{
    MD_STATUS status = MD_OK;

    IICAMK0 = 1U;   /* disable INTIICA0 interrupt */

    if ((1U == IICBSY0) && (0U == MSTS0))
    {
        /* Check bus busy */
        IICAMK0 = 0U;   /* enable INTIICA0 interrupt */
        status = MD_ERROR1;
    }
    else
    {
        STT0 = 1U;      /* generate a start condition */
        IICAMK0 = 0U;   /* enable INTIICA0 interrupt */
        
        /* Wait */
        while (wait--)
        {
            ;
        }
        
        if (0U == STD0)
        {
            status = MD_ERROR2;
        }
		
        /* Set parameter */
        g_iica0_rx_len = rx_num;
        g_iica0_rx_cnt = 0U;
        gp_iica0_rx_address = rx_buf;
        g_iica0_master_status_flag  = _00_IICA_MASTER_FLAG_CLEAR;
        adr |= 0x01U; /* set receive mode */
        IICA0 = adr; /* receive address */
    }

    return (status);
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
