#include "main.h"
#include "wl2rgb.h"

GPIO_InitTypeDef GPIO_InitStruct;
TIM_HandleTypeDef TIM_HandleStruct[3];
TIM_OC_InitTypeDef TIM_OC_InitStruct[3];
static void SystemClock_Config(void);

uint32_t uhPrescalerValue = 0;

#define  PERIOD_VALUE       (uint32_t)(2000)  /* Period Value  */
uint32_t CMP_VAL = 0;

void setPwm(TIM_HandleTypeDef *hTim, TIM_OC_InitTypeDef* sConfig, uint32_t channel, uint32_t cmpValue);

#define LED_R	0
#define LED_G	1
#define LED_B	2

int main(void)
{
    /* -1- Init System*/
    HAL_Init();
    SystemClock_Config();
    /* -2- Calculate Prescaler */
    //We want the Timer to run with 8MHz (the maximum, since we're running of the internal Clock, which runs at 8MHz, without PLL)
    uhPrescalerValue = (uint32_t) (SystemCoreClock / 8000000) - 1;

    /* -3- Enable Clocks*/
    //GPIO
    __GPIOA_CLK_ENABLE();
    //TIMER
    __TIM14_CLK_ENABLE();
    __TIM16_CLK_ENABLE();
    __TIM17_CLK_ENABLE();

    /* -4- Configure TIM14 CH1 Pin*/
    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_TIM14;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* -4- Configure TIM16 CH1 Pin*/
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_TIM16;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* -4- Configure TIM17 CH1 Pin*/
    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_TIM17;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* -5- Configure Timer 14*/
    TIM_HandleStruct[0].Instance = TIM14;
    TIM_HandleStruct[0].Init.Prescaler = uhPrescalerValue;
    TIM_HandleStruct[0].Init.Period = PERIOD_VALUE;
    TIM_HandleStruct[0].Init.ClockDivision = 0;
    TIM_HandleStruct[0].Init.CounterMode = TIM_COUNTERMODE_UP;
    TIM_HandleStruct[0].Init.RepetitionCounter = 0;
    HAL_TIM_PWM_Init(&TIM_HandleStruct[0]);

    /* -6- Configure Timer 16*/
    TIM_HandleStruct[1].Instance = TIM16;
    TIM_HandleStruct[1].Init.Prescaler = uhPrescalerValue;
    TIM_HandleStruct[1].Init.Period = PERIOD_VALUE;
    TIM_HandleStruct[1].Init.ClockDivision = 0;
    TIM_HandleStruct[1].Init.CounterMode = TIM_COUNTERMODE_UP;
    TIM_HandleStruct[1].Init.RepetitionCounter = 0;
    HAL_TIM_PWM_Init(&TIM_HandleStruct[1]);

    /* -7- Configure Timer 17*/
    TIM_HandleStruct[2].Instance = TIM17;
    TIM_HandleStruct[2].Init.Prescaler = uhPrescalerValue;
    TIM_HandleStruct[2].Init.Period = PERIOD_VALUE;
    TIM_HandleStruct[2].Init.ClockDivision = 0;
    TIM_HandleStruct[2].Init.CounterMode = TIM_COUNTERMODE_UP;
    TIM_HandleStruct[2].Init.RepetitionCounter = 0;
    HAL_TIM_PWM_Init(&TIM_HandleStruct[2]);

    int i;

    for (i = 0; i < 3; i++) {
	/* -8- Configure PWM-Output*/
	TIM_OC_InitStruct[i].OCMode = TIM_OCMODE_PWM1;
	TIM_OC_InitStruct[i].OCPolarity = TIM_OCPOLARITY_HIGH;
	TIM_OC_InitStruct[i].OCFastMode = TIM_OCFAST_DISABLE;
	TIM_OC_InitStruct[i].OCNPolarity = TIM_OCNPOLARITY_HIGH;
	TIM_OC_InitStruct[i].OCIdleState = TIM_OCIDLESTATE_RESET;
	TIM_OC_InitStruct[i].OCNIdleState = TIM_OCNIDLESTATE_RESET;
	TIM_OC_InitStruct[i].Pulse = CMP_VAL;
	HAL_TIM_PWM_ConfigChannel(&TIM_HandleStruct[i], &TIM_OC_InitStruct[i], TIM_CHANNEL_1);
    }

    /* -7- Enable Timer and PWM Output*/
    HAL_TIM_PWM_Start(&TIM_HandleStruct[LED_R], TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&TIM_HandleStruct[LED_G], TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&TIM_HandleStruct[LED_B], TIM_CHANNEL_1);

    setPwm(&TIM_HandleStruct[LED_R], &TIM_OC_InitStruct[LED_R], TIM_CHANNEL_1, PERIOD_VALUE);
    setPwm(&TIM_HandleStruct[LED_G], &TIM_OC_InitStruct[LED_G], TIM_CHANNEL_1, PERIOD_VALUE);
    setPwm(&TIM_HandleStruct[LED_B], &TIM_OC_InitStruct[LED_B], TIM_CHANNEL_1, PERIOD_VALUE);

    float scale = (780 - 380) / (float)PERIOD_VALUE;

    while(1) {
	for (CMP_VAL = (PERIOD_VALUE); CMP_VAL != 0; CMP_VAL--) {
	    float r, g, b;
	    wavelengthToColor(((float)CMP_VAL) * scale + 380, &r, &g, &b);
	    r = PERIOD_VALUE - (float)PERIOD_VALUE * r;
	    g = PERIOD_VALUE - (float)PERIOD_VALUE * g;
	    b = PERIOD_VALUE - (float)PERIOD_VALUE * b;
	    setPwm(&TIM_HandleStruct[LED_R], &TIM_OC_InitStruct[LED_R], TIM_CHANNEL_1, r);
	    setPwm(&TIM_HandleStruct[LED_G], &TIM_OC_InitStruct[LED_G], TIM_CHANNEL_1, g);
	    setPwm(&TIM_HandleStruct[LED_B], &TIM_OC_InitStruct[LED_B], TIM_CHANNEL_1, b);
            HAL_Delay(3);
	}
	for (CMP_VAL = 0; CMP_VAL != (PERIOD_VALUE); CMP_VAL++) {
	    float r, g, b;
	    wavelengthToColor(((float)CMP_VAL) * scale + 380, &r, &g, &b);
	    r = PERIOD_VALUE - (float)PERIOD_VALUE * r;
	    g = PERIOD_VALUE - (float)PERIOD_VALUE * g;
	    b = PERIOD_VALUE - (float)PERIOD_VALUE * b;
	    setPwm(&TIM_HandleStruct[LED_R], &TIM_OC_InitStruct[LED_R], TIM_CHANNEL_1, r);
	    setPwm(&TIM_HandleStruct[LED_G], &TIM_OC_InitStruct[LED_G], TIM_CHANNEL_1, g);
	    setPwm(&TIM_HandleStruct[LED_B], &TIM_OC_InitStruct[LED_B], TIM_CHANNEL_1, b);
            HAL_Delay(3);
	}
    }

#if 0
    while (1) {
        /* -8- Illuminate LED*/
        for (CMP_VAL = 0; CMP_VAL != (PERIOD_VALUE); CMP_VAL++) {
	    setPwm(&TIM_HandleStruct[LED_R], &TIM_OC_InitStruct[LED_R], TIM_CHANNEL_1, CMP_VAL);
	    setPwm(&TIM_HandleStruct[LED_G], &TIM_OC_InitStruct[LED_G], TIM_CHANNEL_1, PERIOD_VALUE - CMP_VAL);
            HAL_Delay(1);
        }

        for (CMP_VAL = 0; CMP_VAL != (PERIOD_VALUE); CMP_VAL++) {
	    setPwm(&TIM_HandleStruct[LED_G], &TIM_OC_InitStruct[LED_G], TIM_CHANNEL_1, CMP_VAL);
	    setPwm(&TIM_HandleStruct[LED_B], &TIM_OC_InitStruct[LED_B], TIM_CHANNEL_1, PERIOD_VALUE - CMP_VAL);
            HAL_Delay(1);
        }

        for (CMP_VAL = 0; CMP_VAL != (PERIOD_VALUE); CMP_VAL++) {
	    setPwm(&TIM_HandleStruct[LED_B], &TIM_OC_InitStruct[LED_B], TIM_CHANNEL_1, CMP_VAL);
	    setPwm(&TIM_HandleStruct[LED_G], &TIM_OC_InitStruct[LED_G], TIM_CHANNEL_1, PERIOD_VALUE - CMP_VAL);
            HAL_Delay(1);
        }

        for (CMP_VAL = 0; CMP_VAL != (PERIOD_VALUE); CMP_VAL++) {
	    setPwm(&TIM_HandleStruct[LED_G], &TIM_OC_InitStruct[LED_G], TIM_CHANNEL_1, CMP_VAL);
	    setPwm(&TIM_HandleStruct[LED_R], &TIM_OC_InitStruct[LED_R], TIM_CHANNEL_1, PERIOD_VALUE - CMP_VAL);
            HAL_Delay(1);
        }
    }
#endif
}

void setPwm(TIM_HandleTypeDef *hTim, TIM_OC_InitTypeDef* sConfig, uint32_t channel, uint32_t cmpValue)
{
    sConfig->Pulse = cmpValue;
    HAL_TIM_PWM_ConfigChannel(hTim, sConfig, channel);
    HAL_TIM_PWM_Start(hTim, channel);
}

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_ClkInitTypeDef RCC_ClkInitStruct;

    // Internal Oscillator Directly (it's fairly accurate, at least better than the AVR one
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = 16;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0);

}
