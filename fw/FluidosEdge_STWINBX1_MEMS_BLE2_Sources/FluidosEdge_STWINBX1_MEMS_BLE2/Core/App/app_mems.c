/**
  ******************************************************************************
  * File Name          : app_mems.c
  * Description        : This file provides code for the configuration
  *                      of the STMicroelectronics.X-CUBE-MEMS1.10.0.0 instances.
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

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "app_mems.h"
#include "main.h"
#include <stdio.h>

#include "custom_env_sensors.h"
#include "custom_env_sensors_ex.h"
#include "ilps22qs_settings.h"
#include "steval_stwinbx1.h"
#include "math.h"

/* Private typedef -----------------------------------------------------------*/
typedef struct displayFloatToInt_s {
  int8_t sign; /* 0 means positive, 1 means negative*/
  uint32_t  out_int;
  uint32_t  out_dec;
} displayFloatToInt_t;

/* Private define ------------------------------------------------------------*/
#define MAX_BUF_SIZE 256

#define SUPPORTED "Supported"
#define NOT_SUPPORTED "Not Supported"

/* Private variables ---------------------------------------------------------*/
static uint8_t verbose = 1;  /* Verbose output to UART terminal ON/OFF. */
static CUSTOM_ENV_SENSOR_Capabilities_t EnvCapabilities[CUSTOM_ENV_INSTANCES_NBR];
static char dataOut[MAX_BUF_SIZE];
static char isSupported_Value[14] = {0};
//static int32_t PushButtonState = GPIO_PIN_RESET;

/* Private function prototypes -----------------------------------------------*/
static void floatToInt(float in, displayFloatToInt_t *out_value, int32_t dec_prec);
static void Env_Temp_Sensor_Handler(uint32_t Instance);
static void Env_Hum_Sensor_Handler(uint32_t Instance);
static void Env_Press_Sensor_Handler(uint32_t Instance);
static void Env_Gas_Sensor_Handler(uint32_t Instance);
static void MX_MEMS_Sensors_Init(void);
static void MX_MEMS_Sensors_Process(void);
static void isSupported(uint8_t status, char *isSupportedString);



void MX_MEMS_Init(void)
{
  /* USER CODE BEGIN SV */

  /* USER CODE END SV */

  /* USER CODE BEGIN MEMS_Init_PreTreatment */

  /* USER CODE END MEMS_Init_PreTreatment */

  /* Initialize the peripherals and the MEMS components */

  MX_MEMS_Sensors_Init();

  /* USER CODE BEGIN MEMS_Init_PostTreatment */

  /* USER CODE END MEMS_Init_PostTreatment */
}

/*
 * LM background task
 */
void MX_MEMS_Process(void)
{
  /* USER CODE BEGIN MEMS_Process_PreTreatment */

  /* USER CODE END MEMS_Process_PreTreatment */

	MX_MEMS_Sensors_Process();

  /* USER CODE BEGIN MEMS_Process_PostTreatment */

  /* USER CODE END MEMS_Process_PostTreatment */
}

/**
  * @brief  Initialize the DataLogTerminal application
  * @retval None
  */
