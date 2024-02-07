#ifndef    _MQTTAPP_H_
#define    _MQTTAPP_H_

//#include <semphr.h>
#include <stdint.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"

#include "FreeRTOS.h"
#include "semphr.h"
/*
typedef struct STATUS {
	bool network_status;
	uint8_t MqttStatus;
	uint8_t MqttPingStatus;
	uint32_t unix_time;
	uint32_t MqttPigCnt;

	SemaphoreHandle_t xMutex;
}ESP8266_STATUS;
*/


typedef struct
{
	uint8_t  tx_data[2];
	uint8_t  rx_data[2];
	SemaphoreHandle_t rx_sem;
}PING_t;

typedef struct
{
	uint8_t    tcp_connect_flag;
    uint8_t    mqtt_connect_flag;
	uint8_t    type;
	uint8_t    fixed_flag;
	uint8_t    ping_tick;
	uint32_t   payload_len;
	uint8_t    rx_flag;
	uint16_t    rx_len;
	SemaphoreHandle_t rx_sem;
	uint8_t    rx_buf[1024];

}MQTT_t;


#define  RX_PUB_NAME_LEN  64
#define  RX_PUB_MSG_LEN   128

typedef struct
{
	uint16_t  payload_len;
	uint16_t  topic_len;
    uint8_t   topic_name[RX_PUB_NAME_LEN];
    uint8_t    pub_msg[RX_PUB_MSG_LEN];
}RX_PUBLISH_MSG_t;




/*

*/
#define  ESP_AT_RX_LEN_MAX 512
typedef struct
{
    //uint8_t mqtt_connect_flag;
	//uint8_t mqtt_type;
	//uint32_t mqtt_msg_len;
	//uint8_t mqtt_rx_flag;
	//uint8_t  msg_rx_len;
	uint8_t  len;
	uint8_t  buf[ESP_AT_RX_LEN_MAX];
}ESP_AT_RX;


//esp_rx_msg



uint8_t ESP_Connect_IoTServer(void);

#endif
