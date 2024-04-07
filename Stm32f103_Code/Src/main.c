/* USER CODE BEGIN Header */
/*****************************************************************************
  文件: main.c
  作者: Zhengyu httGZ://gzwelink.taobao.com
  版本: V1.0.0
  时间: 20200401
	平台:MINI-STM32F103C8T6

*******************************************************************************/
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "gpio.h"
#include "tim.h"
#include "AS608.h"
#include "RC522.h"
#include "OLED.h"
#include "key.h"
#include "myflash.h"
#include "Servo.h"
#include <stdio.h>
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
SysPara AS608Para; // 指纹模块AS608参数
uint16_t ValidN;   // 模块内有效模板个数

extern uint8_t UID[4], Temp[4];
extern uint8_t UI0[4]; // 卡片0ID数组
extern uint8_t UI1[4]; // 卡片1ID数组
extern uint8_t UI2[4]; // 卡片2ID数组
extern uint8_t UI3[4]; // 卡片3ID数组

uint8_t face_flag = 0; // 人脸识别标志位

static uint8_t menu_page = 0;	// 页面选择
static uint8_t keynum = 0;		// 按键值
static uint8_t menu_select = 1; // 选择项
static uint8_t ID_select = 0;	// ID选择

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void Read_Card_Flash(void); // 上电从flash中读取卡片ID
int press_FR(void);			// 刷指纹
void Add_Card_Flash(void);	// 将cardID 4字节 存入flash,判断ID 0-3
void Del_FR(void);			// 删除指纹

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* Size of Reception buffer */