void MX_MEMS_Sensors_Init(void)
{
  displayFloatToInt_t out_value_odr;
  int i;

  /* Initialize LED */
  BSP_LED_Init(LED2);

  CUSTOM_ENV_SENSOR_Init(CUSTOM_ILPS22QS_0, ENV_TEMPERATURE | ENV_PRESSURE);

  CUSTOM_ENV_SENSOR_SetOutputDataRate(CUSTOM_ILPS22QS_0, ENV_TEMPERATURE, ILPS22QS_ODR);

  CUSTOM_ENV_SENSOR_SetOutputDataRate(CUSTOM_ILPS22QS_0, ENV_PRESSURE, ILPS22QS_ODR);

  for(i = 0; i < CUSTOM_ENV_INSTANCES_NBR; i++)
  {
    CUSTOM_ENV_SENSOR_GetCapabilities(i, &EnvCapabilities[i]);

    printf("ILPS22QS Environmental Sensor capabilities:\r\n");

    isSupported(EnvCapabilities[i].Temperature, isSupported_Value);
    floatToInt(EnvCapabilities[i].TempMaxOdr, &out_value_odr, 3);
    printf("Temperature: %s (ODR: %d.%02d Hz)\r\n", isSupported_Value, (int)out_value_odr.out_int, (int)out_value_odr.out_dec);

    isSupported(EnvCapabilities[i].Pressure, isSupported_Value);
    floatToInt(EnvCapabilities[i].PressMaxOdr, &out_value_odr, 3);
    printf("Pressure:    %s (ODR: %d.%02d Hz)\r\n", isSupported_Value, (int)out_value_odr.out_int, (int)out_value_odr.out_dec);

    isSupported(EnvCapabilities[i].Humidity, isSupported_Value);
    floatToInt(EnvCapabilities[i].HumMaxOdr, &out_value_odr, 3);
    printf("Humidity:    %s (ODR: %d.%02d Hz)\r\n", isSupported_Value, (int)out_value_odr.out_int, (int)out_value_odr.out_dec);

    isSupported(EnvCapabilities[i].Gas, isSupported_Value);
    floatToInt(EnvCapabilities[i].GasMaxOdr, &out_value_odr, 3);
    printf("Gas:         %s (ODR: %d.%02d Hz)\r\n", isSupported_Value, (int)out_value_odr.out_int, (int)out_value_odr.out_dec);

  }

  printf("Sensor activation, please wait...\r\n\r\n");
  HAL_Delay(5000);
}


/**
  * @brief  Process of the DataLogTerminal application
  * @retval None
  */
void MX_MEMS_Sensors_Process(void)
{
  int i;

//  if (PushButtonDetected != 0U)
//  {
//    /* Debouncing */
//    HAL_Delay(50);
//
//    /* Wait until the button is released */
//    while ((BSP_PB_GetState( BUTTON_KEY ) == PushButtonState));
//
//    /* Debouncing */
//    HAL_Delay(50);
//
//    /* Reset Interrupt flag */
//    PushButtonDetected = 0;
//
//    MX_DataLogTerminal_Init();
//  }

  for (i = 0; i < CUSTOM_ENV_INSTANCES_NBR; i++)
  {
    if (EnvCapabilities[i].Humidity)
    {
      Env_Hum_Sensor_Handler(i);
    }
    if (EnvCapabilities[i].Temperature)
    {
      Env_Temp_Sensor_Handler(i);
    }
    if (EnvCapabilities[i].Pressure)
    {
      Env_Press_Sensor_Handler(i);
    }
    if (EnvCapabilities[i].Gas)
    {
      Env_Gas_Sensor_Handler(i);
    }
  }

  HAL_Delay( 1000 );
}

void Get_MEMS_Data(uint32_t Instance, EnvData_t * env_data)
{
	  if(CUSTOM_ENV_SENSOR_GetValue(Instance, ENV_TEMPERATURE, &(env_data->Temperature)))
	  {
	    printf("\r\nTemperature[%d]: Error\r\n", (int)Instance);
	  }

	  if(CUSTOM_ENV_SENSOR_GetValue(Instance, ENV_PRESSURE, &(env_data->Pressure)))
	  {
	    printf("\r\Pressure[%d]: Error\r\n", (int)Instance);
	  }

}

static void isSupported(uint8_t status, char *isSupportedString)
{
	memset(isSupported, 0, sizeof(isSupportedString));

	if(status == 0)
	{
		memcpy(isSupportedString, NOT_SUPPORTED, sizeof(NOT_SUPPORTED));
	}
	else
	{
		memcpy(isSupportedString, SUPPORTED, sizeof(SUPPORTED));
	}
}

/**
  * @brief  Splits a float into two integer values.
  * @param  in the float value as input
  * @param  out_value the pointer to the output integer structure
  * @param  dec_prec the decimal precision to be used
  * @retval None
  */
static void floatToInt(float in, displayFloatToInt_t *out_value, int32_t dec_prec)
{
  if(in >= 0.0f)
  {
    out_value->sign = 0;
  }else
  {
    out_value->sign = 1;
    in = -in;
  }

  in = in + (0.5f / pow(10, dec_prec));
  out_value->out_int = (int32_t)in;
  in = in - (float)(out_value->out_int);
  out_value->out_dec = (int32_t)trunc(in * pow(10, dec_prec));
}

