/***********************************************************************
 *
 *  FILE        : ATOS001.c
 *  DATE        : 2024-01-15
 *  DESCRIPTION : Main Program
 *
 *  NOTE:THIS IS A TYPICAL EXAMPLE.
 *
 ***********************************************************************/
#include "FreeRTOS.h"
#include "task.h"
#include "Config_SCI6.h"
#include "Config_SCI9.h"
//#include "semphr.h"
#include "queue.h"
#include "semphr.h"
#include <iodefine.h>
#include <stdio.h>
#include <string.h>
#include "mqtt.h"
#include "Config_RTC.h"
#include <time.h>
#include <stdbool.h>
#include "mqttapp.h"
#include "mqtt.h"

#define LED1        D,7

extern void sci6_rx_task(void *pvParameters);
extern void sci9_rx_task(void *pvParameters);

MQTT_t  my_mqtt;
PING_t  ping_msg;
RX_PUBLISH_MSG_t rx_publish;
xQueueHandle sci6xQueueRx;
SemaphoreHandle_t sci6xTxOverSemaphore = NULL;

xQueueHandle sci9xQueueRx;
SemaphoreHandle_t sci9xTxOverSemaphore = NULL;

//SemaphoreHandle_t espRxFlagSemaphore = NULL;
//MQTT_t mqtt_rx_msg;
ESP_AT_RX esp8266_rx_msg;
//extern ESP8266_STATUS my_net_status;

void sci9_send_byte(uint8_t *data, uint16_t len) {
	uint8_t i;
	for (i = 0; i < len; i++) {
		R_Config_SCI9_Serial_Send(&data[i], 1);
		xSemaphoreTake(sci9xTxOverSemaphore, portMAX_DELAY);
	}

}

void sci6_send_byte(uint8_t *data, uint16_t len) {
	uint8_t i;
	for (i = 0; i < len; i++) {
		R_Config_SCI6_Serial_Send(&data[i], 1);
		xSemaphoreTake(sci6xTxOverSemaphore, portMAX_DELAY);
	}

}
/*
BaseType_t my_esp_send_msg(uint8_t *msg, uint8_t len, uint8_t timeout) {
	return 0;
}
*/


BaseType_t mqtt_send_msg(uint8_t *msg, uint8_t len, uint8_t timeout) {

	memset(my_mqtt.rx_buf, 0, 1024);
	my_mqtt.rx_len=0;
	sci6_send_byte(msg, len);
	vTaskDelay(100);
	return 0;
}

BaseType_t esp_send_cmd(uint8_t *cmd, uint8_t timeout) {
	BaseType_t res;
	memset(esp8266_rx_msg.buf, 0, ESP_AT_RX_LEN_MAX);
	esp8266_rx_msg.len = 0;
	sci6_send_byte(cmd, strlen(cmd));
	while (timeout) {
		vTaskDelay(100);
		timeout--;
	}
	printf("%s", esp8266_rx_msg.buf);
	printf("\r\n%s");
	if (strstr(esp8266_rx_msg.buf, "OK")) {
		res = pdTRUE;
		//esp_cmd_rx_process();
	} else {
		res = pdFALSE;
	}
	return res;
}

BaseType_t send_ping_msg(void)
{

	sci6_send_byte(ping_msg.tx_data,2);
	if(xSemaphoreTake(ping_msg.rx_sem,( TickType_t ) 300) == pdTRUE)
	{
    printf("ping msg ok\r\n");
    return pdTRUE;
	}
	else
	{
		printf("ping msg error\r\n");
		return pdFALSE;
	}
	return pdFALSE;
}

