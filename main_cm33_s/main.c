/******************************************************************************
* File Name:   main.c
*
* Description: This is the source code for the Event based TCPWM Example
*              for ModusToolbox.
*
* Demonstrates  Event based TCPWM
*
*
********************************************************************************
* (c) 2026, Infineon Technologies AG, or an affiliate of Infineon
* Technologies AG. All rights reserved.
* This software, associated documentation and materials ("Software") is
* owned by Infineon Technologies AG or one of its affiliates ("Infineon")
* and is protected by and subject to worldwide patent protection, worldwide
* copyright laws, and international treaty provisions. Therefore, you may use
* this Software only as provided in the license agreement accompanying the
* software package from which you obtained this Software. If no license
* agreement applies, then any use, reproduction, modification, translation, or
* compilation of this Software is prohibited without the express written
* permission of Infineon.
*
* Disclaimer: UNLESS OTHERWISE EXPRESSLY AGREED WITH INFINEON, THIS SOFTWARE
* IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* INCLUDING, BUT NOT LIMITED TO, ALL WARRANTIES OF NON-INFRINGEMENT OF
* THIRD-PARTY RIGHTS AND IMPLIED WARRANTIES SUCH AS WARRANTIES OF FITNESS FOR A
* SPECIFIC USE/PURPOSE OR MERCHANTABILITY.
* Infineon reserves the right to make changes to the Software without notice.
* You are responsible for properly designing, programming, and testing the
* functionality and safety of your intended application of the Software, as
* well as complying with any legal requirements related to its use. Infineon
* does not guarantee that the Software will be free from intrusion, data theft
* or loss, or other breaches ("Security Breaches"), and Infineon shall have
* no liability arising out of any Security Breaches. Unless otherwise
* explicitly approved by Infineon, the Software may not be used in any
* application where a failure of the Product or any consequences of the use
* thereof can reasonably be expected to result in personal injury.
*******************************************************************************/

#include "cy_pdl.h"
#include "cybsp.h"

/*******************************************************************************
* Macros
*******************************************************************************/

/* These are the addresses where the core0 and core1 images are located. */
#define CORE0_IMAGE_ADDRESS    CYMEM_CM33_0_S_m33s_ppca0_nvm_C_S_START
#define CORE1_IMAGE_ADDRESS    CYMEM_CM33_0_S_m33s_ppca1_nvm_C_S_START

#define PPCA0_IMAGE_SIZE       CYMEM_CM33_0_S_ppca0_code_SIZE
#define PPCA1_IMAGE_SIZE       CYMEM_CM33_0_S_ppca1_code_SIZE

/* These are the addresses that the other application running on PPCA cores should be using to update. */
#define PPCA_M1_VAR_ADDRESS   0x53020400
#define PPCA_M3_VAR_ADDRESS   0x53040800

/*******************************************************************************
* Global Variables
*******************************************************************************/

/*******************************************************************************
* Function Prototype
*******************************************************************************/

/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
* This is the main function for CM33 CPU. It does...
*    1. Initialize the PPCA, TCPWM, Processing unit, combiners
*    2. Provide the software start to the PWM and mask PWM
*
* Parameters:
*  void
*
* Return:
*  int
*
*******************************************************************************/
int main(void)
{
    cy_rslt_t result;

    /* Initialize the device and board peripherals */
    result = cybsp_init();
    
    /* Board init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
       CY_ASSERT(0);
    }

    /* enable interrupts */
    __enable_irq();

    /* Initializing and starting PPCA CPU Core 0 and PPCA CPU Core 1. */
    Cy_System_Init_CPU0((void*)CORE0_IMAGE_ADDRESS, PPCA0_IMAGE_SIZE);
    Cy_System_Init_CPU1((void*)CORE1_IMAGE_ADDRESS, PPCA1_IMAGE_SIZE);

    /* Enabling the PPCA Configuration. */
    Cy_PPCA_Enable(CNFG_MASK_HW);
    Cy_PPCA_CNFG_PPCA_Output_Selector(CNFG_MASKCNFG_HW, &CNFG_MASK_ppcaOutConfig);
    
    /* EPU configuration */
    Cy_PPCA_EPU_Enable(EPU_BLK_HW);

    /* Configuring the processing unit */
    Cy_PPCA_EPU_PU_T2_Configure(MASK_PU_T2_HW, MASK_PU_T2_INDEX, &MASK_PU_T2_put2_config);
    Cy_PPCA_EPU_PU_T2_Enable(MASK_PU_T2_HW, MASK_PU_T2_INDEX,CY_ENABLE_ASYNC_BYPASS);
    
    /* Configuring the combiner */
    Cy_PPCA_EPU_Combo_Configure(MASK_COMBINER_HW, MASK_COMBINER_INDEX, &MASK_COMBINER_combo_config);
    
    /* Initializing TCPWM as PWM */
    if (CY_TCPWM_SUCCESS != Cy_TCPWM_PWM_Init(PWM_HW, PWM_NUM, &PWM_config))
    {
        /* Handle possible errors */
        CY_ASSERT(0);
    }

    /* Enabling PWM */
    Cy_TCPWM_PWM_Enable(PWM_HW, PWM_NUM);

    /* Initializing TCPWM as PWM */
    if (CY_TCPWM_SUCCESS != Cy_TCPWM_PWM_Init(MASK_PWM_HW, MASK_PWM_NUM, &MASK_PWM_config))
    {
        /* Handle possible errors */
        CY_ASSERT(0);
    }

    /* Enabling PWM */
    Cy_TCPWM_PWM_Enable(MASK_PWM_HW, MASK_PWM_NUM);

    /* Starting PWM */
    Cy_TCPWM_TriggerStart_Single(PWM_HW, PWM_NUM);
    
    /* Starting PWM */
    Cy_TCPWM_TriggerStart_Single(MASK_PWM_HW, MASK_PWM_NUM);

    for (;;)
    {
         Cy_SysLib_Delay(1000);
    }
}