/**
  * @brief  Handles the temperature data getting/sending
  * @param  Instance the device instance
  * @retval None
  */
static void Env_Temp_Sensor_Handler(uint32_t Instance)
{
  float odr;
  float temperature;
  displayFloatToInt_t out_value;
  uint8_t whoami;

  snprintf(dataOut, MAX_BUF_SIZE, "\r\nEnvironmental sensor instance %d:", (int)Instance);
  printf("%s", dataOut);

  if (CUSTOM_ENV_SENSOR_GetValue(Instance, ENV_TEMPERATURE, &temperature))
  {
    snprintf(dataOut, MAX_BUF_SIZE, "\r\nTemp[%d]: Error\r\n", (int)Instance);
  }
  else
  {
    floatToInt(temperature, &out_value, 2);
    snprintf(dataOut, MAX_BUF_SIZE, "\r\nTemp[%d]: %c%d.%02d degC\r\n", (int)Instance, ((out_value.sign) ? '-' : '+'), (int)out_value.out_int,
             (int)out_value.out_dec);
  }

  printf("%s", dataOut);

  if (verbose == 1)
  {
    if (CUSTOM_ENV_SENSOR_ReadID(Instance, &whoami))
    {
      snprintf(dataOut, MAX_BUF_SIZE, "WhoAmI[%d]: Error\r\n", (int)Instance);
    }
    else
    {
      snprintf(dataOut, MAX_BUF_SIZE, "WhoAmI[%d]: 0x%x\r\n", (int)Instance, (int)whoami);
    }

    printf("%s", dataOut);

    if (CUSTOM_ENV_SENSOR_GetOutputDataRate(Instance, ENV_TEMPERATURE, &odr))
    {
      snprintf(dataOut, MAX_BUF_SIZE, "ODR[%d]: Error\r\n", (int)Instance);
    }
    else
    {
      floatToInt(odr, &out_value, 3);
      snprintf(dataOut, MAX_BUF_SIZE, "ODR[%d]: %d.%03d Hz\r\n", (int)Instance, (int)out_value.out_int,
               (int)out_value.out_dec);
    }

    printf("%s", dataOut);
  }
}

/**
  * @brief  Handles the pressure sensor data getting/sending
  * @param  Instance the device instance
  * @retval None
  */
static void Env_Press_Sensor_Handler(uint32_t Instance)
{
  float odr;
  float pressure;
  displayFloatToInt_t out_value;
  uint8_t whoami;

  snprintf(dataOut, MAX_BUF_SIZE, "\r\nEnvironmental sensor instance %d:", (int)Instance);
  printf("%s", dataOut);

  if (CUSTOM_ENV_SENSOR_GetValue(Instance, ENV_PRESSURE, &pressure))
  {
    snprintf(dataOut, MAX_BUF_SIZE, "\r\nPress[%d]: Error\r\n", (int)Instance);
  }
  else
  {
    floatToInt(pressure, &out_value, 2);
    snprintf(dataOut, MAX_BUF_SIZE, "\r\nPress[%d]: %d.%02d hPa\r\n", (int)Instance, (int)out_value.out_int,
             (int)out_value.out_dec);
  }

  printf("%s", dataOut);

  if (verbose == 1)
  {
    if (CUSTOM_ENV_SENSOR_ReadID(Instance, &whoami))
    {
      snprintf(dataOut, MAX_BUF_SIZE, "WhoAmI[%d]: Error\r\n", (int)Instance);
    }
    else
    {
      snprintf(dataOut, MAX_BUF_SIZE, "WhoAmI[%d]: 0x%x\r\n", (int)Instance, (int)whoami);
    }

    printf("%s", dataOut);

    if (CUSTOM_ENV_SENSOR_GetOutputDataRate(Instance, ENV_PRESSURE, &odr))
    {
      snprintf(dataOut, MAX_BUF_SIZE, "ODR[%d]: Error\r\n", (int)Instance);
    }
    else
    {
      floatToInt(odr, &out_value, 3);
      snprintf(dataOut, MAX_BUF_SIZE, "ODR[%d]: %d.%03d Hz\r\n", (int)Instance, (int)out_value.out_int,
               (int)out_value.out_dec);
    }

    printf("%s", dataOut);
  }
}

