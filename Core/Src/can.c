/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    can.c
  * @brief   This file provides code for the configuration
  *          of the CAN instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "can.h"

/* USER CODE BEGIN 0 */
#include "usbd_cdc_if.h"
#include "string.h"

volatile struct {
	//uint8_t CAN_Flag_Data;      		// Data messages need to be sent
	uint8_t CAN_Flag_HeartBeat; 		// HeartBeat message need to be sent
} CAN_Flag_Status;
/* USER CODE END 0 */

CAN_HandleTypeDef hcan2;

/* CAN2 init function */
void MX_CAN2_Init(void)
{

  /* USER CODE BEGIN CAN2_Init 0 */

  /* USER CODE END CAN2_Init 0 */

  /* USER CODE BEGIN CAN2_Init 1 */

  /* USER CODE END CAN2_Init 1 */
  hcan2.Instance = CAN2;
  hcan2.Init.Prescaler = 10;
  hcan2.Init.Mode = CAN_MODE_NORMAL;
  hcan2.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan2.Init.TimeSeg1 = CAN_BS1_15TQ;
  hcan2.Init.TimeSeg2 = CAN_BS2_2TQ;
  hcan2.Init.TimeTriggeredMode = DISABLE;
  hcan2.Init.AutoBusOff = DISABLE;
  hcan2.Init.AutoWakeUp = DISABLE;
  hcan2.Init.AutoRetransmission = DISABLE;
  hcan2.Init.ReceiveFifoLocked = DISABLE;
  hcan2.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN2_Init 2 */
  if(HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING)!= HAL_OK){
	  while(CDC_Transmit_FS((uint8_t *)"PROVA\n\r", strlen("PROVA\n\r")) != USBD_OK);
  }

  /* USER CODE END CAN2_Init 2 */

}

void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(canHandle->Instance==CAN2)
  {
  /* USER CODE BEGIN CAN2_MspInit 0 */

  /* USER CODE END CAN2_MspInit 0 */
    /* CAN2 clock enable */
    __HAL_RCC_CAN2_CLK_ENABLE();
    __HAL_RCC_CAN1_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**CAN2 GPIO Configuration
    PB5     ------> CAN2_RX
    PB6     ------> CAN2_TX
    */
    GPIO_InitStruct.Pin = CAN_RX_Pin|CAN_TX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* CAN2 interrupt Init */
    HAL_NVIC_SetPriority(CAN2_RX0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(CAN2_RX0_IRQn);
  /* USER CODE BEGIN CAN2_MspInit 1 */

  /* USER CODE END CAN2_MspInit 1 */
  }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{

  if(canHandle->Instance==CAN2)
  {
  /* USER CODE BEGIN CAN2_MspDeInit 0 */

  /* USER CODE END CAN2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN2_CLK_DISABLE();
    __HAL_RCC_CAN1_CLK_DISABLE();

    /**CAN2 GPIO Configuration
    PB5     ------> CAN2_RX
    PB6     ------> CAN2_TX
    */
    HAL_GPIO_DeInit(GPIOB, CAN_RX_Pin|CAN_TX_Pin);

    /* CAN2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(CAN2_RX0_IRQn);
  /* USER CODE BEGIN CAN2_MspDeInit 1 */

  /* USER CODE END CAN2_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
void SetCAN_Flag_HeartBeat(uint8_t value) {
  CAN_Flag_Status.CAN_Flag_HeartBeat = value;
}

void CAN_setFilter(){

	CAN_FilterTypeDef canFil; //CAN Bus Filter

    canFil.FilterBank = 10;
    canFil.FilterMode = CAN_FILTERMODE_IDMASK;
    canFil.FilterFIFOAssignment = CAN_RX_FIFO0;
    canFil.FilterIdHigh = 0;
    canFil.FilterIdLow = 0;
    canFil.FilterMaskIdHigh = 0;
    canFil.FilterMaskIdLow = 0;
    canFil.FilterScale = CAN_FILTERSCALE_32BIT;
    canFil.FilterActivation = ENABLE;
    canFil.SlaveStartFilterBank = 0;

    HAL_CAN_ConfigFilter(&hcan2, &canFil);
}
void sendCAN(void){

	CAN_TxHeaderTypeDef Can_Tx_Header;
	if (CAN_Flag_Status.CAN_Flag_HeartBeat == ENABLE) {
	    CAN_Flag_Status.CAN_Flag_HeartBeat = DISABLE;

	    Can_Tx_Header.IDE = CAN_ID_STD;
	    Can_Tx_Header.StdId = ID_TELEMETRY_HEARTBEAT;
	    Can_Tx_Header.RTR = CAN_RTR_DATA;
	    Can_Tx_Header.DLC = 0;
	    uint8_t empty_mex[0];
	    uint32_t null_point;

	    /* Transmit the message if there is an available mailbox -----------------*/
	    while(HAL_CAN_AddTxMessage(&hcan2, &Can_Tx_Header, empty_mex, &null_point)!=HAL_OK){};
	  }
}
/* USER CODE END 1 */
