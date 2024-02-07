# MQTT_Library
MQTT library running on Renesas RX65N chip


This code is an MQTT library file,

Hardware platforms such as Renesas RX65N, ESP8266, etc

Use the AT command to enable ESP8266 to enter TCP transparent mode, and package it into MQTT library files in C language to drive ESP8266 to achieve publish/subscribe functionality



Step 1: Initialize hardware
Step 2: Access the router, make the ESP8266 access the Internet, and access the MQTT Broker
Step 3: Send AT command to put ESP8266 into TCP transparent transmission mode
Step 4: Send MQTT CONNECT message and connect to MQTT Broker
Step 4: If MQTT Broker access is successful, send an MQTT Subscription message to subscribe to the topic
Step 5: If the subscription is successful, we can use the MQTT client to send messages to the topics subscribed to by ESP8266, and ESP8266 can receive messages from the MQTT client
Step 6: We can send messages to the MQTT client by sending an MQTT Publish message
Step 7: In order to maintain a long connection between MQTT and Broker, we must send heartbeat packets at a scheduled time, with a timer cycle of 60 seconds


Program API interface description:

input client_id, client id, which depends on the MQTT Broker server
input username, username, depending on the MQTT Broker server
return void : return empty

Note: If the connection is successful, a message of 20 02 00 00 00 bytes will be returned. We can determine whether the receiving area contains this message to modify the connection status of MQTT,

void  mqtt_connect(uint8_t *client_id, uint8_t *username, uint8_t *password)




input the topic subscribed to by topicname
input QoS message level, only subscription topics with level 0 are implemented in the code

receiveeturn void and return empty

void mqtt_subscribe(uint8_t *topic_name, uint8_t Qos)





input the topic published by the topic
input the content published by msg
return void:  return empty

void MQTT_PublishQs0(uint8_t *topic, uint8_t *msg) 





























Program Run Block Diagram






