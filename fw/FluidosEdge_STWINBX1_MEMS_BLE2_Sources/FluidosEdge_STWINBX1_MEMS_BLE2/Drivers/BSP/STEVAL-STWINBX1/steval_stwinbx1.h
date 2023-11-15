/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file  : steval_stwinbx1.h
  * @brief : header file for the BSP Common driver
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
*/
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STEVAL_STWINBX1_H
#define __STEVAL_STWINBX1_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "steval_stwinbx1_conf.h"
#include "steval_stwinbx1_errno.h"
#include "main.h"

/** @addtogroup BSP
 * @{
 */

/** @defgroup STEVAL_STWINBX1
 * @{
 */

/** @defgroup STEVAL_STWINBX1_LOW_LEVEL
 * @{
 */

/** @defgroup STM32L4XX_NUCLEO_LOW_LEVEL_Exported_Constants LOW LEVEL Exported Constants
  * @{
  */
/**
 * @brief STM32U5XX NUCLEO BSP Driver version number V1.0.0
 */
#define __STEVAL_STWINBX1_BSP_VERSION_MAIN   (uint32_t)(0x01) /*!< [31:24] main version */
#define __STEVAL_STWINBX1_BSP_VERSION_SUB1   (uint32_t)(0x00) /*!< [23:16] sub1 version */
#define __STEVAL_STWINBX1_BSP_VERSION_SUB2   (uint32_t)(0x00) /*!< [15:8]  sub2 version */
#define __STEVAL_STWINBX1_BSP_VERSION_RC     (uint32_t)(0x00) /*!< [7:0]  release candidate */
#define __STEVAL_STWINBX1_BSP_VERSION        ((__STEVAL_STWINBX1_BSP_VERSION_MAIN << 24)\
                                                    |(__STEVAL_STWINBX1_BSP_VERSION_SUB1 << 16)\
                                                    |(__STEVAL_STWINBX1_BSP_VERSION_SUB2 << 8 )\
                                                    |(__STEVAL_STWINBX1_BSP_VERSION_RC))

/** @defgroup STEVAL_STWINBX1_LOW_LEVEL_Exported_Types STEVAL_STWINBX1 LOW LEVEL Exported Types
 * @{
 */

 /**
  * @brief Define for STEVAL_STWINBX1 board
  */
#if !defined (USE_STEVAL_STWINBX1)
 #define USE_STEVAL_STWINBX1
#endif

/** @defgroup STEVAL_STWINBX1_LOW_LEVEL_LED STEVAL_STWINBX1 LOW LEVEL LED
 * @{
 */
/** Define number of LED            **/
#define LEDn                              1U
/**  Definition for BSP USER LED 2   **/
#define LED2_PIN                     	  GPIO_PIN_12
#define LED2_GPIO_PORT                    GPIOH
#define LED2_GPIO_CLK_ENABLE()            __HAL_RCC_GPIOH_CLK_ENABLE()
#define LED2_GPIO_CLK_DISABLE()           __HAL_RCC_GPIOH_CLK_DISABLE()

/**
 * @}
 */

/** @defgroup STEVAL_STWINBX1_LOW_LEVEL_BUTTON STEVAL_STWINBX1 LOW LEVEL BUTTON
 * @{
 */
/* Button state */
#define BUTTON_RELEASED                   0U
#define BUTTON_PRESSED                    1U
/** Define number of BUTTON            **/
#define BUTTONn                           1U

/**
 * @brief User push-button
 */
  /**  Definition for BSP USER BUTTON   **/

#define USER_BUTTON_PIN	                  GPIO_PIN_0
#define USER_BUTTON_GPIO_PORT              GPIOE
#define USER_BUTTON_EXTI_IRQn              EXTI0_IRQn
#define USER_BUTTON_EXTI_LINE              EXTI_LINE_0
#define H_EXTI_0			  hpb_exti[BUTTON_USER]

/**
 * @}
 */

/** @defgroup STEVAL_STWINBX1_LOW_LEVEL_Exported_Types LOW LEVEL Exported Types
  * @{
  */

#ifndef BSP_BUTTON_USER_IT_PRIORITY
  #define BSP_BUTTON_USER_IT_PRIORITY            15U
#endif

typedef enum
{
  LED2 = 0,
  LED_GREEN = LED2,
}Led_TypeDef;

typedef enum
{
  BUTTON_USER = 0U,
}Button_TypeDef;

/* Keep compatibility with CMSIS Pack already delivered */
#define BUTTON_KEY BUTTON_USER

typedef enum
{
  BUTTON_MODE_GPIO = 0,
  BUTTON_MODE_EXTI = 1
} ButtonMode_TypeDef;
/**
 * @}
 */

/**
 * @}
 */

/**
  * @}
  */

/**
  * @}
  */

/** @defgroup STEVAL_STWINBX1_LOW_LEVEL_Exported_Variables LOW LEVEL Exported Constants
  * @{
  */
extern EXTI_HandleTypeDef hpb_exti[BUTTONn];
/**
  * @}
  */

/** @defgroup STEVAL_STWINBX1_LOW_LEVEL_Exported_Functions STEVAL_STWINBX1 LOW LEVEL Exported Functions
 * @{
 */

int32_t  BSP_GetVersion(void);
int32_t  BSP_LED_Init(Led_TypeDef Led);
int32_t  BSP_LED_DeInit(Led_TypeDef Led);
int32_t  BSP_LED_On(Led_TypeDef Led);
int32_t  BSP_LED_Off(Led_TypeDef Led);
int32_t  BSP_LED_Toggle(Led_TypeDef Led);
int32_t  BSP_LED_GetState(Led_TypeDef Led);
int32_t  BSP_PB_Init(Button_TypeDef Button, ButtonMode_TypeDef ButtonMode);
int32_t  BSP_PB_DeInit(Button_TypeDef Button);
int32_t  BSP_PB_GetState(Button_TypeDef Button);
void     BSP_PB_Callback(Button_TypeDef Button);
void     BSP_PB_IRQHandler (Button_TypeDef Button);

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */
#ifdef __cplusplus
}
#endif

#endif /* __STEVAL_STWINBX1__H */

