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

#include "sim800l.h"
#define STM32WB


/******************************SIM800L AT command definitions**********************************/

/**************************Pre-allocated buffers***********************************/

// AT+IFC - Flow control settings, Page 44
char cmd_flow_control[LEN_FLOW_CONTROL] = "AT+IFC=1,1";


// AT+GSMBUSY - Reject incoming calls, Page 161
char cmd_reject_call[LEN_REJECT_CALL] = "AT+GSMBUSY=x";


// AT+CSQ - Signal quality check, Page 83
char cmd_signal_quality[LEN_SIGNAL_QUALITY] = "AT+CSQ";


// AT+CMGF - Select SMS format (Text/PDU), Page 102-103
char cmd_sms_format[SELECT_SMS_MESSAGE_FORMAT] = "AT+CMGF=x";


// AT+CMGR - Read SMS by index, Page 106-108
char cmd_read_sms[READ_SMS_MESSAGE] = "AT+CMGR=xx,x";


// AT+CMGD - Delete SMS by index, Page 102
char cmd_delete_sms[DELETE_SMS_MESSAGE] = "AT+CMGD=xx";


// AT+CMGS - Send SMS, Page 110
char cmd_send_sms[SEND_SMS_MESSAGE + PHONE_NUMBER_LEN] = "AT+CMGS=\"+xxxxxxxxxxx\"";


// AT+CNMI - Configure SMS indication, Page 113
char cmd_cnmi_mode[LEN_CNMI_MODE] = "AT+CNMI=0,0,0,0,0";


// Common Response: "OK"
char response_ok[LEN_OK_RESPONSE] = "\r\nOK\r\n";


// Common Response: "ERROR"
char response_error[LEN_ERROR_RESPONSE] = "\r\nERROR\r\n";

/**************************Pre-allocated buffers***********************************/

/******************************SIM800L AT command definitions**********************************/

/**
 * @brief  Transmit an AT command to the SIM800L module via UART.
 * @param  huart: UART handle.
 * @param  command: Command string to be sent.
 * @param  command_length: Length of the command string.
 * @retval None
 */
void SIM800L_SendATCommand(UART_HandleTypeDef *huart, char *command, uint8_t command_length)
{
    HAL_UART_Transmit(huart, (uint8_t *)command, command_length, HAL_MAX_DELAY);
}



/**
 * @brief  Check for an expected response from the SIM800L module.
 *         Identifies "OK" or "ERROR" in the response buffer.
 * @param  huart: UART handle.
 * @param  response_buff: Buffer to store the response received.
 * @retval RESPONSE_OK if "OK" is found, RESPONSE_ERROR if "ERROR" is found, otherwise NO_RESPONSE.
 */
uint8_t SIM800L_CheckResponse(UART_HandleTypeDef *huart, char *response_buff)
{
    HAL_UART_Receive(huart, (uint8_t *)response_buff, BUFFER_SIZE, HAL_MAX_DELAY);
    if (strstr(response_buff, response_ok))
    {
        return RESPONSE_OK;
    }
    else if (strstr(response_buff, response_error))
    {
        return RESPONSE_ERROR;
    }
    return NO_RESPONSE;
}



/**
 * @brief  Send an SMS message to a specified phone number.
 *         Uses AT+CMGS command. A Ctrl+Z (ASCII 26) character is required to end the message.
 * @param  huart: UART handle.
 * @param  recipient_number: The phone number to send the SMS to.
 * @param  msg_text: The text message content.
 * @param  response_buff: Buffer to store the module's response.
 * @retval RESPONSE_OK on success, otherwise RESPONSE_ERROR.
 */
uint8_t SIM800L_SendSMS(UART_HandleTypeDef *huart, char *recipient_number, char *msg_text, char *response_buff)
{
    snprintf(cmd_send_sms, sizeof(cmd_send_sms), "AT+CMGS=\"%s\"", recipient_number);
    SIM800L_SendATCommand(huart, cmd_send_sms, strlen(cmd_send_sms));


    HAL_Delay(1000);

    SIM800L_SendATCommand(huart, msg_text, strlen(msg_text));

    char end_message =26;

    HAL_UART_Transmit(huart, (uint8_t *)&end_message, 1, HAL_MAX_DELAY);

    return SIM800L_CheckResponse(huart, response_buff);
}



/**
 * @brief  Set the SMS format (Text or PDU).
 * @note   Uses AT+CMGF command.
 * @param  huart: UART handle.
 * @param  format_mode: 0 for PDU mode, 1 for Text mode.
 * @param  response_buff: Buffer to store the module's response.
 * @retval RESPONSE_OK on success, otherwise RESPONSE_ERROR.
 */
uint8_t SIM800L_SetSMSFormat(UART_HandleTypeDef *huart, uint8_t format_mode, char *response_buff)
{
    cmd_sms_format[SELECT_SMS_MESSAGE_FORMAT-1] = format_mode + '0';
    SIM800L_SendATCommand(huart, cmd_sms_format, SELECT_SMS_MESSAGE_FORMAT);
    return SIM800L_CheckResponse(huart, response_buff);
}