void main_task(void *pvParameters) {
	static uint32_t cnt = 1;
	static uint8_t res = 0xFF;
	uint8_t msg_buf[64] = { 0 };
	memset(esp8266_rx_msg.buf, 0, 512);
	esp8266_rx_msg.len = 0;
	ping_msg.rx_data[0]=0xD0;
	ping_msg.rx_data[1]=0x00;
	ping_msg.tx_data[0]=0xC0;
	ping_msg.tx_data[1]=0x00;
	if(ping_msg.rx_sem==NULL)
	{
		ping_msg.rx_sem=xSemaphoreCreateBinary();
	}

	while (1) {
		//get_rtc_unix_count();
		PORTD.PODR.BIT.B7 = ~PORTD.PIDR.BIT.B7;
		vTaskDelay(100);
		cnt++;

		if (cnt % 10 == 0) {
			//ESP_Connect_IoTServer();
			if (res > 0) {
				res = ESP_Connect_IoTServer();
			}
		}
		if (cnt % 60 == 0) {
			if(my_mqtt.mqtt_connect_flag==pdTRUE)
			{
				send_ping_msg();
			//	MQTT_SendPingMsg();
			}
		}

#if 0


		if (my_net_status.MqttStatus == MX_MQTT_CONNECT) {
			printf("mqtt ping msg.... out\r\n");
			my_net_status.MqttPingStatus = pdFALSE;
			MQTT_SendPingMsg();
		}

		if (pdFALSE) {
			memset(msg_buf, 0, 64);
			sprintf(msg_buf, "msg count: %d", cnt);
			if (my_mqtt.mqtt_connect_flag==pdTRUE) {
				MQTT_PublishQs0("/commands/dispense", msg_buf); //  "/server/id456123/msg"
			}
		}
		//MQTT_PublishQs0

	    // my_mqtt.ping_tick++
		if (pdFALSE) {

			if (my_net_status.MqttStatus == MX_MQTT_CONNECT) {
				memset(msg_buf, 0, 64);
				sprintf(msg_buf, "msg count: %d", cnt);
				MQTT_PublishQs0("/commands/dispense", msg_buf); //  "/server/id456123/msg"
			}
		}
		if (my_mqtt.ping_tick % 60 == 0) {

			if (my_net_status.MqttStatus == MX_MQTT_CONNECT) {
				printf("mqtt ping msg.... out\r\n");
				my_net_status.MqttPingStatus = pdFALSE;
				MQTT_SendPingMsg();
			}

		}
#endif
	}
	vTaskDelete(NULL);

}

void sci6_rx_task(void *pvParameters) {

	static uint8_t curbyte;
	static uint8_t prebyte;
	static uint16_t rx_cnt;
	uint8_t i, ucQueueMsgValue = 0;
	memset(esp8266_rx_msg.buf, 0, ESP_AT_RX_LEN_MAX);
	esp8266_rx_msg.len = 0;
	sci6xTxOverSemaphore = xSemaphoreCreateBinary();
	//espRxFlagSemaphore = xSemaphoreCreateBinary();
	my_mqtt.type = 0;
	my_mqtt.rx_len = 0;
	if(my_mqtt.rx_sem==NULL)
	{
		my_mqtt.rx_sem=xSemaphoreCreateBinary();
	}

	my_mqtt.mqtt_connect_flag=pdFALSE;
	my_mqtt.tcp_connect_flag=pdFALSE;
	//my_mqtt.tcp_connect_flag= pdTRUE
	memset(my_mqtt.rx_buf,0,1024);


	esp8266_rx_msg.len=0;
	memset(esp8266_rx_msg.buf,0,ESP_AT_RX_LEN_MAX);
	rx_publish.payload_len=0;
	rx_publish.topic_len=0;
	memset(rx_publish.topic_name,0,RX_PUB_NAME_LEN);
	memset(rx_publish.pub_msg,0,RX_PUB_MSG_LEN);

	sci6xQueueRx = xQueueCreate(512, sizeof(uint8_t));
	R_Config_SCI6_Start();
	printf("sci6_rx_task\r\n");
	sci6_send_byte("hello\r\n", 7);
	while (1) {

		if (xQueueReceive(sci6xQueueRx, (void*) &ucQueueMsgValue,
		portMAX_DELAY)) {
			// printf("%c",ucQueueMsgValue);
			 if (my_mqtt.tcp_connect_flag==pdTRUE) {	//if (my_mqtt.tcp_connect_flag= pdTRUE) {   //	if (pdFALSE) {
         if(my_mqtt.fixed_flag==pdFALSE)
         {
			switch(ucQueueMsgValue)
					{
					case MQTT_CONNECT:     my_mqtt.fixed_flag=pdTRUE; break;
					case MQTT_CONNACK:     my_mqtt.fixed_flag=pdTRUE; break;
					case MQTT_PUBLISH:     my_mqtt.fixed_flag=pdTRUE; break;
					case MQTT_PUBACK:      my_mqtt.fixed_flag=pdTRUE; break;
					case MQTT_SUBSCRIBE:   my_mqtt.fixed_flag=pdTRUE; break;
					case MQTT_SUBACK:      my_mqtt.fixed_flag=pdTRUE; break;
					case MQTT_UNSUBSCRIBE: my_mqtt.fixed_flag=pdTRUE; break;
					case MQTT_UNSUBACK:    my_mqtt.fixed_flag=pdTRUE; break;
					case MQTT_PINGREQ:     my_mqtt.fixed_flag=pdTRUE; break;
					case MQTT_PINGRESP:    my_mqtt.fixed_flag=pdTRUE; break;
					case MQTT_DISCONNECT:  my_mqtt.fixed_flag=pdTRUE; break;
					default:               my_mqtt.fixed_flag=pdFALSE; break;
					}
			my_mqtt.type = ucQueueMsgValue;
			my_mqtt.rx_len = 0;
         }
         if(my_mqtt.fixed_flag==pdTRUE)
         {


        	 my_mqtt.rx_buf[my_mqtt.rx_len++] =ucQueueMsgValue;
			if(my_mqtt.rx_len==2)
			{
				my_mqtt.payload_len =ucQueueMsgValue;//= decodeRemainLength(&my_mqtt.rx_buf[1]);
			}

			if((my_mqtt.payload_len+2)==my_mqtt.rx_len)
			{

				xSemaphoreGive(my_mqtt.rx_sem);

			}

			// printf("-%02X-%02X",ucQueueMsgValue,my_mqtt.rx_len);
         }

		} else {
			esp8266_rx_msg.buf[esp8266_rx_msg.len++] =
					ucQueueMsgValue;
			if (esp8266_rx_msg.len > (ESP_AT_RX_LEN_MAX-1)) {
				esp8266_rx_msg.len=0;
			}
		}

	}
}
vTaskDelete(NULL);
}

