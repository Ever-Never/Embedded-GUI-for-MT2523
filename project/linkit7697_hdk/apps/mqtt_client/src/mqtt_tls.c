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

#define TEST_MQTT_MOSQUITTO_CA_CRT									    \
"-----BEGIN CERTIFICATE-----\r\n" 										\
"MIIC8DCCAlmgAwIBAgIJAOD63PlXjJi8MA0GCSqGSIb3DQEBBQUAMIGQMQswCQYD\r\n"	\
"VQQGEwJHQjEXMBUGA1UECAwOVW5pdGVkIEtpbmdkb20xDjAMBgNVBAcMBURlcmJ5\r\n"	\
"MRIwEAYDVQQKDAlNb3NxdWl0dG8xCzAJBgNVBAsMAkNBMRYwFAYDVQQDDA1tb3Nx\r\n"	\
"dWl0dG8ub3JnMR8wHQYJKoZIhvcNAQkBFhByb2dlckBhdGNob28ub3JnMB4XDTEy\r\n"	\
"MDYyOTIyMTE1OVoXDTIyMDYyNzIyMTE1OVowgZAxCzAJBgNVBAYTAkdCMRcwFQYD\r\n"	\
"VQQIDA5Vbml0ZWQgS2luZ2RvbTEOMAwGA1UEBwwFRGVyYnkxEjAQBgNVBAoMCU1v\r\n"	\
"c3F1aXR0bzELMAkGA1UECwwCQ0ExFjAUBgNVBAMMDW1vc3F1aXR0by5vcmcxHzAd\r\n"	\
"BgkqhkiG9w0BCQEWEHJvZ2VyQGF0Y2hvby5vcmcwgZ8wDQYJKoZIhvcNAQEBBQAD\r\n"	\
"gY0AMIGJAoGBAMYkLmX7SqOT/jJCZoQ1NWdCrr/pq47m3xxyXcI+FLEmwbE3R9vM\r\n"	\
"rE6sRbP2S89pfrCt7iuITXPKycpUcIU0mtcT1OqxGBV2lb6RaOT2gC5pxyGaFJ+h\r\n"	\
"A+GIbdYKO3JprPxSBoRponZJvDGEZuM3N7p3S/lRoi7G5wG5mvUmaE5RAgMBAAGj\r\n"	\
"UDBOMB0GA1UdDgQWBBTad2QneVztIPQzRRGj6ZHKqJTv5jAfBgNVHSMEGDAWgBTa\r\n"	\
"d2QneVztIPQzRRGj6ZHKqJTv5jAMBgNVHRMEBTADAQH/MA0GCSqGSIb3DQEBBQUA\r\n"	\
"A4GBAAqw1rK4NlRUCUBLhEFUQasjP7xfFqlVbE2cRy0Rs4o3KS0JwzQVBwG85xge\r\n"	\
"REyPOFdGdhBY2P1FNRy0MDr6xr+D2ZOwxs63dG1nnAnWZg7qwoLgpZ4fESPD3PkA\r\n"	\
"1ZgKJc2zbSQ9fCPxt2W3mdVav66c6fsb7els2W2Iz7gERJSX\r\n"					\
"-----END CERTIFICATE-----"