/**
 * @brief  Configure new SMS message indications.
 * @note   Uses AT+CNMI command.
 * @param  huart: UART handle.
 * @param  mode: Mode parameter for SMS indication.
 * @param  mt: Message type parameter.
 * @param  bm: Broadcast messages parameter.
 * @param  ds: Delivery status parameter.
 * @param  buffer: Buffering parameter.
 * @param  response_buff: Buffer to store the module's response.
 * @retval RESPONSE_OK on success, otherwise RESPONSE_ERROR.
 */
uint8_t SIM800L_SetSMSIndicationMode(UART_HandleTypeDef *huart, uint8_t mode, uint8_t mt, uint8_t bm, uint8_t ds, uint8_t buffer, char *response_buff)
{
    if ((mode>3) || (mt>3) || (bm>2) || (ds>1) || (buffer>1))
        return INDEX_OUT_OF_RANGE;
    snprintf(cmd_cnmi_mode, sizeof(cmd_cnmi_mode), "AT+CNMI=%d,%d,%d,%d,%d", mode,mt,bm,ds,buffer);
    SIM800L_SendATCommand(huart, cmd_cnmi_mode, strlen(cmd_cnmi_mode));
    return SIM800L_CheckResponse(huart, response_buff);
}



/**
 * @brief  Retrieve the signal quality reported by the SIM800L module.
 * @note   Uses AT+CSQ command. Signal strength ranges from 0 to 31.
 * @param  huart: UART handle.
 * @param  response_buff: Buffer to store the module's response.
 * @retval Signal strength as an integer on success, RESPONSE_ERROR on failure.
 */
uint8_t SIM800L_GetSignalQuality(UART_HandleTypeDef *huart, char *response_buff)
{
    SIM800L_SendATCommand(huart, cmd_signal_quality, LEN_SIGNAL_QUALITY);
    if (SIM800L_CheckResponse(huart, response_buff) == RESPONSE_OK)
    {
        char *signal_data = strstr(response_buff,"+CSQ: ");
        if (signal_data)
        {
            int signal_strength = atoi(signal_data + 6);
            return (uint8_t)signal_strength;
        }
    }
    return RESPONSE_ERROR;
}



/**
 * @brief  Read an SMS message by its index in storage.
 * @note   Uses AT+CMGR command.
 * @param  huart: UART handle.
 * @param  sms_idx: Index of the SMS message to read.
 * @param  mode: Message read mode.
 * @param  response_buff: Buffer to store the SMS content.
 * @retval RESPONSE_OK on success, otherwise RESPONSE_ERROR.
 */
uint8_t SIM800L_ReadSMS(UART_HandleTypeDef *huart, uint8_t sms_idx, uint8_t mode, char *response_buff)
{
    snprintf(cmd_read_sms, sizeof(cmd_read_sms), "AT+CMGR=%d,%d", sms_idx,mode);
    SIM800L_SendATCommand(huart, cmd_read_sms, strlen(cmd_read_sms));
    return SIM800L_CheckResponse(huart,response_buff);
}



/**
 * @brief  Delete an SMS message at a specified index.
 * @note   Uses AT+CMGD command.
 * @param  huart: UART handle.
 * @param  sms_idx: Index of the SMS message to delete.
 * @param  response_buff: Buffer to store the module's response.
 * @retval RESPONSE_OK on success, otherwise RESPONSE_ERROR.
 */
uint8_t SIM800L_DeleteSMS(UART_HandleTypeDef *huart, uint8_t sms_idx, char *response_buff)
{
    snprintf(cmd_delete_sms, sizeof(cmd_delete_sms), "AT+CMGD=%d", sms_idx);
    SIM800L_SendATCommand(huart, cmd_delete_sms, strlen(cmd_delete_sms));
    return SIM800L_CheckResponse(huart,response_buff);
}



/**
 * @brief  Reject an incoming call.
 * @note   Uses AT+GSMBUSY command.
 * @param  huart: UART handle.
 * @param  rejection_mode: Mode parameter for rejecting calls.
 * @param  response_buff: Buffer to store the module's response.
 * @retval RESPONSE_OK on success, otherwise RESPONSE_ERROR.
 */
uint8_t SIM800L_RejectIncomingCall(UART_HandleTypeDef *huart, uint8_t rejection_mode, char *response_buff)
{
    snprintf(cmd_reject_call, sizeof(cmd_reject_call), "AT+GSMBUSY=%d", rejection_mode);
    SIM800L_SendATCommand(huart, cmd_reject_call, strlen(cmd_reject_call));
    return SIM800L_CheckResponse(huart,response_buff);
}



/**
 * @brief  Locate a character within a buffer range.
 * @param  buffer: Buffer to search within.
 * @param  start: Starting index for the search.
 * @param  end: Ending index for the search.
 * @param  target_character: Character to find.
 * @retval Position of character if found, -1 if not found.
 */
int SIM800L_FindCharacter(char *buffer, uint8_t start, uint8_t end, char target_character)
{
    for (int i = start; i <= end; i++)
    {
        if (buffer[i] == target_character)
        {
            return i;
        }
    }
    return -1; //char not found
}