int main(void)
{
	HAL_Init();
	SystemClock_Config();  // 8M外部晶振，72M主频
	MX_GPIO_Init();		   // 初始化输入引脚
	MX_TIM4_Init();		   // 定时器4初始输出pwm控制舵机
	MX_USART1_UART_Init(); // 串口1配置，PA9-> USART1_TX，PA10-> USART1_RX ，57600波特率，8位数据，1位停止位，无校验
	MX_USART2_UART_Init(); // 与k210通讯串口，串口通讯.
	MX_USART3_UART_Init(); // 调试串口
	printf("Demo1");
	OLED_Init(); // OLED初始化
	OLED_ShowString(1, 1, "  Welcome Home! ");
	OLED_ShowString(2, 1, "   loading...   ");
	RFID_Init();					 // RFID初始化
	Key_Init();						 // 按键初始化
	Servo_Init();					 // 舵机初始化
	while (GZ_HandShake(&AS608Addr)) // 初始化指纹模块
	{
		HAL_Delay(100); // 等待1秒
		printf("finger init again\r\n");
	}
	Read_Card_Flash(); // 读取flash中的卡片ID
	HAL_Delay(1000);
	printf("所有模块已初始化成功\r\n");
	OLED_ShowString(2, 1, "  INIT SUCCESS! ");
	HAL_Delay(1000);

	while (1)
	{
		/* USER CODE END WHILE */
		/* USER CODE BEGIN 3 */
		OLED_Clear();
		// 判断menu_page的值
		switch (menu_page)
		{
		case 0:
			OLED_ShowString(1, 1, "  Please unlock");
			while (1)
			{
				printf("face_flag = %d\r\n", face_flag);
				// 检测face_flag是否为1，为1则解锁成功
				if (face_flag == 1)
				{
					OLED_Clear();
					OLED_ShowString(1, 1, "Unlock success");
					OLED_ShowString(2, 1, "Face unlock");
					OLED_ShowString(3, 1, "success");
					ControlServo();
					face_flag = 0;
					break;
				}
				// 检测按键是否按下
				keynum = Key_Scan();
				if (keynum == 1)
				{
					menu_page = 1;
					break;
				}
				// 检测card是否解锁
				if (PcdRequest(REQ_ALL, Temp) == MI_OK)
				{
					if (PcdAnticoll(UID) == MI_OK)
					{
						if (UID[0] == UI0[0] && UID[1] == UI0[1] && UID[2] == UI0[2] && UID[3] == UI0[3])
						{
							OLED_Clear();
							OLED_ShowString(1, 1, "Unlock success");
							OLED_ShowString(2, 1, "card ID: 0");
							ControlServo();
							menu_page = 0;
							break;
						}
						else if (UID[0] == UI1[0] && UID[1] == UI1[1] && UID[2] == UI1[2] && UID[3] == UI1[3])
						{
							OLED_Clear();
							OLED_ShowString(1, 1, "Unlock success");
							OLED_ShowString(2, 1, "card ID: 1");
							ControlServo();
							menu_page = 0;
							break;
						}
						else if (UID[0] == UI2[0] && UID[1] == UI2[1] && UID[2] == UI2[2] && UID[3] == UI2[3])
						{
							OLED_Clear();
							OLED_ShowString(1, 1, "Unlock success");
							OLED_ShowString(2, 1, "card ID: 2");
							ControlServo();
							menu_page = 0;
							break;
						}
						else if (UID[0] == UI3[0] && UID[1] == UI3[1] && UID[2] == UI3[2] && UID[3] == UI3[3])
						{
							OLED_Clear();
							OLED_ShowString(1, 1, "Unlock success");
							OLED_ShowString(2, 1, "card ID: 3");
							ControlServo();
							menu_page = 0;
							break;
						}
						else
						{
							OLED_Clear();
							OLED_ShowString(1, 1, "Unlock failed");
							OLED_ShowString(2, 1, "xxxxxxxxxxxxx");
							OLED_ShowString(3, 1, "Please  again");
							HAL_Delay(3000);
							menu_page = 0;
							break;
						}
					}
				}
				// 检测指纹解锁
				else
				{
					if (press_FR() >= 0)
					{
						menu_page = 0;
						break;
					}
				}
			}
			break;
		case 1:
			OLED_Clear(); // 清屏
			switch (menu_select)
			{
			case 1:
				OLED_ShowString(1, 1, "-->Add card");
				OLED_ShowString(2, 1, "   Add finger");
				OLED_ShowString(3, 1, "   Delete card");
				OLED_ShowString(4, 1, "   Delete finger");
				break;
			case 2:
				OLED_ShowString(1, 1, "   Add card");
				OLED_ShowString(2, 1, "-->Add finger");
				OLED_ShowString(3, 1, "   Delete card");
				OLED_ShowString(4, 1, "   Delete finger");
				break;
			case 3:
				OLED_ShowString(1, 1, "   Add card");
				OLED_ShowString(2, 1, "   Add finger");
				OLED_ShowString(3, 1, "-->Delete card");
				OLED_ShowString(4, 1, "   Delete finger");
				break;
			case 4:
				OLED_ShowString(1, 1, "   Add card");
				OLED_ShowString(2, 1, "   Add finger");
				OLED_ShowString(3, 1, "   Delete card");
				OLED_ShowString(4, 1, "-->Delete finger");
				break;
			}
			while (1)
			{
				keynum = Key_Scan();
				if (keynum == 1)
				{
					menu_select++;
					if (menu_select > 4)
						menu_select = 1;
					break;
				}

				else if (keynum == 2)
				{
					if (menu_select == 1)
					{
						menu_page = 2;
						break;
					}
					else if (menu_select == 2)
					{
						menu_page = 3;
						break;
					}
					else if (menu_select == 3)
					{
						menu_page = 4;
						break;
					}
					else if (menu_select == 4)
					{
						menu_page = 5;
						break;
					}

					break;
				}
				else if (keynum == 3) // 返回上一级
				{
					menu_page = 0;
					break;
				}
				else if (keynum == 4) // 返回主菜单
				{
					menu_page = 0;
					break;
				}
			}

			break;
		case 2:
			OLED_Clear();
			OLED_ShowString(2, 1, "Please Add Card");
			while (1)
			{
				keynum = Key_Scan();
				if (keynum == 1)
				{
					ID_select++;
					if (ID_select > 3)
						ID_select = 0;
				}
				else if (keynum == 4) // 返回主菜单
				{
					menu_page = 0;
					break;
				}
				else if (keynum == 3) // 返回上一级
				{
					menu_page = 1;
					break;
				}
				if (ID_select == 0)
				{
					OLED_ShowString(3, 1, "   Card 0");
				}
				else if (ID_select == 1)
				{
					OLED_ShowString(3, 1, "   Card 1");
				}
				else if (ID_select == 2)
				{
					OLED_ShowString(3, 1, "   Card 2");
				}
				else if (ID_select == 3)
				{
					OLED_ShowString(3, 1, "   Card 3");
				}
				if (PcdRequest(REQ_ALL, Temp) == MI_OK)
				{
					printf("请求成功\r\n");
					if (PcdAnticoll(UID) == MI_OK)
					{
						// 存储卡片ID
						printf("UID: %x %x %x %x\r\n", UID[0], UID[1], UID[2], UID[3]);
						if (ID_select == 0)
						{
							UI0[0] = UID[0];
							UI0[1] = UID[1];
							UI0[2] = UID[2];
							UI0[3] = UID[3];
						}
						else if (ID_select == 1)
						{
							UI1[0] = UID[0];
							UI1[1] = UID[1];
							UI1[2] = UID[2];
							UI1[3] = UID[3];
						}
						else if (ID_select == 2)
						{
							UI2[0] = UID[0];
							UI2[1] = UID[1];
							UI2[2] = UID[2];
							UI2[3] = UID[3];
						}
						else if (ID_select == 3)
						{
							UI3[0] = UID[0];
							UI3[1] = UID[1];
							UI3[2] = UID[2];
							UI3[3] = UID[3];
						}
						Add_Card_Flash();
						OLED_Clear();
						OLED_ShowString(2, 1, "   Add card");
						OLED_ShowString(3, 1, "   success");
						HAL_Delay(3000);
						menu_page = 1;
						ID_select = 0;
						break;
					}
				}
			}

			break;
		case 3:
			OLED_Clear();
			OLED_ShowString(2, 1, "  please finger1");
			while (1)
			{
				uint8_t ensure = 0;
				keynum = Key_Scan();
				if (keynum == 1)
				{
					ID_select++;
					if (ID_select > 3)
						ID_select = 0;
				}
				else if (keynum == 2)
				{
					menu_page = 1;
				}
				else if (keynum == 3) // 返回主菜单
				{
					menu_page = 1;
					break;
				}
				else if (keynum == 4) // 返回上一级
				{
					menu_page = 0;
					break;
				}
				if (ID_select == 0)
				{
					OLED_ShowString(3, 1, "   Finger 0");
				}
				else if (ID_select == 1)
				{
					OLED_ShowString(3, 1, "   Finger 1");
				}
				else if (ID_select == 2)
				{
					OLED_ShowString(3, 1, "   Finger 2");
				}
				else if (ID_select == 3)
				{
					OLED_ShowString(3, 1, "   Finger 3");
				}
				// 录入指纹
				ensure = GZ_GetImage();
				if (ensure == 0x00)
				{
					ensure = GZ_GenChar(CharBuffer1); // 生成特征
					if (ensure == 0x00)
					{
						ensure = GZ_GetImage();
						if (ensure == 0x00)
						{
							ensure = GZ_GenChar(CharBuffer2); // 生成特征
							if (ensure == 0x00)
							{
								ensure = GZ_Match();
								if (ensure == 0x00)
								{
									ensure = GZ_RegModel();
									if (ensure == 0x00)
									{
										ensure = GZ_StoreChar(CharBuffer2, ID_select); // 储存模板
										if (ensure == 0x00)
										{
											printf("录入成功");
											GZ_ValidTempleteNum(&ValidN); // 读库指纹个数
											printf("指纹库中有%d个指纹", ValidN);
											OLED_Clear();
											OLED_ShowString(2, 1, "  Add finger");
											OLED_ShowString(3, 1, "   success");
											HAL_Delay(3000); //
											menu_page = 1;
											ID_select = 0;
											break;
										}
									}
								}
							}
						}
					}
				}
			}
			break;
		case 4:
			OLED_Clear();
			OLED_ShowString(2, 1, "  Delete card1");
			while (1)
			{
				keynum = Key_Scan();
				if (keynum == 1)
				{
					ID_select++;
					if (ID_select > 3)
						ID_select = 0;
				}
				else if (keynum == 2)
				{
					// 删除卡片
					UID[0] = 0;
					UID[1] = 0;
					UID[2] = 0;
					UID[3] = 0;
					if (ID_select == 0)
					{
						UI0[0] = UID[0];
						UI0[1] = UID[1];
						UI0[2] = UID[2];
						UI0[3] = UID[3];
					}
					else if (ID_select == 1)
					{
						UI1[0] = UID[0];
						UI1[1] = UID[1];
						UI1[2] = UID[2];
						UI1[3] = UID[3];
					}
					else if (ID_select == 2)
					{
						UI2[0] = UID[0];
						UI2[1] = UID[1];
						UI2[2] = UID[2];
						UI2[3] = UID[3];
					}
					else if (ID_select == 3)
					{
						UI3[0] = UID[0];
						UI3[1] = UID[1];
						UI3[2] = UID[2];
						UI3[3] = UID[3];
					}
					Add_Card_Flash();
					printf("删除卡片成功\r\n");
					OLED_Clear();
					OLED_ShowString(2, 1, "  Delete card");
					OLED_ShowString(3, 1, "   success");
					HAL_Delay(3000);
					menu_page = 1;
					ID_select = 0;
					break;
				}
				else if (keynum == 3) // 返回主菜单
				{
					menu_page = 1;
					break;
				}
				else if (keynum == 4) // 返回上一级
				{
					menu_page = 0;
					break;
				}
				if (ID_select == 0)
				{
					OLED_ShowString(3, 1, "   Card 0");
				}
				else if (ID_select == 1)
				{
					OLED_ShowString(3, 1, "   Card 1");
				}
				else if (ID_select == 2)
				{
					OLED_ShowString(3, 1, "   Card 2");
				}
				else if (ID_select == 3)
				{
					OLED_ShowString(3, 1, "   Card 3");
				}
			}
			break;
		case 5:
			OLED_Clear();
			OLED_ShowString(2, 1, "  Delete finger1");
			while (1)
			{
				keynum = Key_Scan();
				if (keynum == 1)
				{
					ID_select++;
					if (ID_select > 3)
						ID_select = 0;
				}
				else if (keynum == 2)
				{
					// 删除指纹
					Del_FR();
					printf("删除指纹成功\r\n");
					OLED_Clear();
					OLED_ShowString(2, 1, "  Delete Finger");
					OLED_ShowString(3, 1, "   success");
					HAL_Delay(3000);
					menu_page = 1;
					ID_select = 0;
					break;
				}
				else if (keynum == 3) // 返回主菜单
				{
					menu_page = 1;
					break;
				}
				else if (keynum == 4) // 返回上一级
				{
					menu_page = 0;
					break;
				}
				if (ID_select == 0)
				{
					OLED_ShowString(3, 1, "   Finger 0");
				}
				else if (ID_select == 1)
				{
					OLED_ShowString(3, 1, "   Finger 1");
				}
				else if (ID_select == 2)
				{
					OLED_ShowString(3, 1, "   Finger 2");
				}
				else if (ID_select == 3)
				{
					OLED_ShowString(3, 1, "   Finger 3");
				}
			}
			break;
		}
	}
}

