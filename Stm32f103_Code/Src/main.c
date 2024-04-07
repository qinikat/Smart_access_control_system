/* USER CODE BEGIN Header */
/*****************************************************************************
  �ļ�: main.c
  ����: Zhengyu httGZ://gzwelink.taobao.com
  �汾: V1.0.0
  ʱ��: 20200401
	ƽ̨:MINI-STM32F103C8T6

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
SysPara AS608Para; // ָ��ģ��AS608����
uint16_t ValidN;   // ģ������Чģ�����

extern uint8_t UID[4], Temp[4];
extern uint8_t UI0[4]; // ��Ƭ0ID����
extern uint8_t UI1[4]; // ��Ƭ1ID����
extern uint8_t UI2[4]; // ��Ƭ2ID����
extern uint8_t UI3[4]; // ��Ƭ3ID����

uint8_t face_flag = 0; // ����ʶ���־λ

static uint8_t menu_page = 0;	// ҳ��ѡ��
static uint8_t keynum = 0;		// ����ֵ
static uint8_t menu_select = 1; // ѡ����
static uint8_t ID_select = 0;	// IDѡ��

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void Read_Card_Flash(void); // �ϵ��flash�ж�ȡ��ƬID
int press_FR(void);			// ˢָ��
void Add_Card_Flash(void);	// ��cardID 4�ֽ� ����flash,�ж�ID 0-3
void Del_FR(void);			// ɾ��ָ��

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* Size of Reception buffer */

