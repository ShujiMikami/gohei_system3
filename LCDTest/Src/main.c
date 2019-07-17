
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2019 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f3xx_hal.h"
#include "usart.h"
#include "gpio.h"
#include <string.h>

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
static void writeCommand(uint8_t data);
static void writeData(uint8_t data);
static void Initialize();
static void ClearDisply();
static void ReturnHome();
static void writeCharToPos(uint8_t address, char data);
static void WriteString(char* string, int line);

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  Initialize();

  WriteString("abcde", 1);
  WriteString("fghij", 2);
  




  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */
void writeCommand(uint8_t data)
{
  //RSPinをコントロール側に
  HAL_GPIO_WritePin(RS_Pin_GPIO_Port, RS_Pin_Pin, GPIO_PIN_RESET);

  //RWPinを書き込みにする
  HAL_GPIO_WritePin(RW_Pin_GPIO_Port, RW_Pin_Pin, GPIO_PIN_RESET);

  //�?ータのセ�?�?
  HAL_GPIO_WritePin(DB0_pin_GPIO_Port, DB0_pin_Pin, (GPIO_PinState)(data & 0x01));
  HAL_GPIO_WritePin(DB1_Pin_GPIO_Port, DB1_Pin_Pin, (GPIO_PinState)((data >> 1) & 0x01));
  HAL_GPIO_WritePin(DB2_Pin_GPIO_Port, DB2_Pin_Pin, (GPIO_PinState)((data >> 2) & 0x01));
  HAL_GPIO_WritePin(DB3_Pin_GPIO_Port, DB3_Pin_Pin, (GPIO_PinState)((data >> 3) & 0x01));
  HAL_GPIO_WritePin(DB4_Pin_GPIO_Port, DB4_Pin_Pin, (GPIO_PinState)((data >> 4) & 0x01));
  HAL_GPIO_WritePin(DB5_Pin_GPIO_Port, DB5_Pin_Pin, (GPIO_PinState)((data >> 5) & 0x01));
  HAL_GPIO_WritePin(DB6_Pin_GPIO_Port, DB6_Pin_Pin, (GPIO_PinState)((data >> 6) & 0x01));
  HAL_GPIO_WritePin(DB7_Pin_GPIO_Port, DB7_Pin_Pin, (GPIO_PinState)((data >> 7) & 0x01));

  //Epinにパルス送信
  HAL_GPIO_WritePin(E_Pin_GPIO_Port, E_Pin_Pin, GPIO_PIN_SET);
  HAL_Delay(1);
  HAL_GPIO_WritePin(E_Pin_GPIO_Port, E_Pin_Pin, GPIO_PIN_RESET);
}
void writeData(uint8_t data)
{
  //RSPinをデータ側に
  HAL_GPIO_WritePin(RS_Pin_GPIO_Port, RS_Pin_Pin, GPIO_PIN_SET);

  //RWPinを書き込みにする
  HAL_GPIO_WritePin(RW_Pin_GPIO_Port, RW_Pin_Pin, GPIO_PIN_RESET);

  //�?ータのセ�?�?
  HAL_GPIO_WritePin(DB0_pin_GPIO_Port, DB0_pin_Pin, (GPIO_PinState)(data & 0x01));
  HAL_GPIO_WritePin(DB1_Pin_GPIO_Port, DB1_Pin_Pin, (GPIO_PinState)((data >> 1) & 0x01));
  HAL_GPIO_WritePin(DB2_Pin_GPIO_Port, DB2_Pin_Pin, (GPIO_PinState)((data >> 2) & 0x01));
  HAL_GPIO_WritePin(DB3_Pin_GPIO_Port, DB3_Pin_Pin, (GPIO_PinState)((data >> 3) & 0x01));
  HAL_GPIO_WritePin(DB4_Pin_GPIO_Port, DB4_Pin_Pin, (GPIO_PinState)((data >> 4) & 0x01));
  HAL_GPIO_WritePin(DB5_Pin_GPIO_Port, DB5_Pin_Pin, (GPIO_PinState)((data >> 5) & 0x01));
  HAL_GPIO_WritePin(DB6_Pin_GPIO_Port, DB6_Pin_Pin, (GPIO_PinState)((data >> 6) & 0x01));
  HAL_GPIO_WritePin(DB7_Pin_GPIO_Port, DB7_Pin_Pin, (GPIO_PinState)((data >> 7) & 0x01));

  //Epinにパルス送信
  HAL_GPIO_WritePin(E_Pin_GPIO_Port, E_Pin_Pin, GPIO_PIN_SET);
  HAL_Delay(1);
  HAL_GPIO_WritePin(E_Pin_GPIO_Port, E_Pin_Pin, GPIO_PIN_RESET);
}
void Initialize()
{
//40msec以上�?つ
  HAL_Delay(40);

  //FunctionSet 1行目 00110000
  writeCommand(0x30);
  //37us以上�?つ
  HAL_Delay(1);

  //FunctionSet 2行目 00111000
  writeCommand(0x38);
  //37us以上�?つ
  HAL_Delay(1);

  //Display ON/OFF 00001100
  writeCommand(0x0C);
  //37us以上�?つ
  HAL_Delay(1);

  //Display clear 00000001
  writeCommand(0x01);
  //1.52ms以上�?つ
  HAL_Delay(2);

  //Entry mode set 00000110
  writeCommand(0x06);
  //�?たなくて�?�?のかな?
  HAL_Delay(1);
}
void ClearDisply()
{
  //Display clear 00000001
  writeCommand(0x01);
  //1.52ms以上�?つ
  HAL_Delay(2);
}
void ReturnHome()
{
  writeCommand(0x02);
  HAL_Delay(2);
}
void writeCharToPos(uint8_t address, char data)
{
  writeCommand(0x80 + address);

  writeData((uint8_t)data);
}
void WriteString(char* string, int line)
{
  int cnt = 0;
  for(cnt = 0; cnt < strlen(string); cnt++){
    writeCharToPos(cnt + (line - 1) * 0x40, string[cnt]);
  }
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