#define TEST_MQTT_CA_CRT 												\
"-----BEGIN CERTIFICATE-----\r\n" 										\
"MIIDXTCCAkWgAwIBAgIJANbHvHQmGQktMA0GCSqGSIb3DQEBCwUAMEUxCzAJBgNV\r\n"	\
"BAYTAkFVMRMwEQYDVQQIDApTb21lLVN0YXRlMSEwHwYDVQQKDBhJbnRlcm5ldCBX\r\n"	\
"aWRnaXRzIFB0eSBMdGQwHhcNMTUwOTIzMDg0NzQyWhcNMTYwOTIyMDg0NzQyWjBF\r\n"	\
"MQswCQYDVQQGEwJBVTETMBEGA1UECAwKU29tZS1TdGF0ZTEhMB8GA1UECgwYSW50\r\n"	\
"ZXJuZXQgV2lkZ2l0cyBQdHkgTHRkMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIB\r\n"	\
"CgKCAQEA5r25cngOtptvIGMfupTf0LfNAU3JWFqx1zOIJfgSC/6oP9AvKkUUjG7M\r\n"	\
"vY5cv2TO7ycoZ7Ami/GhCzODSWoZE/nP3TSHmE7idmpOAP5RLiNkQdUtUJO8fTQM\r\n"	\
"aNXZgA3Wn9AAsItzIKAnpXA5w0drOpnHHZC5NEmZAs6ONj+/jGAQLE4AgEkQpzE7\r\n"	\
"cGEiFdesCVMgETqZVpAGlZjoA6gIEsmoYbXTA9oJZu+1DMsr1NT0BLulKejsxfJf\r\n"	\
"v3ecd3+T3OlYNTqWvlW4FWxgjhaHKHOEQZDllnLEl109GgoinB4F40mOh1DprUs5\r\n"	\
"SWoZUgNnl1ERSbh8oEWvfvmzuH2++QIDAQABo1AwTjAdBgNVHQ4EFgQU+IU88yhC\r\n"	\
"kbjH5/BleMmBrfHC4qYwHwYDVR0jBBgwFoAU+IU88yhCkbjH5/BleMmBrfHC4qYw\r\n"	\
"DAYDVR0TBAUwAwEB/zANBgkqhkiG9w0BAQsFAAOCAQEA1WQfCKAM8Ga4AVEGHgcI\r\n"	\
"TCQrLBr7/JY/HALkDUXry/rmpqQ/sJKJM1wfmaVIfkBjcxzwpU4Pf2YRhmm+7bd1\r\n"	\
"lLby/dF9nILcQKmzXPdT9USXwpzuAYA6poRPYfl++S/d4u3z5HtIjWjBec5yKqyn\r\n"	\
"P3xQ1W2l6dRHz/A7H50h9Btfweavp/GyTW3DWhBEtcyAK4CkxBYnX3bdlCBCZXr3\r\n"	\
"N3n/B54vhJYKhvN0fr2x8spQgRGn8YfGVl7AiLKMDkNa/qYig+DnbtoP944mnBpF\r\n"	\
"+NiqWU2LNxXja92eOZP0gEuN+q52n27it6jjjNsB9TeTsXOaZ4Hj9F2HR/3FxkKg\r\n"	\
"vg==\r\n"																\
"-----END CERTIFICATE-----"


#define TEST_MQTT_CLI_CRT												\
"-----BEGIN CERTIFICATE-----\r\n" 										\
"MIIDFzCCAf8CCQC+PCz8A/3UYzANBgkqhkiG9w0BAQsFADBFMQswCQYDVQQGEwJB\r\n"	\
"VTETMBEGA1UECAwKU29tZS1TdGF0ZTEhMB8GA1UECgwYSW50ZXJuZXQgV2lkZ2l0\r\n"	\
"cyBQdHkgTHRkMB4XDTE1MDkyMzA4NTEzMVoXDTE2MDkyMjA4NTEzMVowVjELMAkG\r\n"	\
"A1UEBhMCQVUxEzARBgNVBAgMClNvbWUtU3RhdGUxITAfBgNVBAoMGEludGVybmV0\r\n"	\
"IFdpZGdpdHMgUHR5IEx0ZDEPMA0GA1UEAwwGdGVzdGVyMIIBIjANBgkqhkiG9w0B\r\n"	\
"AQEFAAOCAQ8AMIIBCgKCAQEA9HRRfEYgPYbURY4hYHR/RNNLN0BgQ6y6ZLO+rqGm\r\n"	\
"lSrvztNrnUyMjhvhq3h95BQPfvNbZlMOMyCnM98IxFaNxRAbza9W1Eoup37zRxTn\r\n"	\
"3fb7tnOcxUd4o9bp9S5NCXoUOq2QjCP+dpgIyiDa+LXk8idEQg4PU8kGonjXSXdr\r\n"	\
"GbfxmUyNbj6UY7wRod+YqBhu41OeXjwYIqXne2pKMKibZ+PcJazl98798OTVV7mr\r\n"	\
"Uc7c1qGYBKFaY9EGo3l739ye+40eEFpkN5KiWubWjsZRtvx/iQBTVQHh8Fz+dlP7\r\n"	\
"pYaYEm/yuCwITIrJFFprAL2gbKlLjAvEyRv3EqAxVJsF5wIDAQABMA0GCSqGSIb3\r\n"	\
"DQEBCwUAA4IBAQCqrb356nlAqIAgmjhPo8pLIvSozPF2OW9j/PW4hGqxLEi3WKhV\r\n"	\
"6SCdC5nJZQTfG8XhW5g4mJJBQQlPcXjgwitIOQC9nr/6G9JdKHJQsKWIJBA25WFH\r\n"	\
"M2QjqQOB1fMVyvNfQIr9yEuahXGWju/fTUO7g6cJ2qo9vDAdct7ngcIAfLW2ApO2\r\n"	\
"HhqUYWAP+GUM4kWM/CTErRAb+SYkaHEc+P4S/i049Y5X1vpAdAw0MDJVPBlqiPh2\r\n"	\
"nF4QaGUByUpq2VOKbCEFmhsg9ECt1kPCF+lPaJp26XvhT6kvPwZNC45i06EIaBIt\r\n"	\
"aCOWvHY8fzmcxGVJJh/Av0IyhkFzEVWZkveJ\r\n"								\
"-----END CERTIFICATE-----"