int main(void)
{
	HAL_Init();
	SystemClock_Config();  // 8M�ⲿ����72M��Ƶ
	MX_GPIO_Init();		   // ��ʼ����������
	MX_TIM4_Init();		   // ��ʱ��4��ʼ���pwm���ƶ��
	MX_USART1_UART_Init(); // ����1���ã�PA9-> USART1_TX��PA10-> USART1_RX ��57600�����ʣ�8λ���ݣ�1λֹͣλ����У��
	MX_USART2_UART_Init(); // ��k210ͨѶ���ڣ�����ͨѶ.
	MX_USART3_UART_Init(); // ���Դ���
	printf("Demo1");
	OLED_Init(); // OLED��ʼ��
	OLED_ShowString(1, 1, "  Welcome Home! ");
	OLED_ShowString(2, 1, "   loading...   ");
	RFID_Init();					 // RFID��ʼ��
	Key_Init();						 // ������ʼ��
	Servo_Init();					 // �����ʼ��
	while (GZ_HandShake(&AS608Addr)) // ��ʼ��ָ��ģ��
	{
		HAL_Delay(100); // �ȴ�1��
		printf("finger init again\r\n");
	}
	Read_Card_Flash(); // ��ȡflash�еĿ�ƬID
	HAL_Delay(1000);
	printf("����ģ���ѳ�ʼ���ɹ�\r\n");
	OLED_ShowString(2, 1, "  INIT SUCCESS! ");
	HAL_Delay(1000);

	while (1)
	{
		/* USER CODE END WHILE */
		/* USER CODE BEGIN 3 */
		OLED_Clear();
		// �ж�menu_page��ֵ
		switch (menu_page)
		{
		case 0:
			OLED_ShowString(1, 1, "  Please unlock");
			while (1)
			{
				printf("face_flag = %d\r\n", face_flag);
				// ���face_flag�Ƿ�Ϊ1��Ϊ1������ɹ�
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
				// ��ⰴ���Ƿ���
				keynum = Key_Scan();
				if (keynum == 1)
				{
					menu_page = 1;
					break;
				}
				// ���card�Ƿ����
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
				// ���ָ�ƽ���
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
			OLED_Clear(); // ����
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
				else if (keynum == 3) // ������һ��
				{
					menu_page = 0;
					break;
				}
				else if (keynum == 4) // �������˵�
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
				else if (keynum == 4) // �������˵�
				{
					menu_page = 0;
					break;
				}
				else if (keynum == 3) // ������һ��
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
					printf("����ɹ�\r\n");
					if (PcdAnticoll(UID) == MI_OK)
					{
						// �洢��ƬID
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
				else if (keynum == 3) // �������˵�
				{
					menu_page = 1;
					break;
				}
				else if (keynum == 4) // ������һ��
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
				// ¼��ָ��
				ensure = GZ_GetImage();
				if (ensure == 0x00)
				{
					ensure = GZ_GenChar(CharBuffer1); // ��������
					if (ensure == 0x00)
					{
						ensure = GZ_GetImage();
						if (ensure == 0x00)
						{
							ensure = GZ_GenChar(CharBuffer2); // ��������
							if (ensure == 0x00)
							{
								ensure = GZ_Match();
								if (ensure == 0x00)
								{
									ensure = GZ_RegModel();
									if (ensure == 0x00)
									{
										ensure = GZ_StoreChar(CharBuffer2, ID_select); // ����ģ��
										if (ensure == 0x00)
										{
											printf("¼��ɹ�");
											GZ_ValidTempleteNum(&ValidN); // ����ָ�Ƹ���
											printf("ָ�ƿ�����%d��ָ��", ValidN);
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
					// ɾ����Ƭ
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
					printf("ɾ����Ƭ�ɹ�\r\n");
					OLED_Clear();
					OLED_ShowString(2, 1, "  Delete card");
					OLED_ShowString(3, 1, "   success");
					HAL_Delay(3000);
					menu_page = 1;
					ID_select = 0;
					break;
				}
				else if (keynum == 3) // �������˵�
				{
					menu_page = 1;
					break;
				}
				else if (keynum == 4) // ������һ��
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
					// ɾ��ָ��
					Del_FR();
					printf("ɾ��ָ�Ƴɹ�\r\n");
					OLED_Clear();
					OLED_ShowString(2, 1, "  Delete Finger");
					OLED_ShowString(3, 1, "   success");
					HAL_Delay(3000);
					menu_page = 1;
					ID_select = 0;
					break;
				}
				else if (keynum == 3) // �������˵�
				{
					menu_page = 1;
					break;
				}
				else if (keynum == 4) // ������һ��
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

// �ϵ��flash�ж�ȡ��ƬID
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

// ˢָ��
int press_FR(void)
{
	SearchResult seach;
	uint8_t ensure;
	char *str;

	ensure = GZ_GetImage();
	if (ensure == 0x00) // ��ȡͼ��ɹ�
	{
		ensure = GZ_GenChar(CharBuffer1);
		if (ensure == 0x00) // ���������ɹ�
		{
			ensure = GZ_HighSpeedSearch(CharBuffer1, 0, 300, &seach);
			if (ensure == 0x00) // �����ɹ�
			{
				if (seach.mathscore > 100)
				{
					// ƴ��Finger ID: �� seach.pageID������ʾ��OLED��
					// ƴ���ַ����洢��str�У� ��oled����ʾ
					str = "Finger ID:";
					printf("ƥ��");
					OLED_Clear();
					OLED_ShowString(1, 1, "Unlock success");
					OLED_ShowString(2, 1, str);
					OLED_ShowNum(2, 12, seach.pageID, 1);
					ControlServo();
					return 0;
				}
				else
				{
					printf("����1");
					OLED_Clear();
					OLED_ShowString(1, 1, "Unlock failed");
					HAL_Delay(3000);
					return 1;
				}
			}
			else
			{
				printf("����2");
				OLED_Clear();
				OLED_ShowString(1, 1, "Unlock failed");
				HAL_Delay(3000);
				return 2;
			}
		}
	}
	return -1;
}

// ɾ��ָ��
void Del_FR(void)
{
	// GZ_Empty();//���ָ�ƿ�
	GZ_DeletChar(ID_select, 1); // ɾ������ָ��
}

// ��cardID 4�ֽ� ����flash,�ж�ID 0-3
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