void sci9_rx_task(void *pvParameters) {
uint8_t i;
uint8_t sci9RxValue = 0;
sci9xTxOverSemaphore = xSemaphoreCreateBinary();
sci9xQueueRx = xQueueCreate(512, sizeof(uint8_t));
if(my_mqtt.rx_sem==NULL)
{
	my_mqtt.rx_sem=xSemaphoreCreateBinary();
}
R_Config_SCI9_Start();
printf("sci9_rx_task\r\n");
while (1) {
	xSemaphoreTake(my_mqtt.rx_sem, portMAX_DELAY);

	if(my_mqtt.rx_buf[0] == 0x30)
	{
		rx_publish.payload_len=my_mqtt.rx_buf[1];
		rx_publish.topic_len=my_mqtt.rx_buf[3];
        memcpy(rx_publish.topic_name,my_mqtt.rx_buf+4,rx_publish.topic_len);
        memcpy(rx_publish.pub_msg,my_mqtt.rx_buf+4+rx_publish.topic_len,rx_publish.payload_len-rx_publish.topic_len);
		printf("receive topic: [%s]\r\n",rx_publish.topic_name);
		printf("receive message: [%s]\r\n",rx_publish.pub_msg);
		memset(rx_publish.topic_name,0,RX_PUB_NAME_LEN);
		memset(rx_publish.pub_msg,0,RX_PUB_MSG_LEN);

	}
#if 0


	  printf("mqtt start\r\n");
		for(i=0;i<my_mqtt.rx_len;i++)
		{
			printf("%02X ",my_mqtt.rx_buf[i]);
		}
		printf("\r\nmqtt end\r\n");
#endif


		if (my_mqtt.payload_len == 0 && my_mqtt.rx_buf[0] == 0xD0) {
					xSemaphoreGive(ping_msg.rx_sem);
				}

				if (my_mqtt.payload_len == 2) {
					if (my_mqtt.rx_buf[0] == 0x20 && my_mqtt.rx_buf[1] == 0x02
							&& my_mqtt.rx_buf[2] == 0x00 && my_mqtt.rx_buf[3] == 0x00) {
						printf("mqtt connect ok\r\n");
					}
				}


	memset(my_mqtt.rx_buf,0,my_mqtt.payload_len+5);
	  my_mqtt.rx_len=0;
}
vTaskDelete(NULL);

}
