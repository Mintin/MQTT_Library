/*-------------------------------------------------*/
/*            功能介绍                                      */
/*-------------------------------------------------*/
/*                                                 */
/*            实现MQTT协议功能的头文件             */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __MQTT_H
#define __MQTT_H
#include <stdint.h>

// 定义MQTT报文类型
#define MQTT_CONNECT    0x10
#define MQTT_CONNACK    0x20
#define MQTT_PUBLISH    0x30
#define MQTT_PUBACK     0x40
#define MQTT_SUBSCRIBE  0x80
#define MQTT_SUBACK     0x90
#define MQTT_UNSUBSCRIBE    0xA0
#define MQTT_UNSUBACK   0xB0
#define MQTT_PINGREQ    0xC0
#define MQTT_PINGRESP   0xD0
#define MQTT_DISCONNECT    0xE0




// 定义MQTT连接标志
#define MQTT_CONNECT_FLAG_CLEAN     0x02
#define MQTT_CONNECT_FLAG_WILL      0x04
#define MQTT_CONNECT_FLAG_WILL_QOS0 0x00
#define MQTT_CONNECT_FLAG_WILL_QOS1 0x08
#define MQTT_CONNECT_FLAG_WILL_QOS2 0x10
#define MQTT_CONNECT_FLAG_WILL_RETAIN   0x20
#define MQTT_CONNECT_FLAG_PASSWORD  0x40
#define MQTT_CONNECT_FLAG_USERNAME  0x80
#define MX_MQTT_CONNECT    0x01
#define MX_MQTT_DISCONNECT    0x02


void mqtt_connect(uint8_t *client_id, uint8_t *username, uint8_t *password);
void mqtt_subscribe(uint8_t *topic_name, uint8_t Qos);
void MQTT_PublishQs0(uint8_t *topic, uint8_t *data);
void MQTT_SendPingMsg(void);
uint32_t decodeRemainLength(uint8_t *data);
//void MQTT_PublishQs0(uint8_t *topic, uint8_t *msg);

#endif
