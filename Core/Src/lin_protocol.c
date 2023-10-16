#include "lin_protocol.h"


//--------------------------------------------------------------
// Global variables
//--------------------------------------------------------------
LIN_MASTER_t LIN_MASTER;
LIN_FRAME_t LIN_FRAME;
uint8_t btn = 1;

uint8_t checker_1 = 0;
uint8_t checker_2 = 0;
uint8_t checker_3 = 0;

extern uint8_t flag_read_pin;

void led_testing() {
    if(HAL_GPIO_ReadPin (GPIOA, GPIO_PIN_0))
    {
        HAL_Delay(500);
        btn = -btn;
    }
    if(btn == 1)
    {
//        HAL_Delay(200);
        // Set The LED ON!
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
    }
    else
    {
//        HAL_Delay(200);
        // Else .. Turn LED OFF!
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
    }
}

// --------------------------------------------------------------
// sends data via LIN interface
// frame:
// frame_id = Unique ID [0x00 to 0xFF]
// data_len = number of data to be sent
// data [] = data to be sent
//
// return_value:
// LIN_OK = Frame has been sent
// LIN_WRONG_LEN = wrong number of data
// --------------------------------------------------------------
LIN_ERR_t UB_LIN_SendData(LIN_FRAME_t *frame, UART_HandleTypeDef *huart)
{
  uint8_t checksum = 0;
  uint8_t n = 0;
  uint8_t frame_id = 0;
  uint8_t tx_data = 0;

  // check the length
  if((frame->data_len < 1) || (frame->data_len > LIN_MAX_DATA)) {
    return(LIN_WRONG_LEN);
  }

  // calculate checksum
  checksum = p_LIN_makeChecksum(frame);

  // wait until the last byte has been sent
//  while (USART_GetFlagStatus(LIN_UART, USART_FLAG_TXE) == RESET);
//  while (__HAL_UART_GET_FLAG(huart, UART_FLAG_TXE) == RESET);
  //--------------------------------------------------------------------
  // Break-Field
  //--------------------------------------------------------------------
//  flag_read_pin = 1; //active timer for reading pin
//  uint8_t test_data = 0xB7;
    flag_read_pin = 1; //active timer for reading pin

  HAL_LIN_SendBreak(huart);

//  HAL_UART_Transmit(huart, &test_data, 1, 100);
  // wait until BreakField has been sent
//  while (USART_GetFlagStatus(LIN_UART, USART_FLAG_TC) == RESET);
//  while (__HAL_UART_GET_FLAG(huart, UART_FLAG_TC) == RESET);

  // small pause
  checker_1++;
//  p_LIN_wait_us(LIN_BREAKFIELD_DELAY);
  checker_2++;
  //--------------------------------------------------------------------
  // Sync-Field
  //--------------------------------------------------------------------
//  USART_SendData(huart, LIN_SYNC_DATA);
  uint8_t sync_byte = LIN_SYNC_DATA;
  HAL_UART_Transmit(huart, &sync_byte, 1, 100);

  // wait until SyncField has been sent
//  while (USART_GetFlagStatus(LIN_UART, USART_FLAG_TC) == RESET);
//  while (__HAL_UART_GET_FLAG(huart, UART_FLAG_TC) == RESET);

  // small pause
//  p_LIN_wait_us(LIN_DATA_BYTE_DELAY);

  //--------------------------------------------------------------------
  // ID-Field
  //--------------------------------------------------------------------
//  USART_SendData(huart, frame->frame_id);
  frame_id = frame->frame_id;
  HAL_UART_Transmit(huart, &frame_id, 1, 100);

  // wait until IDField has been sent
//  while (USART_GetFlagStatus(LIN_UART, USART_FLAG_TC) == RESET);
//  while (__HAL_UART_GET_FLAG(huart, UART_FLAG_TC) == RESET);

  // small pause
//  p_LIN_wait_us(LIN_FRAME_RESPONSE_DELAY);

  //--------------------------------------------------------------------
  // Data-Field [1...n]
  //--------------------------------------------------------------------
  for(n=0; n < frame->data_len; n++) {
//    USART_SendData(huart, frame->data[n]);
	  tx_data = frame->data[n];
	  HAL_UART_Transmit(huart, &tx_data, 1, 100);
	  checker_3++;
	  // wait until DataField has been sent
//    while (USART_GetFlagStatus(LIN_UART, USART_FLAG_TC) == RESET);
//	  while (__HAL_UART_GET_FLAG(huart, UART_FLAG_TC) == RESET);

	  // small Pause
//	  p_LIN_wait_us(LIN_DATA_BYTE_DELAY);
  }

  //--------------------------------------------------------------------
  // CRC-Field
  //--------------------------------------------------------------------
//  USART_SendData(huart, checksum);
  HAL_UART_Transmit(huart, &checksum, sizeof(checksum), 100);
  // wait until CRCField has been sent
//  while (USART_GetFlagStatus(LIN_UART, USART_FLAG_TC) == RESET);
//  while (__HAL_UART_GET_FLAG(huart, UART_FLAG_TC) == RESET);

  // small pause
  // so that the next frame is not sent too fast
//  p_LIN_wait_us(LIN_INTER_FRAME_DELAY);

  if (checker_1 == 1000 || checker_2 == 1000 || checker_3 == 1000) {
	  checker_1 = 0;
	  checker_2 = 0;
	  checker_3 = 0;
  }

  return(LIN_OK);
}