/* USER CODE END 3 */

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
	{
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */

	/* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
	   tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

// 上电从flash中读取卡片ID
void Read_Card_Flash()
{
	uint16_t cardID[2];
	stmflash_read(0X08009000, cardID, 2);
	UI0[0] = cardID[0] >> 8;
	UI0[1] = cardID[0];
	UI0[2] = cardID[1] >> 8;
	UI0[3] = cardID[1];
	stmflash_read(0X08009004, cardID, 2);
	UI1[0] = cardID[0] >> 8;
	UI1[1] = cardID[0];
	UI1[2] = cardID[1] >> 8;
	UI1[3] = cardID[1];
	stmflash_read(0X08009008, cardID, 2);
	UI2[0] = cardID[0] >> 8;
	UI2[1] = cardID[0];
	UI2[2] = cardID[1] >> 8;
	UI2[3] = cardID[1];
	stmflash_read(0X0800900C, cardID, 2);
	UI3[0] = cardID[0] >> 8;
	UI3[1] = cardID[0];
	UI3[2] = cardID[1] >> 8;
	UI3[3] = cardID[1];
}

// 刷指纹
int press_FR(void)
{
	SearchResult seach;
	uint8_t ensure;
	char *str;

	ensure = GZ_GetImage();
	if (ensure == 0x00) // 获取图像成功
	{
		ensure = GZ_GenChar(CharBuffer1);
		if (ensure == 0x00) // 生成特征成功
		{
			ensure = GZ_HighSpeedSearch(CharBuffer1, 0, 300, &seach);
			if (ensure == 0x00) // 搜索成功
			{
				if (seach.mathscore > 100)
				{
					// 拼接Finger ID: 与 seach.pageID，并显示在OLED上
					// 拼接字符串存储与str中， 在oled上显示
					str = "Finger ID:";
					printf("匹配");
					OLED_Clear();
					OLED_ShowString(1, 1, "Unlock success");
					OLED_ShowString(2, 1, str);
					OLED_ShowNum(2, 12, seach.pageID, 1);
					ControlServo();
					return 0;
				}
				else
				{
					printf("错误1");
					OLED_Clear();
					OLED_ShowString(1, 1, "Unlock failed");
					HAL_Delay(3000);
					return 1;
				}
			}
			else
			{
				printf("错误2");
				OLED_Clear();
				OLED_ShowString(1, 1, "Unlock failed");
				HAL_Delay(3000);
				return 2;
			}
		}
	}
	return -1;
}

// 删除指纹
void Del_FR(void)
{
	// GZ_Empty();//清空指纹库
	GZ_DeletChar(ID_select, 1); // 删除单个指纹
}

// 将cardID 4字节 存入flash,判断ID 0-3
void Add_Card_Flash()
{
	uint16_t cardID[2];
	cardID[0] = UID[0] << 8 | UID[1];
	cardID[1] = UID[2] << 8 | UID[3];
	if (ID_select == 0)
	{
		stmflash_write(0X08009000, cardID, 2);
	}
	else if (ID_select == 1)
	{
		stmflash_write(0X08009004, cardID, 2);
	}
	else if (ID_select == 2)
	{
		stmflash_write(0X08009008, cardID, 2);
	}
	else if (ID_select == 3)
	{
		stmflash_write(0X0800900C, cardID, 2);
	}
}

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
