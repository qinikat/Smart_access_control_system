#include "main.h" // Device header

void Key_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /*Configure GPIO pins : PBPin PBPin PBPin PBPin */
    GPIO_InitStruct.Pin = key1_Pin | key2_Pin | key3_Pin | key4_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

uint8_t Key_Scan(void)
{
    uint8_t keynum = 0;
    if (HAL_GPIO_ReadPin(GPIOB, key1_Pin) == 0)
    {
        HAL_Delay(20);
        if (HAL_GPIO_ReadPin(GPIOB, key1_Pin) == 0)
        {
            while (HAL_GPIO_ReadPin(GPIOB, key1_Pin) == 0)
                ;
            keynum = 1;
        }
    }
    if (HAL_GPIO_ReadPin(GPIOB, key2_Pin) == 0)
    {
        HAL_Delay(20);
        if (HAL_GPIO_ReadPin(GPIOB, key2_Pin) == 0)
        {
            while (HAL_GPIO_ReadPin(GPIOB, key2_Pin) == 0)
                ;
            keynum = 2;
        }
    }
    if (HAL_GPIO_ReadPin(GPIOB, key3_Pin) == 0)
    {
        HAL_Delay(20);
        if (HAL_GPIO_ReadPin(GPIOB, key3_Pin) == 0)
        {
            while (HAL_GPIO_ReadPin(GPIOB, key3_Pin) == 0)
                ;
            keynum = 3;
        }
    }
    if (HAL_GPIO_ReadPin(GPIOB, key4_Pin) == 0)
    {
        HAL_Delay(20);
        if (HAL_GPIO_ReadPin(GPIOB, key4_Pin) == 0)
        {
            while (HAL_GPIO_ReadPin(GPIOB, key4_Pin) == 0)
                ;
            keynum = 4;
        }
    }
    return keynum;
}
