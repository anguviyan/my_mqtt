/*******************************************************************************
 * Copyright (c) 2012, 2017 IBM Corp.
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
 *    Ian Craggs - initial contribution
 *******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MQTTClient.h"
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

/*#define ADDRESS     "tcp://localhost:1883"*/
#define ADDRESS     "tcp://broker.hivemq.com:1883"
#define CLIENTID    "ExampleClientPub"
#define TOPIC       "MQTT"
/*#define PAYLOAD     "Hello World!"*/
#define QOS         0
#define TIMEOUT     10000L

char str;

#define path "/sys/class/leds/input2::capslock"

int gpio_get_value( char *sight,unsigned int *value)
{
	int fd,len;
	char buf[64];
	char ch;

	len = snprintf(buf,sizeof(buf),path "%s" ,sight);
	fd = open(buf,O_RDONLY);
	if(fd < 0)
	{
		perror("gpio/get-value");
		return fd;
	}

	read(fd,&ch,1);

	if(ch != '0')
	{
		*value = 1;
	}
	else
	{
		*value = 0;
	}

	close(fd);
}

/********************************My_Code********************************/
#define path "/sys/class/leds/input2::capslock"
int gpio_set_value (unsigned int value)
{
	int fd, len;
	char buf[64];

	len = snprintf (buf, sizeof (buf), path "/brightness");
	fd = open (buf, O_WRONLY);

	if (fd < 0)
	{
		perror ("gpio/set-value");
		return fd;
	}

	if (value)
		write (fd, "1", 2);
	else
		write (fd, "0", 2);
	close (fd);
	return 0;
}
/**********************************************************************/
#define path1 "/sys/class/leds/input2::numlock"
int gpio_set_value1 (unsigned int value)
{
	int fd, len;
	char buf[64];

	len = snprintf (buf, sizeof (buf), path1 "/brightness");
	fd = open (buf, O_WRONLY);
	if (fd < 0)
	{
		perror ("gpio/set-value");
		return fd;
	}

	if (value)
		write (fd, "1", 2);
	else
		write (fd, "0", 2);
	close (fd);
	return 0;
}
volatile MQTTClient_deliveryToken deliveredtoken;

void delivered(void *context, MQTTClient_deliveryToken dt)
{
    printf("Message with token value %d delivery confirmed\n", dt);
    deliveredtoken = dt;
}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    int i;
    char* payloadptr;

    printf("Message arrived\n");
    printf("     topic: %s\n", topicName);
    printf("   message: ");

/*    payloadptr = message->payload;
    int value = atoi(payloadptr);
    printf("The Message is %d\n",value);
    switch(value)
    {
	    case 100:
		    gpio_set_value(0);
		    break;
	case 101:
		    gpio_set_value(1);
		    break;
	case 200:
		    gpio_set_value1(0);
		    break;
	case 201:
		    gpio_set_value1(1);
		    break;
    }*/
    for(i=0; i<message->payloadlen; i++)
    {
        putchar(*payloadptr++);
    }
    putchar('\n');
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

void connlost(void *context, char *cause)
{
    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause);
}

int main(int argc, char* argv[])
{
    int rc,value;

    int ch;//my_line

    char str_payload[100];

    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;


    MQTTClient_create(&client, ADDRESS, CLIENTID,MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
while(1){
    MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }
    /*gpio_get_value("/brightness",&value);
    sprintf(str_payload,"%d",value);*/
    //pubmsg.payload = str_payload/*PAYLOAD*/;
    //pubmsg.payloadlen = (int)strlen(str_payload/*PAYLOAD*/);
   /* pubmsg.qos = QOS;
    pubmsg.retained = 0;
    deliveredtoken = 0;

    MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);*/

    //printf("Waiting for publication of %s\n""on topic %s for client with ClientID: %s\n",/*PAYLOA*/str_payload, TOPIC, CLIENTID);

    printf("Subscribing to topic %s\nfor client %s using QoS%d\n\n""Press Q<Enter> to quit\n\n", TOPIC, CLIENTID, QOS);//my_line

    MQTTClient_subscribe(client, TOPIC, QOS);//my_line
    do//my_line
    {//my_line
	    ch = getchar();//my_line
    } while(ch!='Q' && ch != 'q');//my_line

    while(deliveredtoken != token);
    MQTTClient_unsubscribe(client, TOPIC);//my_line
}
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    return rc;

}
