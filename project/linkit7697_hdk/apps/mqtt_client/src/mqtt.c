/*******************************************************************************
 * Copyright (c) 2014 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Allan Stockdill-Mander/Ian Craggs - initial API and implementation and/or initial documentation
 *******************************************************************************/


/* Includes ------------------------------------------------------------------*/

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "wifi_api.h"

#include "MQTTClient.h"
#include "mqtt.h"
#include "syslog.h"


#define MQTT_SERVER		"iot.eclipse.org"
#define MQTT_PORT		"1883"
#define MQTT_TOPIC		"7697test"
#define MQTT_CLIENT_ID	"mqtt-7697-client"
#define MQTT_MSG_VER	"0.50"

static int arrivedcount = 0;

log_create_module(mqtt, PRINT_LEVEL_INFO);


/**
* @brief          MQTT message RX handle
* @param[in]      MQTT received message data
* @return         None
*/
static void messageArrived(MessageData *md)
{
    MQTTMessage *message = md->message;

    LOG_I(mqtt, "Message arrived: qos %d, retained %d, dup %d, packetid %d\n", 
        message->qos, message->retained, message->dup, message->id);
    LOG_I(mqtt, "Payload %d.%s\n", (size_t)(message->payloadlen), (char *)(message->payload));

    ++arrivedcount;
}

/**
* @brief          MQTT client example entry function
* @return         None
*/
int mqtt_client_example(void)
{
    int rc = 0;
    unsigned char msg_buf[100];     //Buffer for outgoing messages, such as unsubscribe.
    unsigned char msg_readbuf[100]; //Buffer for incoming messages, such as unsubscribe ACK.
    char buf[100];                  //Buffer for application payload.

    Network n;  //TCP network
    Client c;   //MQTT client
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    char *topic = MQTT_TOPIC;
    MQTTMessage message;

    //Initialize MQTT network structure
    NewNetwork(&n);

    //Connect to remote server
    LOG_I(mqtt, "Connect to %s:%s\n", MQTT_SERVER, MQTT_PORT);
    rc = ConnectNetwork(&n, MQTT_SERVER, MQTT_PORT);

    if (rc != 0) {
        LOG_I(mqtt, "TCP connect failed,status -%4X\n", -rc);
        return rc;
    }

    //Initialize MQTT client structure
    MQTTClient(&c, &n, 12000, msg_buf, 100, msg_readbuf, 100);

    //The packet header of MQTT connection request
    data.willFlag = 0;
    data.MQTTVersion = 3;
    data.clientID.cstring = MQTT_CLIENT_ID;
    data.username.cstring = NULL;
    data.password.cstring = NULL;
    data.keepAliveInterval = 10;
    data.cleansession = 1;

    //Send MQTT connection request to the remote MQTT server
    rc = MQTTConnect(&c, &data);

    if (rc != 0) {
        LOG_I(mqtt, "MQTT connect failed,status -%4X\n", -rc);
        return rc;
    }
    
    LOG_I(mqtt, "Subscribing to %s\n", topic);
    rc = MQTTSubscribe(&c, topic, QOS1, messageArrived);
    LOG_I(mqtt, "Client Subscribed %d\n", rc);

    // QoS 0
    sprintf(buf, "Hello World! QoS 0 message from app version %s\n", MQTT_MSG_VER);
    message.qos = QOS0;
    message.retained = false;
    message.dup = false;
    message.payload = (void *)buf;
    message.payloadlen = strlen(buf) + 1;
    rc = MQTTPublish(&c, topic, &message);

    while (arrivedcount < 1) {
        MQTTYield(&c, 1000);
    }

    // QoS 1
    sprintf(buf, "Hello World! QoS 1 message from app version %s\n", MQTT_MSG_VER);
    message.qos = QOS1;
    message.payloadlen = strlen(buf) + 1;
    rc = MQTTPublish(&c, topic, &message);
    while (arrivedcount < 2) {
        MQTTYield(&c, 1000);
    }

    // QoS 2
    sprintf(buf, "Hello World! QoS 2 message from app version %s\n", MQTT_MSG_VER);
    message.qos = QOS2;
    message.payloadlen = strlen(buf) + 1;
    rc = MQTTPublish(&c, topic, &message);
    while (arrivedcount < 3) {
        MQTTYield(&c, 1000);
    }

    if ((rc = MQTTUnsubscribe(&c, topic)) != 0) {
        LOG_I(mqtt, "The return from unsubscribe was %d\n", rc);
    }
    LOG_I(mqtt, "MQTT unsubscribe done\n");

    if ((rc = MQTTDisconnect(&c)) != 0) {
        LOG_I(mqtt, "The return from disconnect was %d\n", rc);
    }
    LOG_I(mqtt, "MQTT disconnect done\n");

    n.disconnect(&n);
    LOG_I(mqtt, "Network disconnect done\n");

    return 0;
    //LOG_I(mqtt, "example project test success.");

}