#define TEST_MQTT_CLI_KEY												\
"-----BEGIN RSA PRIVATE KEY-----\r\n" 									\
"Proc-Type: 4,ENCRYPTED\r\n" 											\
"DEK-Info: DES-EDE3-CBC,6B963B16AB4BB8BE\r\n" 							\
"\r\n" 																	\
"vVLDh0hopgNm5q88Y4Bc3Puum89hNcBhvP17wF3ZHamIUqkHbyFGpU7LOt+XaRnX\r\n"	\
"Sdd2wa0U/mDnNYT1PEytf+SQWhMNemUOCGrQwlE9gS1+ovg4qCpVS0gFxx2O2bOw\r\n"	\
"lCUjFkiMVQ6IOn/Zg7kqpCwOTUF/yvEFbfU7Gh6f429PzoCg5S8AKey+ZPvdt/kx\r\n"	\
"T/8JKe2n57tiGRTA6OIwjYVa9qmmbK+2m4ky47KPMaMSNGuxzIoMM98khJ84VmGO\r\n"	\
"LEbL+L3GODGrdd8tfZIDYhWeCDeAzKbdZhLEHmKw/ezW4+tBYNvXmxJo2i0NrgkR\r\n"	\
"zm3xGgQTXrXMfaiyIP+oBlrj/bij5M/SIAJn6fg7CROQs3sablr74bKAB6ZLzCf8\r\n"	\
"hDoef8WM/zmYp46pEBnrXVEZEIkcD9LwDwJ5mlATOtDt9U2JkShDkA/to0RTlxuf\r\n"	\
"5qvnBaOQkuvm+AVMkO8i6SNLt++eUMFNF5ZpqsjkN6nCZEWZlC0R6mbJeMN5K6nZ\r\n"	\
"bwbJSjE1k8mEisbfIT5ZtyHmygNg2AV90jQGGHSufwIfh94enlBT/GxCyDeVyjCe\r\n"	\
"hGY9JdkFkSJ9yN/RRmdOwA2oHenwQOoSP/rf44ZMNuGlDE6gHl/rR19HoW4xPl0s\r\n"	\
"HBe3t2da1tVotG+zSaQ7Qme7heD1uo26UoV4rXCNlfzg1XpJiZqiIX/rbgtZhADs\r\n"	\
"rwrQV9QEH74Vju39WRBc7s8HGmc5PZ6EQx/Mc5LFAzMBOHVZexAy0sWLM4FMX3Nm\r\n"	\
"h1rXtV3CY1A6q5SMuHaMtumnOQX/Y0ugqYBM8DGozls6iqCbLljg+8cAL+BWy3cs\r\n"	\
"8KdMmKRkzLmIYKC3E4n9l8mFngIvrVVMRnKBo61hzH9DTMYGL++gSHCxiyan242X\r\n"	\
"tjDgYLXv3/PjNnwWoUAud+bqfTGyULXvnBpQMItLoDnEO/sqivcqC1xwth9uCdsy\r\n"	\
"FCyVxTtSouTEmCzF7QCiFQjozHOa74uCtPKdmK6+uVIrcZeLL6fEvtI/hV63tnwh\r\n"	\
"xqxya3SqXYwQRwe5pyrp9p19e1PEf85JdnYG1DoMvekYHPJBjLF/SbAwTc4BafKG\r\n"	\
"LMxliyZhUh96BCrwN4BTXQ2rJqAnZuXHTzfNvACUdK3QgcdHBTrLgEg2L+a1oYOe\r\n"	\
"aF5wDyPfuEP1Xzz3Eb6MN6NsHurClqWzwi+lXMwoGgQ3yzMHnKlwXkqjt+ds1+vv\r\n"	\
"wCBSy0Pq3Y6WVE3bQ19K2lf1iBg1eEpcxNFWBS/G9z1c/mzRYrSdmJvJ2SopHY2D\r\n"	\
"hfWaxAurWobSQDueb6pijWfLfG3vWqjOGqH/CuD4lgU1rzzwCWx9HmPQH3Y+1CDP\r\n"	\
"TVqSwHea6FjhQtZjgaah4ixOVV1M1On/7UFFc5/gEg3DbyzV8ZdW87v7PZyssWRA\r\n"	\
"SwSBBIycl+BA2F8OlECPGLqYLvtFlfZRuNsRf0adhlKf2e46SO59lPXsKQp0avnr\r\n"	\
"P7biV5YpFVSJpnGkCxw4XPV3ZHQLNzIWRxc6bWw3b62P+Lf8677ohhEoRLTYMpAq\r\n"	\
"qhTeLOxNSOLPk17UeX/Wh8f6lfDVQRj70ZH5P4m8oFQMJsY8cq1Iww==\r\n"			\
"-----END RSA PRIVATE KEY-----"


