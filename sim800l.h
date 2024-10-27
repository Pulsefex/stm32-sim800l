/**
 *
 * This SIM800L library was written by
 *
 * Leroy Musa (219198761)
 * Mame Mor Mbacke (218666206)
 * Justin Chiu (219589076)
 * Nicole Xiong (219574334)
 * Connor Chan (218993089)
 *
 * https://github.com/pulsefex/sim800l
 */

#ifndef __SIM800L_H__
#define __SIM800L_H__

#if defined(STM32WB)
#include "stm32wbxx_hal.h"
#include "stm32wbxx_hal_uart.h"
#else
#endif

#include <stdbool.h>
#include <string.h>


#define SELECT_SMS_MESSAGE_FORMAT           9           //AT+CMGF pg 102-103
#define READ_SMS_MESSAGE                    12          //AT+CMGR pg 106-108
#define DELETE_SMS_MESSAGE                  10          //AT+CMGD pg 102
#define SEND_SMS_MESSAGE                    10          //AT+CMGS pg 110
#define LEN_CNMI_MODE                       17          //AT+CNMI pg 113
#define LEN_OK_RESPONSE                     6
#define LEN_ERROR_RESPONSE                  9
#define MSG_CONTENT_LEN                     79          //custom length for message content
#define LEN_SIGNAL_QUALITY                  6           //AT+CSQ pg 82-83
#define LEN_FLOW_CONTROL                    10          //AT+IFC pg 44
#define LEN_REJECT_CALL                     12          //AT+GSMBUSY pg 161
#define SMS_STATE_READ                      0
#define SMS_STATE_UNREAD                    1
#define RESPONSE_ERROR                      0x1
#define RESPONSE_OK                         0x2
#define NO_RESPONSE                         0x4
#define NO_SMS                              0x8
#define INDEX_OUT_OF_RANGE                  0x10
#define LENGTH_OUT_OF_RANGE                 0x20
#define BUFFER_SIZE                         255
#define SMS_STORAGE                         35
#define PHONE_NUMBER_LEN                    12          //max format: [+][x][xxxxxxxxxx]


/****************************************Function Prototypes*******************************************/

uint8_t SIM800L_ReadSMS(UART_HandleTypeDef *huart, uint8_t sms_idx, uint8_t mode, char* response_buff);
uint8_t SIM800L_DeleteSMS(UART_HandleTypeDef *huart, uint8_t sms_idx, char* response_buff);
uint8_t SIM800L_SendSMS(UART_HandleTypeDef *huart, char* recipient_number, char* msg_text, char* response_buff);
uint8_t SIM800L_SetSMSFormat(UART_HandleTypeDef *huart, uint8_t format_mode, char* response_buff);
uint8_t SIM800L_SetSMSIndicationMode(UART_HandleTypeDef *huart, uint8_t mode, uint8_t mt, uint8_t bm, uint8_t ds, uint8_t buffer, char* response_buff); 
uint8_t SIM800L_GetSignalQuality(UART_HandleTypeDef *huart, char* response_buff);
uint8_t SIM800L_CheckResponse(UART_HandleTypeDef *huart, char* response_buff);
uint8_t SIM800L_GetSenderPhoneNumber(UART_HandleTypeDef *huart, char* phone_number, char* response_buff);
uint8_t SIM800L_GetMessageContent(UART_HandleTypeDef *huart, char* content, char* response_buff);
uint8_t SIM800L_GetSMSState(UART_HandleTypeDef *huart, char* response_buff);
uint8_t SIM800L_RejectIncomingCall(UART_HandleTypeDef *huart, uint8_t rejection_mode, char* response_buff);
void SIM800L_SendATCommand(UART_HandleTypeDef *huart, char* command, uint8_t command_length);
int SIM800L_FindCharacter(char* buffer, uint8_t start, uint8_t end, char target_character);

/****************************************Function Prototypes*******************************************/



#endif // __SIM800L_H__
