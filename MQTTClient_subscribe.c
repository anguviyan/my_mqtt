/*******************************************************************************
 * Copyright (c) 2012, 2017 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution. 
 *
 * The Eclipse Public License is available at 
 *   http://www.eclipse.org/legal/epl-v10.html
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
#define CLIENTID    "456258"
#define TOPIC       "SRI"
/*#define PAYLOAD     "Hello World!"*/
#define QOS         1
#define TIMEOUT     2000L


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
/**********************************************************************/


volatile MQTTClient_deliveryToken deliveredtoken;

void delivered (void *context, MQTTClient_deliveryToken dt)
{
  printf ("Message with token value %d delivery confirmed\n", dt);
  deliveredtoken = dt;
}

int msgarrvd (void *context, char *topicName, int topicLen,MQTTClient_message * message)
{
  int i;
  char *payloadptr;

 // char ar[64];

/*  printf ("Message arrived\n");
  printf ("     topic: %s\n", topicName);
  printf ("   message: ");*/

  payloadptr = message->payload;
  /*for(i=0;i<strlen(payloadptr);i++)
  {
	  printf("%c",payloadptr[i]);
  }*/
  //printf("%s\n",payloadptr);
/********************************My_Code********************************/
  
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
  }
  /*gpio_set_value(value);*/
/**********************************************************************/
  /*for (i = 0; i < message->payloadlen; i++)
    {
      putchar (*payloadptr++);
    }*/
  //putchar ('\n');

  /*switch(payloadptr)
  {
	  case 'a':
		  gpio_set_value(0);
		  break;
	 case 'b':
		  gpio_set_value(1);
		  break;
  }*/

  /*strcpy(ar,payloadptr);
  printf("%s\n",ar);*/

  MQTTClient_freeMessage (&message);
  MQTTClient_free (topicName);
  return 1;
}

void connlost (void *context, char *cause)
{
  printf ("\nConnection lost\n");
  printf ("     cause: %s\n", cause);
}

int main (int argc, char *argv[])
{
  MQTTClient client;
  MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
  int rc;
  int ch;

  MQTTClient_create (&client, ADDRESS, CLIENTID,MQTTCLIENT_PERSISTENCE_NONE, NULL);
  conn_opts.keepAliveInterval = 20;
  conn_opts.cleansession = 1;
  
  MQTTClient_setCallbacks (client, NULL, connlost, msgarrvd, delivered);

  if ((rc = MQTTClient_connect (client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
      printf ("Failed to connect, return code %d\n", rc);
      exit (EXIT_FAILURE);
    }
  //printf ("Subscribing to topic %s\nfor client %s using QoS%d\n\n""Press Q<Enter> to quit\n\n", TOPIC, CLIENTID, QOS);
  MQTTClient_subscribe (client, TOPIC, QOS);
  
  do
    {
      ch = getchar ();
    }
  while (ch != 'Q' && ch != 'q');

  MQTTClient_unsubscribe (client, TOPIC);
  MQTTClient_disconnect (client, 10000);
  MQTTClient_destroy (&client);
  
  return rc;
}
