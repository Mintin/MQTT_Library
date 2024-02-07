/*-------------------------------------------------*/
/*            功能介绍                                      */
/*-------------------------------------------------*/
/*                                                 */
/*            实现MQTT协议功能的源文件             */
/*                                                 */
/*-------------------------------------------------*/

#include "mqtt.h"
#include "string.h"
#include "stdio.h"
#include "utils_hmac.h"
#include<stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include "mqttapp.h"

const uint8_t MY_PRI_KEY[] =
		"MIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQC4jxOKzTeRlrbZ \
z0/0omW95ZvaLo27ZGc6vyclqA0ZZ7RpDJJQjv4gwItWPP5M/4y/Yb1fuBr4AtDc \
Wyrt7apowHJ90EEDxJ+wc8yvApQIoiamQCYDvC2Va2L72jh7AQ40RmfAukCUI/gT \
1+0Hk5wEmwpzpd726v59c3OkRfcfiqFgJUT3IlPOWIA3eDDp7edJj3Y3vywrhegZ \
KnCC84Dg1+j/UK2mS4cWO+iZ82IuQb/I4zmRxq0shBSQ3xYd+7MjH6KDuJgvh8Ag \
IjZDaQYALFxW/SC5ruhSnb+Vnu3Jt2dJvJ2hUEN4Q0w5g06RRFbGYnheO/8n9W7J \
RQjSq94HAgMBAAECggEALc7haFmwWbQBVYWF0HAAdcZITTqx1hnBH40zf7665Zve \
ZRaIOHONcv6ndvOzoNPrENOHJ/tvwjAcvBzqxlxvqAlgQYcPpApJIohTfPrUWGI6 \
2pfgd5vc6pHZYy6SR0GHT4VMGkOAZOSanf5BqFhEaMY8rWC1uy6vlN6zjMoK4Oph \
ZR8pJ74WXB9oIIKtH8ftLeluknJWoO/xTSYRwzTL6Bpz19OYaqlQNuU9MzVaRQu5 \
aFCwX43NxEfXojy+jO44CqBbwYG5mKHkyxwfCyzIL0I7GgBqT9yAfe4kJ/8Z/d7H  \
BRaeMV85JaUWwB5/WzwaKaaHMQ5w5XtgS9pNQ5/RZQKBgQDaj3AN3Zz88CKiH1iF \
Kk0yn7zdlwILy//JJjHhBEReuTRmRZmziJa8OsOzvk/IjcL4SDJEyidljI2c6EB2 \
+L1NZWuwsAYMCWnURcc7KXGQlZiU4TMXUbGv53zPMXUDgUB+U57XUgBaTy+yP55Z \
kSlGW48g7VGLsGjbuffURd3aZQKBgQDYLJGkrxj7hao8vjlodkPQRysHC/JPt1cP\
mGrToq79cKIWU5W4FXK2CGbirPe0H4wQE8abxz/jNUQkm9OfTNP+gMiACVMUdvRf\
31h750ENFuByx2PIa5KLJ0ve3W+IJGWdpmLX/dCfZCsln3Hn3xKVQbMS1Jz1YmCd\
PRLY8Ol5+wKBgQCintV2S62yeg6a6I3Zz4bPIxHJIsHv+yLgDR5R2fXxMQavDhx1\
/CyD84mH5RPCGtC2IindCzlTEsiPtjO5EgzwWIVPWzm0nRB0VFMQTx7jwJteM03r\
jOqUAr7X/2mcaQCOvjNS7P28NG09yJ5O8HZwllGh41KoiGMNEos3E6EmDQKBgGBe\
fg8Mf/ag/+NUagNHtbOEBop9qOggs48gQ39VZu1U6O1LFdOavCiqVjN3KHYEGkyQ\
hFmt2a1QcbBgR1ccyOUMRFXP0lmtLgQPbULPNRDAwg5pFPKkAhBG1GvmC3KFx0cg\
hbAEVpy36wQ6sGzN5VrkjxA1+ELsOGXgdCaCP5NBAoGAQkZDjgBFrWNGavuXuOMx\
BSqfhN5gz0KWQeIdiY4vGGd/WnXuPsYiAEiqN9WdZnZnL9cIEnuqmbI2AK6PBPwc\
w222mq7azdF+isQvHUMkTwCDOUQ7rPS+S/IyPiCiCDVLgCHcxxj1DEokGTyFvYJS\
ItOMw6+ogmvG9XoTMWSK99s=";

