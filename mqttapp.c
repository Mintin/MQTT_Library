#include "mqttapp.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "semphr.h"
#include "mqttapp.h"
#include <stdbool.h>
#include <string.h>
#include "mqtt.h"

#define m_printf  printf

#define SSID   "ChinaNet-406"                     //路由器SSID名称
#define PASS   "521YUNmeng"                 //路由器密码

extern MQTT_t  my_mqtt;

void sci6_send_byte(uint8_t *data, uint16_t len);
void mqtt_connect(uint8_t *client_id, uint8_t *username, uint8_t *password);
extern SemaphoreHandle_t espRxFlagSemaphore;
BaseType_t esp_send_cmd(uint8_t *cmd, uint8_t timeout);
void esp8266_rst()
{
	m_printf("esp8266_rst start\r\n");
	vTaskDelay(100);
	m_printf("esp8266_rst start [ok]\r\n");
}


uint8_t ESP_Connect_IoTServer(void)
{
	uint8_t tmp_buf[64] = { 0 };
	uint8_t res;
	uint8_t  cnt;
	m_printf("esp8266_rst start\r\n");
	vTaskDelay(100);
	m_printf("esp8266_rst start [ok]\r\n");

	m_printf("准备设置STA模式\r\n");
	if (esp_send_cmd("AT+CWMODE=1\r\n", 1)) {
		m_printf("设置STA模式成功\r\n");

	} else {
		m_printf("设置STA模式失败，准备重启\r\n");
		 return 1;
	}

	vTaskDelay(100);

	m_printf("准备准备重启\r\n");
	if (esp_send_cmd("AT+RST\r\n", 1)) {
		m_printf("重启成功\r\n");
	} else {
		m_printf("重启失败，重启失败\r\n");
		 return 2;
	}
	vTaskDelay(100);

	m_printf("查询是否STA模式\r\n");
	if (esp_send_cmd("AT+CWMODE?\r\n", 1)) {
		m_printf("查询是否STA模式成功\r\n");
	} else {
		m_printf("查询是否STA模式失败，查询是否STA模式失败\r\n");
		return 3;
	}

	m_printf("准备连接路由器\r\n");
	memset(tmp_buf, 0, 64);
	sprintf(tmp_buf, "AT+CWJAP=\"%s\",\"%s\"\r\n", SSID, PASS);
	//memset(at_rcv_buf,0,512);
	if (esp_send_cmd(tmp_buf, 6)) {                     //连接路由器,1s超时单位，总计30s超时时间

	m_printf("连接路由器成功\r\n");
	} else {
		m_printf("连接路由器失败，准备重启\r\n");
		 return 4;
	}            //串口提示数据


	vTaskDelay(100);
	vTaskDelay(100);
	vTaskDelay(100);
	vTaskDelay(100);


//122.222.162.237: 1883

	m_printf("准备TCP连接....\r\n");
	memset(tmp_buf, 0, 64);

	sprintf(tmp_buf, "AT+CIPSTART=\"TCP\",\"%s\",%d\r\n","1.116.175.121",1883);

	m_printf("连接的URL:%s\r\n",tmp_buf);
	if (esp_send_cmd(tmp_buf, 5)) {

		m_printf("TCP连接成功\r\n");
	} else {
		m_printf("TCP连接失败，准备重启\r\n");
		 return 5;
	}
vTaskDelay(100);
vTaskDelay(100);
vTaskDelay(100);

m_printf("开启透传模式....\r\n");
memset(tmp_buf, 0, 64);
sprintf(tmp_buf, "%s\r\n","AT+CIPMODE=1");
if (esp_send_cmd(tmp_buf, 5)) {
	m_printf("开启透传模式成功\r\n");
} else {
	m_printf("开启透传模式失败，准备重启\r\n");
	 return 6;
}

vTaskDelay(100);
vTaskDelay(100);
vTaskDelay(100);

m_printf("开始透传....\r\n");
memset(tmp_buf, 0, 64);
sprintf(tmp_buf, "%s\r\n","AT+CIPSEND");
if (esp_send_cmd(tmp_buf, 5)) {
	my_mqtt.tcp_connect_flag=pdTRUE;
m_printf("开启开始透传成功\r\n");
} else {
m_printf("开启开始透传失败，准备重启\r\n");
return 7;
}

vTaskDelay(100);
vTaskDelay(100);
vTaskDelay(100);

if(my_mqtt.tcp_connect_flag==pdTRUE)
{
	mqtt_connect("MQTT001","martin","456123");


}

if(my_mqtt.mqtt_connect_flag=pdTRUE)
{

	mqtt_subscribe("/client/esp456123/msg",0);


}


#if 0





/*
if(my_net_status.MqttStatus==MX_MQTT_CONNECT)
{
	m_printf("MQTT Connect [SUCCESS]\r\n");
	mqtt_subscribe("/client/esp456123/msg",1);
	vTaskDelay(100);
	return 0;
}
*/
#endif
	return 0;
}


