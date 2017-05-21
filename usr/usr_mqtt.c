#include "usr_mqtt.h"
#include <hsf.h>
#include <hfthread.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#include "MQTTHSF.h"
#include "MQTTClient.h"
#include "ringbuf.h"

unsigned char sendbuf[512], readbuf[512];
char msg_payload[256];
MQTTClient client_hf;
MQTTMessage msg_hf;
RingBuffer rb;
uint8_t rbb_buff[1024]={0};
hfthread_hande_t mqtt_usr,mqtt_task;
/**
  * Deserializes the supplied (wire) buffer into an ack
  * @param packettype returned integer - the MQTT packet type
  * @param dup returned integer - the MQTT dup flag
  * @param packetid returned integer - the MQTT packet identifier
  * @param buf the raw buffer data, of the correct length determined by the remaining length field
  * @param buflen the length in bytes of the data in the supplied buffer
  * @return error code.  1 is success, 0 is failure
  */
void messageArrived(MessageData* data)
{
	u_printf("Message arrived on topic %.*s: %.*s\n", data->topicName->lenstring.len, data->topicName->lenstring.data,
		data->message->payloadlen, data->message->payload);  
}
/**
  * Deserializes the supplied (wire) buffer into an ack
  * @param packettype returned integer - the MQTT packet type
  * @param dup returned integer - the MQTT dup flag
  * @param packetid returned integer - the MQTT packet identifier
  * @param buf the raw buffer data, of the correct length determined by the remaining length field
  * @param buflen the length in bytes of the data in the supplied buffer
  * @return error code.  1 is success, 0 is failure
  */
void uart_to_mqtt(void* arg)
{

	int recv_num=0;
	fd_set rset;
	char recv[32]={0};
	for(;;)
	{
		msleep(1000);
//		if (FD_ISSET(fd, &rset))
//		{
//			if((recv_num=recv(sockb_ok,recv,sizeof(recv),0))>0)
//			{
//				u_printf("recv data bytes:%d\n",recv_num);
//			}
//			else
//			{
//				close(sockb_ok);
//				sockb_ok=-1;
//				u_printf("tcp disconnectd!\n");
//			}
		//}		
	}
	
//	hfuart_handle_t huart0;
//	char *buf;
//	int recv_bytes;
//	int rc=0;
//	char *str="hello str";
//	message.qos = 0;
//	message.retained = 0;
//	message.payload = payload;
//	huart0 = hfuart_open(0);
//	if(huart0==NULL)
//	{
//		u_printf("open uart0 fail\n");
//		goto exit_thread;
//	}
//	buf = (char*)hfmem_malloc(1000);
//	if(buf==NULL)
//	{
//		u_printf("memory alloc fail\n");
//		goto exit_thread;
//	}
//	for(;;)
//	{
//		//u_printf("uart0 start ok\n");
////		recv_bytes = hfuart_recv(huart0,buf,1000,1000);
////		if(recv_bytes>0)
////		{
////			  u_printf("uart0 recv ok recv_bytes==%d\n",recv_bytes);
////			  memcpy(message.payload,buf,recv_bytes);
////				message.payloadlen = recv_bytes;
////			  u_printf("MQTTPublish start\n");
////				if ((rc = MQTTPublish(&client, "hsf/test/mac", &message)) != 0)
////				{
////				  u_printf("MQTTPublish fail\n");
////				}
////				u_printf("MQTTPublish over\n");
////				#if !defined(MQTT_TASK)
////						if ((rc = MQTTYield(&client, 1000)) != 0)
////            {
////							//msleep(1);
////						}
////				#endif      
////		}
//		
//	}
//  exit_thread:
//		if(buf!=NULL)
//		{
//			hfmem_free(buf);
//		}
//		hfuart_close(huart0);
//		hfthread_destroy(NULL);
	return ;
}
/**
  * Deserializes the supplied (wire) buffer into an ack
  * @param packettype returned integer - the MQTT packet type
  * @param dup returned integer - the MQTT dup flag
  * @param packetid returned integer - the MQTT packet identifier
  * @param buf the raw buffer data, of the correct length determined by the remaining length field
  * @param buflen the length in bytes of the data in the supplied buffer
  * @return error code.  1 is success, 0 is failure
  */
void uartInitAndStart(void)
{
	hfthread_create(uart_to_mqtt, "uart_to_mqtt", 2048, NULL, HFTHREAD_PRIORITIES_NORMAL, NULL,NULL); 
}
/**
  * Deserializes the supplied (wire) buffer into an ack
  * @param packettype returned integer - the MQTT packet type
  * @param dup returned integer - the MQTT dup flag
  * @param packetid returned integer - the MQTT packet identifier
  * @param buf the raw buffer data, of the correct length determined by the remaining length field
  * @param buflen the length in bytes of the data in the supplied buffer
  * @return error code.  1 is success, 0 is failure
  */