//

void generate_jwt() {

}

//extern BaseType_t my_esp_send_msg(uint8_t *msg, uint8_t len, uint8_t timeout);
void mqtt_connect(uint8_t *client_id, uint8_t *username, uint8_t *password) {

	uint16_t index = 0;
	uint8_t *pack_buf = NULL;
	uint16_t payload_len;
	uint16_t temp, i;
	uint16_t total_len;
	uint8_t payload_size[4];
	uint8_t payload_size_cnt;

	uint16_t id_len = strlen(client_id);
	uint16_t user_len = strlen(username);
	uint16_t pass_len = strlen(password);

	payload_len = (id_len + 2) + (user_len + 2) + (pass_len + 2) + 10;
	total_len = payload_len + 1;
	payload_size_cnt = 0;
	do {
		temp = payload_len % 128;
		payload_len = payload_len / 128;
		if (payload_len > 0)
			temp |= 0x80;
		payload_size[payload_size_cnt] = temp;
		payload_size_cnt++;
	} while (payload_len > 0);

	total_len = total_len + payload_size_cnt;
	pack_buf = (uint8_t*) pvPortMalloc(total_len);     //

	pack_buf[0] = MQTT_CONNECT;   //固定报文头
	memcpy(&pack_buf[1], payload_size, payload_size_cnt);
	//printf("len:%d\r\n", total_len);
	index = 1 + payload_size_cnt;
	/*change_head*/
	pack_buf[index + 0] = 0x00;   //MSB  default  0x00
	pack_buf[index + 1] = 0x04;   //LSB  default  0x04
	pack_buf[index + 2] = 'M';   //  MQTT
	pack_buf[index + 3] = 'Q';  //  MQTT
	pack_buf[index + 4] = 'T';  //  MQTT
	pack_buf[index + 5] = 'T';  //  MQTT
	pack_buf[index + 6] = 0x04;   //协议版本
	pack_buf[index + 7] = 0xC2;   //可变报头第8个字节 ：使能用户名和密码校验，不使用遗嘱，不保留会话
	pack_buf[index + 8] = 0x00;   //可变报头第9个字节 ：保活时间高字节 0x00
	pack_buf[index + 9] = 60;   //可变报头第10个字节：保活时间低字节    60s

	/*     CLIENT_ID      */
	pack_buf[index + 10] = id_len >> 8;               			  	//客户端ID长度高字节
	pack_buf[index + 11] = id_len & 0x00FF;
	memcpy(&pack_buf[index + 12], client_id, id_len);
	/*     CLIENT_USER      */
	pack_buf[index + 12 + id_len] = user_len >> 8;               //客户端ID长度高字节
	pack_buf[index + 13 + id_len] = user_len & 0x00FF;
	memcpy(&pack_buf[index + 14 + id_len], username, user_len);

	/*     CLIENT_PASS      */
	pack_buf[index + 14 + id_len + user_len] = pass_len >> 8;    //客户端ID长度高字节
	pack_buf[index + 15 + id_len + user_len] = pass_len & 0x00FF;
	memcpy(&pack_buf[index + 16 + id_len + user_len], password, pass_len);
	//sci6_send_byte(pack_buf,total_len);
	mqtt_send_msg(pack_buf, total_len, 3);

	vPortFree(pack_buf);
	printf("----\r\n");

}

uint32_t decodeRemainLength(uint8_t *data) {
	uint32_t multiplier = 1;
	uint32_t value = 0;
	uint8_t encodedByte = 0;
	do {
		encodedByte = *data++;
		value += (encodedByte & 127) * multiplier;
		multiplier *= 128;
		if (multiplier > 128 * 128 * 128) {
			// throw Error(Malformed Remaining Length)
			// error 出错
			return value;
		}
	} while ((encodedByte & 128) != 0);
	return value;
	//输出已解码的剩余长度
	//printf("%u", value);
}

