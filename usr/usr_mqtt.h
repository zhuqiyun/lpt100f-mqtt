#ifndef  __USR_MQTT_H__
#define  __USR_MQTT_H__

  #include<hsf.h>
  #include"ringbuf.h"	
	
extern hfthread_hande_t mqtt_usr,mqtt_task;
	extern RingBuffer rb;
	extern uint8_t rbb_buff[1024];
	void mqttInitAndSubcribe(void);
void destroyMqttThread(void);
	void uart_to_mqtt(void* arg);
	void uartInitAndStart(void);
	int sockbConnect(char * host,int port);
#endif