static const char mqtt_ca_cert[] = TEST_MQTT_MOSQUITTO_CA_CRT;
static const size_t mqtt_ca_crt_len  = sizeof( mqtt_ca_cert );

#define MQTT_SERVER		"test.mosquitto.org"
#define MQTT_PORT		"8883"
#define MQTT_TOPIC		"7697test"
#define MQTT_CLIENT_ID	"mqtt-7697-client-ssl"
#define MQTT_MSG_VER	"0.50"

static int arrivedcount = 0;

log_create_module(mqtt_tls, PRINT_LEVEL_INFO);


/**
* @brief          MQTT message RX handle
* @param[in]      MQTT received message data
* @return         None
*/
static void messageArrived(MessageData *md)
{
    MQTTMessage *message = md->message;

    LOG_I(mqtt_tls, "Message arrived: qos %d, retained %d, dup %d, packetid %d\n", 
        message->qos, message->retained, message->dup, message->id);
    LOG_I(mqtt_tls, "Payload %d.%s\n", (size_t)(message->payloadlen), (char *)(message->payload));
    ++arrivedcount;
}

/**
* @brief          MQTT client example entry function
* @return         None
*/
extern int mqtt_client_example_ssl(void)
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
    
    const char mqtt_cli_cert[] = TEST_MQTT_CLI_CRT;
    const char mqtt_cli_key[] = TEST_MQTT_CLI_KEY;
    const char mqtt_cli_pwd[] = "11111111";
    const size_t mqtt_cli_crt_len  = sizeof( mqtt_cli_cert );
    const size_t mqtt_cli_key_len  = sizeof( mqtt_cli_key );
    const size_t mqtt_cli_pwd_len  = sizeof( mqtt_cli_pwd ) - 1;

    //Initialize MQTTnetwork structure
    NewNetwork(&n);

    //Connect to remote server
    LOG_I(mqtt_tls, "TLS Connect to %s:%s\n", MQTT_SERVER, MQTT_PORT);
    rc = TLSConnectNetwork(&n, MQTT_SERVER, MQTT_PORT, mqtt_ca_cert, mqtt_ca_crt_len,
    		mqtt_cli_cert, mqtt_cli_crt_len,
    		mqtt_cli_key, mqtt_cli_key_len,
    		mqtt_cli_pwd, mqtt_cli_pwd_len);

    if (rc != 0) {
        LOG_I(mqtt_tls, "TCP connect fail,status -%4X\n", -rc);
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
        LOG_I(mqtt_tls, "MQTT connect failed,status -%4X\n", -rc);
        return rc;
    }

    LOG_I(mqtt_tls, "Subscribing to %s\n", topic);
    rc = MQTTSubscribe(&c, topic, QOS1, messageArrived);
    LOG_I(mqtt_tls, "Client Subscribed %d\n", rc);

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
        LOG_I(mqtt_tls, "The return from unsubscribe was %d\n", rc);
    }
    LOG_I(mqtt_tls, "MQTT unsubscribe done\n");

    if ((rc = MQTTDisconnect(&c)) != 0) {
        LOG_I(mqtt_tls, "The return from disconnect was %d\n", rc);
    }
    LOG_I(mqtt_tls, "MQTT disconnect done\n");

    n.disconnect(&n);
    LOG_I(mqtt_tls, "Network disconnect done\n");
    LOG_I(mqtt_tls, "example project test success.");
    return 0;
}