/**
  * @brief  Handles the humidity data getting/sending
  * @param  Instance the device instance
  * @retval None
  */
static void Env_Hum_Sensor_Handler(uint32_t Instance)
{
  float odr;
  float humidity;
  displayFloatToInt_t out_value;
  uint8_t whoami;

  snprintf(dataOut, MAX_BUF_SIZE, "\r\nEnvironmental sensor instance %d:", (int)Instance);
  printf("%s", dataOut);

  if (CUSTOM_ENV_SENSOR_GetValue(Instance, ENV_HUMIDITY, &humidity))
  {
    snprintf(dataOut, MAX_BUF_SIZE, "\r\nHum[%d]: Error\r\n", (int)Instance);
  }
  else
  {
    floatToInt(humidity, &out_value, 2);
    snprintf(dataOut, MAX_BUF_SIZE, "\r\nHum[%d]: %d.%02d %%\r\n", (int)Instance, (int)out_value.out_int,
             (int)out_value.out_dec);
  }

  printf("%s", dataOut);

  if (verbose == 1)
  {
    if (CUSTOM_ENV_SENSOR_ReadID(Instance, &whoami))
    {
      snprintf(dataOut, MAX_BUF_SIZE, "WhoAmI[%d]: Error\r\n", (int)Instance);
    }
    else
    {
      snprintf(dataOut, MAX_BUF_SIZE, "WhoAmI[%d]: 0x%x\r\n", (int)Instance, (int)whoami);
    }

    printf("%s", dataOut);

    if (CUSTOM_ENV_SENSOR_GetOutputDataRate(Instance, ENV_HUMIDITY, &odr))
    {
      snprintf(dataOut, MAX_BUF_SIZE, "ODR[%d]: Error\r\n", (int)Instance);
    }
    else
    {
      floatToInt(odr, &out_value, 3);
      snprintf(dataOut, MAX_BUF_SIZE, "ODR[%d]: %d.%03d Hz\r\n", (int)Instance, (int)out_value.out_int,
               (int)out_value.out_dec);
    }

    printf("%s", dataOut);
  }
}

/**
  * @brief  Handles the gas sensor data getting/sending
  * @param  Instance the device instance
  * @retval None
  */
static void Env_Gas_Sensor_Handler(uint32_t Instance)
{
  float odr;
  float gas;
  displayFloatToInt_t out_value;
  uint8_t whoami;

  snprintf(dataOut, MAX_BUF_SIZE, "\r\nEnvironmental sensor instance %d:", (int)Instance);
  printf("%s", dataOut);

  if (CUSTOM_ENV_SENSOR_GetValue(Instance, ENV_GAS, &gas))
  {
    snprintf(dataOut, MAX_BUF_SIZE, "\r\nGas[%d]: Error\r\n", (int)Instance);
  }
  else
  {
    floatToInt(gas, &out_value, 2);
    snprintf(dataOut, MAX_BUF_SIZE, "\r\nGas[%d]: %d.%02d\r\n", (int)Instance, (int)out_value.out_int,
            (int)out_value.out_dec);
  }

  printf("%s", dataOut);

  if (verbose == 1)
  {
    if (CUSTOM_ENV_SENSOR_ReadID(Instance, &whoami))
    {
      snprintf(dataOut, MAX_BUF_SIZE, "WhoAmI[%d]: Error\r\n", (int)Instance);
    }
    else
    {
      snprintf(dataOut, MAX_BUF_SIZE, "WhoAmI[%d]: 0x%x\r\n", (int)Instance, (int)whoami);
    }

    printf("%s", dataOut);

    if (CUSTOM_ENV_SENSOR_GetOutputDataRate(Instance, ENV_GAS, &odr))
    {
      snprintf(dataOut, MAX_BUF_SIZE, "ODR[%d]: Error\r\n", (int)Instance);
    }
    else
    {
      floatToInt(odr, &out_value, 3);
      snprintf(dataOut, MAX_BUF_SIZE, "ODR[%d]: %d.%03d Hz\r\n", (int)Instance, (int)out_value.out_int,
               (int)out_value.out_dec);
    }

    printf("%s", dataOut);
  }
}

#ifdef __cplusplus
}
#endif
