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
#define CLIENTID    "ExampleClientPub"
#define TOPIC       "MQTT"
/*#define PAYLOAD     "Hello World!"*/
#define QOS         1
#define TIMEOUT    3000L

#define path "/sys/class/leds/input2::capslock"

int gpio_get_value (char *sight, unsigned int *value)
{
  int fd, len;
  char buf[64];
  char ch;

  len = snprintf (buf, sizeof (buf), path "%s", sight);
  fd = open (buf, O_RDONLY);
  if (fd < 0)
    {
      perror ("gpio/get-value");
      return fd;
    }

  read (fd, &ch, 1);

  if (ch != '0')
    {
      *value = 101;
    }
  else
    {
      *value = 100;
    }
  close (fd);
  return 0;
}

#define path1 "/sys/class/leds/input2::numlock"

int gpio_get_value1 (char *sight, unsigned int *value)
{
  int fd, len;
  char buf[64];
  char ch;

  len = snprintf (buf, sizeof (buf), path1 "%s", sight);
  fd = open (buf, O_RDONLY);
  if (fd < 0)
    {
      perror ("gpio/get-value");
      return fd;
    }

  read (fd, &ch, 1);

  if (ch != '0')
    {
      *value = 201;
    }
  else
    {
      *value = 200;
    }
  close (fd);
  return 0;
}



int main (int argc, char *argv[])
{

  char str_payload[100];
  char str_payload1[100];
  int value,value1,ch;

  MQTTClient client;
  MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
  MQTTClient_message pubmsg = MQTTClient_message_initializer;
  MQTTClient_deliveryToken token;
  MQTTClient_create (&client, ADDRESS, CLIENTID,MQTTCLIENT_PERSISTENCE_NONE, NULL);
  conn_opts.keepAliveInterval = 20;
  conn_opts.cleansession = 1;
      int rc;

while(1){
      if ((rc = MQTTClient_connect (client, &conn_opts)) != MQTTCLIENT_SUCCESS)
	{
	  printf ("Failed to connect, return code %d\n", rc);
	  exit (EXIT_FAILURE);
	}
      gpio_get_value("/brightness",&value);
      sprintf(str_payload,"%d",value);
      pubmsg.payload = str_payload/*PAYLOAD*/;
      pubmsg.payloadlen = (int) strlen (/*PAYLOAD*/str_payload);
      pubmsg.qos = QOS;
      pubmsg.retained = 0;

      MQTTClient_publishMessage (client, TOPIC, &pubmsg, &token);
      printf ("Waiting for up to %d seconds for publication of %s\n" "on topic %s for client with ClientID: %s\n", (int) (TIMEOUT / 1000), /*PAYLOAD*/str_payload, TOPIC, CLIENTID);
      
      gpio_get_value1("/brightness",&value1);
      sprintf(str_payload1,"%d",value1);
      pubmsg.payload = str_payload1/*PAYLOAD*/;
      pubmsg.payloadlen = (int) strlen (/*PAYLOAD*/str_payload1);
      pubmsg.qos = QOS;
      pubmsg.retained = 0;

      MQTTClient_publishMessage (client, TOPIC, &pubmsg, &token);
      printf ("Waiting for up to %d seconds for publication of %s\n" "on topic %s for client with ClientID: %s\n", (int) (TIMEOUT / 1000), /*PAYLOAD*/str_payload1, TOPIC, CLIENTID);
     /************For Publish*************/
      rc = MQTTClient_waitForCompletion (client, token, TIMEOUT);
      printf ("Message with delivery token %d delivered\n", token);
}
      MQTTClient_disconnect (client, 10000);
      MQTTClient_destroy (&client);

  return rc;

}
