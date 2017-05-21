/*******************************************************************************
 * Copyright (c) 2014, 2015 IBM Corp.
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
 *    Ian Craggs - documentation and platform specific header
 *******************************************************************************/

#if !defined(__MQTT_HSF_)
#define __MQTT_HSF_

#if defined(__cplusplus)
 extern "C" {
#endif
	 
  #include <hsf.h>
	 
	 
#define MQTT_TASK 1	 
	 
	 
typedef struct Timer Timer;

struct Timer
{
    unsigned long end_time;
};

	typedef struct Network Network;

	struct Network
	{
		int my_socket;
		int (*mqttread) (Network*, unsigned char*, int, int);
		int (*mqttwrite) (Network*, unsigned char*, int, int);
		void (*disconnect) (Network*);
	};
	
	void TimerInit(Timer*);
	char TimerIsExpired(Timer*);
	void TimerCountdownMS(Timer*, unsigned int);
	void TimerCountdown(Timer*, unsigned int);
	int TimerLeftMS(Timer*);

	typedef struct Mutex
	{
		hfthread_sem_t sem;
	} Mutex;

	int MutexInit(Mutex*);
  void MutexFree(Mutex* mutex);
	int MutexLock(Mutex*);
	void MutexUnlock(Mutex*);

	typedef struct Thread
	{
		int task;
	} Thread;

	int ThreadStart(Thread*, void (*fn)(void*), void* arg);

	int HSF_read(Network*, unsigned char*, int, int);
	int HSF_write(Network*, unsigned char*, int, int);
	void HSF_disconnect(Network*);

	void NetworkInit(Network*);
	
	int NetworkConnect(Network*, char*, int);	
	

#if defined(__cplusplus)
     }
#endif

#endif
