/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

  /* Private includes ----------------------------------------------------------*/
  /* USER CODE BEGIN Includes */

  /* USER CODE END Includes */

  /* Exported types ------------------------------------------------------------*/
  /* USER CODE BEGIN ET */

  /* USER CODE END ET */

  /* Exported constants --------------------------------------------------------*/
  /* USER CODE BEGIN EC */

  /* USER CODE END EC */

  /* Exported macro ------------------------------------------------------------*/
  /* USER CODE BEGIN EM */

  /* USER CODE END EM */

  /* Exported functions prototypes ---------------------------------------------*/
  void Error_Handler(void);

/* USER CODE BEGIN EFP */
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN Private defines */
#define MF522_MISO_Pin GPIO_PIN_5
#define MF522_MISO_GPIO_Port GPIOA
#define MF522_RST_Pin GPIO_PIN_6
#define MF522_RST_GPIO_Port GPIOA
#define MF522_SCK_Pin GPIO_PIN_7
#define MF522_SCK_GPIO_Port GPIOA
#define MF522_SDA_Pin GPIO_PIN_0
#define MF522_SDA_GPIO_Port GPIOB
#define MF522_MOSI_Pin GPIO_PIN_1
#define MF522_MOSI_GPIO_Port GPIOB
#define key1_Pin GPIO_PIN_12
#define key1_GPIO_Port GPIOB
#define key2_Pin GPIO_PIN_13
#define key2_GPIO_Port GPIOB
#define key3_Pin GPIO_PIN_14
#define key3_GPIO_Port GPIOB
#define key4_Pin GPIO_PIN_15
#define key4_GPIO_Port GPIOB
  /* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