void mqttThreadStart(void* arg)
{
	Network network;
	int rc = 0;
	char * str="hello world\n";
	MQTTPacket_connectData connectData = MQTTPacket_connectData_initializer;
	NetworkInit(&network);
	MQTTClientInit(&client_hf, &network, 30000, sendbuf, sizeof(sendbuf), readbuf, sizeof(readbuf));
	connectData.MQTTVersion = 4;
	connectData.clientID.cstring = "hsf_sample";
	
	u_printf("MQTTConnect start\n");
	
	if ((rc = MQTTConnect(&client_hf, &connectData)) != 0)
	{
		u_printf("MQTTConnect fail\n");
	}
	
	u_printf("MQTTStartTask start\n");	
	if ((rc = MQTTStartTask(&client_hf)) != HF_SUCCESS)
	{
		u_printf("MQTTStartTask fail\n");
	}	
	
	u_printf("MQTTSubscribe start\n");	
	if ((rc = MQTTSubscribe(&client_hf, "hsf/test/mac", 0, messageArrived)) != 0)
	{
		u_printf("MQTTSubscribe fail\n");
	}
	
	u_printf("MQTTSubscribe OK\n");
	
	for(;;)
	{
			msleep(1000);
			msg_hf.qos = 0;
			msg_hf.retained = 0;
		  msg_hf.payload = msg_payload;
			memcpy(msg_hf.payload,str,strlen(str));
			msg_hf.payloadlen = strlen(str);
			if ((rc = MQTTPublish(&client_hf, "hsf/test/send", &msg_hf)) != 0)
			{
				
			}
	}
}
/**
  * Deserializes the supplied (wire) buffer into an ack
  * @param packettype returned integer - the MQTT packet type
  * @param dup returned integer - the MQTT dup flag
  * @param packetid returned integer - the MQTT packet identifier
  * @param buf the raw buffer data, of the correct length determined by the remaining length field
  * @param buflen the length in bytes of the data in the supplied buffer
  * @return error code.  1 is success, 0 is failure
  */
void mqttInitAndSubcribe(void)
{
	hfthread_create(mqttThreadStart, "mqttThreadStart", 1024, NULL, HFTHREAD_PRIORITIES_MID, &mqtt_usr,NULL); 
}
/**
  * Deserializes the supplied (wire) buffer into an ack
  * @param packettype returned integer - the MQTT packet type
  * @param dup returned integer - the MQTT dup flag
  * @param packetid returned integer - the MQTT packet identifier
  * @param buf the raw buffer data, of the correct length determined by the remaining length field
  * @param buflen the length in bytes of the data in the supplied buffer
  * @return error code.  1 is success, 0 is failure
  */
void destroyMqttThread(void)
{
	hfthread_destroy(mqtt_usr); 
	hfthread_destroy(mqtt_task); 	
  MutexFree(&(client_hf.mutex));
}
/**
  * Deserializes the supplied (wire) buffer into an ack
  * @param packettype returned integer - the MQTT packet type
  * @param dup returned integer - the MQTT dup flag
  * @param packetid returned integer - the MQTT packet identifier
  * @param buf the raw buffer data, of the correct length determined by the remaining length field
  * @param buflen the length in bytes of the data in the supplied buffer
  * @return error code.  1 is success, 0 is failure
  */
int sockbConnect(char * host,int port)
{
	char rsp[128];
	char *words[5]={NULL};
	
	hfat_send_cmd("AT+SOCKB\r\n",sizeof("AT+SOCKB\r\n"),rsp,sizeof(rsp));	
	if(hfat_get_words(rsp,words, 5)>0)
	{
		u_printf("\nresult:%s\ntcp:%s\nport:%s\nIP:%s\n",words[0],words[1],words[2],words[3]);
		if(0!=strncmp(words[0],"+ok",strlen("ok"))||0!=strncmp(words[1],"TCP",strlen("TCP"))||0!=strncmp(words[2],"1883",strlen("1883"))
			 ||0!=strncmp(words[3],"iotplc.cn",strlen("iotplc.cn"))
		  )
		{
				u_printf("set tcp para \n");			  
		  	hfat_send_cmd("AT+SOCKB=TCP,1883,iotplc.cn\r\n",sizeof("AT+SOCKB=TCP,1883,iotplc.cn\r\n"),rsp,sizeof(rsp));	
		}
		else
		{
		   u_printf("para has been setted \n");	
		}
	} 
	hfat_send_cmd("AT+TCPDISB=on\r\n",sizeof("AT+TCPDISB=on\r\n"),rsp,sizeof(rsp));
	msleep(100);
	
	return hfnet_socketb_fd();
}