void mqtt_subscribe(uint8_t *topic_name, uint8_t Qos) {
	uint16_t i;
	uint16_t Total = 0;
	uint8_t *temp_buff = NULL;
	uint16_t Fixed_len = 2;                              //SUBSCRIBE报文中，固定报头长度=2
	uint16_t Variable_len = 2;                           //SUBSCRIBE报文中，可变报头长度=2
	uint16_t Payload_len = 2 + strlen(topic_name) + 1; //计算有效负荷长度 = 2字节(topic_name长度)+ topic_name字符串的长度 + 1字节服务等级
	Total = Fixed_len + Variable_len + Payload_len;
	temp_buff = (uint8_t*) pvPortMalloc(Total);

	temp_buff[0] = 0x82;                                    //第1个字节 ：固定0x82
	temp_buff[1] = Variable_len + Payload_len;             //第2个字节 ：可变报头+有效负荷的长度
	temp_buff[2] = 0x00;                              //第3个字节 ：报文标识符高字节，固定使用0x00
	temp_buff[3] = 0x01;		                      //第4个字节 ：报文标识符低字节，固定使用0x01
	temp_buff[4] = strlen(topic_name) / 256;            //第5个字节 ：topic_name长度高字节
	temp_buff[5] = strlen(topic_name) % 256;		    //第6个字节 ：topic_name长度低字节
	memcpy(&temp_buff[6], topic_name, strlen(topic_name)); //第7个字节开始 ：复制过来topic_name字串
	temp_buff[6 + strlen(topic_name)] = Qos;                  //最后1个字节：订阅等级
	printf(">>>\r\n");
	for (i = 0; i < Total; i++) {
		printf("%02X", temp_buff[i]);
	}
	printf("\r\n<<<\r\n");
	mqtt_send_msg(temp_buff, Total, 3);
	vTaskDelay(100);
	vPortFree(temp_buff);
}

void MQTT_PublishQs0(uint8_t *topic, uint8_t *msg) {
	uint8_t pub_msg_buf[128] = { 0 };
	uint16_t temp, total;
	uint8_t index_len = 1;
	uint8_t i;
	uint16_t temp_len;
	//uint16_t msg_len;
	printf("topic: %d \r\n", strlen(topic));
	printf("msg: %d \r\n", strlen(msg));

	//msg_len=strlen(msg);
	if (strlen(msg) < 1 || strlen(topic) < 1) {
		//	topic
		printf("MQTT_PublishQs0 msg len error \r\n");
		return;
	}

	total = 2 + strlen(msg) + strlen(topic);	//+strlen(msg);strlen(topic)
	printf("total: %d \r\n", total);
	do {
		temp = total % 128;
		total = total / 128;
		if (total > 0)
			temp |= 0x80;
		pub_msg_buf[index_len] = temp;
		index_len++;
	} while (total > 0);
	pub_msg_buf[0] = 0x30;
	temp_len = strlen(topic);
	pub_msg_buf[index_len + 0] = (uint8_t) (temp_len >> 8);
	pub_msg_buf[index_len + 1] = (uint8_t) temp_len;

	memcpy(&pub_msg_buf[index_len + 2], topic, temp_len);
	memcpy(&pub_msg_buf[index_len + 2 + temp_len], msg, strlen(msg));
	//	memcpy(&pub_msg_buf[index_len + temp_len + 2], msg, 14);
	total = index_len + 2 + temp_len + strlen(msg);
	mqtt_send_msg(pub_msg_buf, total, 3);
	for (i = 0; i < total; i++) {
		printf("%02X ", pub_msg_buf[i]);
	}
	printf("\r\n");

}

/*----------------------------------------------------------*/
/*函数名：PING报文，心跳包                                  */
/*参  数：无                                                */
/*返回值：无                                                */
/*----------------------------------------------------------*/
#if 0

void MQTT_PingREQ(void) {
	uint8_t temp_buff[2] = { 0 };
	temp_buff[0] = 0xC0;              //第1个字节 ：固定0xC0
	temp_buff[1] = 0x00;              //第2个字节 ：固定0x00
}

void MQTT_SendPingMsg(void) {
	uint8_t temp_buff[2] = { 0 };
	temp_buff[0] = 0xC0;              //第1个字节 ：固定0xC0
	temp_buff[1] = 0x00;              //第2个字节 ：固定0x00

	mqtt_send_msg(temp_buff, 2, 2);
}
#endif