// --------------------------------------------------------------
// receives data via LIN interface
// frame:
// frame_id = Unique ID [0x00 to 0xFF]
// data_len = number of data to be received
// return:
// data [] = data received (if LIN_OK)
//
// return_value:
// LIN_OK = Frame was received
// LIN_WRONG_LEN = wrong number of data
// LIN_RX_EMPTY = no frame received
// LIN_WRONG_CRC = Checksum wrong
// --------------------------------------------------------------
LIN_ERR_t UB_LIN_ReceiveData(LIN_FRAME_t *frame, UART_HandleTypeDef *huart)
{
  uint32_t rx_timeout = 0;
  uint8_t checksum = 0;
  uint8_t n = 0;

  // check the length
  if((frame->data_len < 1) || (frame->data_len > LIN_MAX_DATA)) {
    return(LIN_WRONG_LEN);
  }

  // wait until the last byte has been sent
//  while (USART_GetFlagStatus(LIN_UART, USART_FLAG_TXE) == RESET);
  while (__HAL_UART_GET_FLAG(huart, UART_FLAG_TXE) == RESET);

  //-------------------------------
  // Break-Field
  //-------------------------------
  HAL_LIN_SendBreak(huart);
  // wait until BreakField has been sent
//  while (USART_GetFlagStatus(LIN_UART, USART_FLAG_TC) == RESET);
  while (__HAL_UART_GET_FLAG(huart, UART_FLAG_TC) == RESET);

  // small pause
  delay_us(LIN_BREAKFIELD_DELAY);

  //-------------------------------
  // Sync-Field
  //-------------------------------
//  USART_SendData(LIN_UART, LIN_SYNC_DATA);
  HAL_UART_Transmit(huart, (uint8_t*)LIN_SYNC_DATA, 1, 100);
  // wait until SyncField has been sent
//  while (USART_GetFlagStatus(LIN_UART, USART_FLAG_TC) == RESET);
  while (__HAL_UART_GET_FLAG(huart, UART_FLAG_TC) == RESET);

  // small pause
  delay_us(LIN_DATA_BYTE_DELAY);

  //-------------------------------
  // ID-Field
  //-------------------------------
//  USART_SendData(LIN_UART, frame->frame_id);
  HAL_UART_Transmit(huart, &(frame->frame_id), sizeof(frame->frame_id), 100);
  // wait until IDField has been sent
//  while (USART_GetFlagStatus(LIN_UART, USART_FLAG_TC) == RESET);
  while (__HAL_UART_GET_FLAG(huart, UART_FLAG_TC) == RESET);

  // small pause
  delay_us(LIN_DATA_BYTE_DELAY);

  //-------------------------------
  // Prepare master
  //-------------------------------
  LIN_MASTER.mode=RECEIVE_DATA;
  LIN_MASTER.data_ptr=0;
  LIN_MASTER.crc=0;

  LIN_FRAME.data_len=frame->data_len;
  LIN_FRAME.data[0]=0;

  //-------------------------------
  // wait until frame is received
  // or timeout
  //-------------------------------
  rx_timeout=0;
  n=0;
  do {
    // timeout counter
    rx_timeout++;
    if(rx_timeout > LIN_RX_TIMEOUT_CNT) {
      // leave the loop
      break;
    }
    // reset timeout, at data reception
    if(LIN_MASTER.data_ptr!=n) {
      n=LIN_MASTER.data_ptr; // update condition for data_pointer
      rx_timeout=0; //reset timeout each time get data
    }
  }while(LIN_MASTER.mode != SEND_DATA); //break when mode == SEND_DATA

  //-------------------------------
  // check if frame was received
  //-------------------------------
  if(LIN_MASTER.mode != SEND_DATA) { // execute when mode == RECEIVE_DATA || RECEIVE_CRC
    // no frame received
    LIN_MASTER.mode=SEND_DATA;
    // small pause
    // so that the next frame is not sent too fast
    delay_us(LIN_INTER_FRAME_DELAY);
    return(LIN_RX_EMPTY);
  }

  //-------------------------------
  // copy received data
  //-------------------------------
  for(n=0;n<frame->data_len;n++) {
    frame->data[n]=LIN_FRAME.data[n];
  }
  // calculate checksum
  checksum=p_LIN_makeChecksum(frame);

  //-------------------------------
  // check if crc ok
  //-------------------------------
  if(LIN_MASTER.crc!=checksum) {
    // checksum incorrect
    // small pause
    // so that the next frame is not sent too fast
    delay_us(LIN_INTER_FRAME_DELAY);
    return(LIN_WRONG_CRC);
  }

  //-------------------------------
  // data is ok
  //-------------------------------
  // small pause
  // so that the next frame is not sent too fast
  delay_us(LIN_INTER_FRAME_DELAY);

  return(LIN_OK);
}

// --------------------------------------------------------------
// internal function
// Calculate checksum over all data
// (classic-mode = inverted modulo256 sum)
//
// ret_value = checksum
//--------------------------------------------------------------
uint8_t p_LIN_makeChecksum(LIN_FRAME_t *frame)
{
  uint8_t ret_wert = 0;
  uint8_t n = 0;
  uint16_t dummy = 0;

  // calculate checksum
  dummy=0;
  for(n=0;n<frame->data_len;n++) {
    dummy+=frame->data[n];
    if(dummy>0xFF) {
      dummy-=0xFF;
    }
  }
  ret_wert=(uint8_t)(dummy);
  ret_wert^=0xFF;

  return(ret_wert);
}

void p_LIN_wait_us(uint32_t n)
{
  volatile uint32_t p,t;

  // small pause
  for(p=0;p<n;p++) {
    for(t=0;t<15;t++); // ca 1us
  }
}

//uint8_t LIN_testing_receive() {
//
//}



