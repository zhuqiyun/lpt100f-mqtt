
/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *-pipe -fno-strict-aliasing -Wall -Wstrict-prototypes -Wmissing-prototypes -Werror-implicit-function-declaration -Wpointer-arith -std=gnu99 -ffunction-sections -fdata-sections -Wchar-subscripts -Wcomment -Wformat=2 -Wimplicit-int -Wmain -Wparentheses -Wsequence-point -Wreturn-type -Wswitch -Wtrigraphs -Wunused -Wuninitialized -Wunknown-pragmas -Wfloat-equal -Wundef -Wshadow -Wbad-function-cast -Wwrite-strings -Wsign-compare -Waggregate-return  -Wmissing-declarations -Wformat -Wmissing-format-attribute -Wno-deprecated-declarations -Wpacked -Wredundant-decls -Wnested-externs -Wlong-long -Wunreachable-code -Wcast-align --param max-inline-insns-single=500
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
#include <hsf.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <httpc/httpc.h>
#include "../example.h"
#include "hfmsgq.h"

#if (EXAMPLE_USE_DEMO==USER_UPNP_DEMO)

#define SSDP_PORT				(1900)
#define SSDP_ADDR				("239.255.255.250")

const int hf_gpio_fid_to_pid_map_table[HFM_MAX_FUNC_CODE]=
{
	HF_M_PIN(2),	//HFGPIO_F_JTAG_TCK
	HF_M_PIN(3),	//HFGPIO_F_JTAG_TDO
	HF_M_PIN(4),	//HFGPIO_F_JTAG_TDI
	HF_M_PIN(5),	//HFGPIO_F_JTAG_TMS
	HFM_NOPIN,		//HFGPIO_F_USBDP
	HFM_NOPIN,		//HFGPIO_F_USBDM
	HF_M_PIN(39),	//HFGPIO_F_UART0_TX
	HF_M_PIN(40),	//HFGPIO_F_UART0_RTS
	HF_M_PIN(41),	//HFGPIO_F_UART0_RX
	HF_M_PIN(42),	//HFGPIO_F_UART0_CTS
	
	HF_M_PIN(27),	//HFGPIO_F_SPI_MISO
	HF_M_PIN(28),	//HFGPIO_F_SPI_CLK
	HF_M_PIN(29),	//HFGPIO_F_SPI_CS
	HF_M_PIN(30),	//HFGPIO_F_SPI_MOSI
	
	HFM_NOPIN,	//HFGPIO_F_UART1_TX,
	HFM_NOPIN,	//HFGPIO_F_UART1_RTS,
	HFM_NOPIN,	//HFGPIO_F_UART1_RX,
	HFM_NOPIN,	//HFGPIO_F_UART1_CTS,
	
	HF_M_PIN(43),	//HFGPIO_F_NLINK
	HF_M_PIN(44),	//HFGPIO_F_NREADY
	HF_M_PIN(45),	//HFGPIO_F_NRELOAD
	HF_M_PIN(7),	//HFGPIO_F_SLEEP_RQ
	HF_M_PIN(8),	//HFGPIO_F_SLEEP_ON

	HFM_NOPIN,		//HFGPIO_F_RESERVE0
	HFM_NOPIN,		//HFGPIO_F_RESERVE1
	HFM_NOPIN,		//HFGPIO_F_RESERVE2
	HFM_NOPIN,		//HFGPIO_F_RESERVE3
	HFM_NOPIN,		//HFGPIO_F_RESERVE4
	HFM_NOPIN,		//HFGPIO_F_RESERVE5
	
	HFM_NOPIN,	//HFGPIO_F_USER_DEFINE
};

const hfat_cmd_t user_define_at_cmds_table[]=
{
	{NULL,NULL,NULL,NULL} //the last item must be null
};

static int get_if_ip_addr(void)
{
	char *words[6]={NULL};
	char rsp[64]={0};
	char is_ap_mode=0;

	if(hfat_send_cmd("AT+WMODE\r\n",sizeof("AT+WMODE\r\n"),rsp,64)!=0)
		return 0;

	if(hfat_get_words(rsp,words, 6)>0)
	{
		if(strcmp("STA",words[1])==0)
		{
			is_ap_mode=0;
		}
	}

	if(is_ap_mode)
	{
		hfat_send_cmd("AT+LANN\r\n",sizeof("AT+LANN\r\n"),rsp,64);
	}
	else
		hfat_send_cmd("AT+WANN\r\n",sizeof("AT+WANN\r\n"),rsp,64);
	
	if(hfat_get_words(rsp,words, 6)>0)
	{
		//u_printf("\nresult:%s\nmode:%s\nIP:%s\nMASK:%s\nGW:%s\n",words[0],words[1],words[2],words[3],words[4]);
		if(is_ap_mode)
			return inet_addr(words[1]);
		else
			return inet_addr(words[2]);
	}
	return 0;
}

static int send_packet(int fd, const void *data, size_t len,struct sockaddr_in *to)
{
	int slen=0;
	static struct sockaddr_in toaddr;

	//DEBUG_PRINTF("%s len=%d fd=%d %d\n",__FUNCTION__,len,fd,svr->sockfd);
	if (to==NULL)
	{
		memset(&toaddr, 0, sizeof(struct sockaddr_in));
		toaddr.sin_family = AF_INET;
		toaddr.sin_port = htons(SSDP_PORT);
		toaddr.sin_len = sizeof(struct sockaddr_in);
		toaddr.sin_addr.s_addr = inet_addr(SSDP_ADDR);
		to = &toaddr;
	}
	
	slen = sendto(fd, (const char*)data, len, 0, (struct sockaddr *) to, sizeof(struct sockaddr_in));
	if(slen<0)
		u_printf("slen %d\n",slen);
	
	return slen;
}

static int send_msearch_packet(int fd)
{
	char *msearch="M-SEARCH * HTTP/1.1\r\nHOST: 239.255.255.250:1900\r\nMAN: \"ssdp:discover\"\r\nMX: 2\r\nST: LPB100\r\n\r\n";
	send_packet(fd,msearch,strlen(msearch),NULL);
	return 0;
}


static int send_alive_packet(int fd,char *pkt_buffer,int len,uint32_t if_addr)
{
	char *alivepkt="NOTIFY * HTTP/1.1\r\n"\
"Host:239.255.255.250:1900\r\n"\
"NT:urn:www.hi-flying.com:service:X_HF_LPB100:1\r\n"\
"NTS:ssdp:alive\r\n"\
"Location:%d.%d.%d.%d.48899\r\n"\
"Cache-Control:max-age=900\r\n"\
"Server:HI-FLYING/LPB100 UPnP/1.0 UPnP-Device-Host/1.0\r\n\r\n";
	sprintf(pkt_buffer,alivepkt,if_addr&0xFF,(if_addr>>8)&0xff,(if_addr>>16)&0xff,(if_addr>>24)&0xff);
	send_packet(fd,alivepkt,strlen(alivepkt),NULL);
	return 0;
}

static int send_msearch_rsp_packet(int fd,char *pkt_buffer,int len,uint32_t if_addr,struct sockaddr_in *to)
{
	char rsp_pkt="HTTP/1.1 200 OK\r\n"\
"Cache-Control: max-age=1800\r\n"\
"ST: urn:schemas-upnp-org:device:InternetGatewayDevice:1\r\n"\
"USN: uuid:12342409-1234-1234-5678-28c68ea3946a::urn:schemas-upnp-org:device:InternetGatewayDevice:1\r\n"\
"EXT:\r\n"
"Server: OS 1.0 UPnP/1.0 Netgear/V1.3\r\n"\
"Location: %d.%d.%d.%d.48899\r\n\r\n";

	sprintf(pkt_buffer,rsp_pkt,if_addr&0xFF,(if_addr>>8)&0xff,(if_addr>>16)&0xff,(if_addr>>24)&0xff);
	return 0;
}

static int create_multicast_socket(uint32_t if_addr)
{
	char optval;
	int r;
	int sd;
	int on=0;
	struct ip_mreq mreq;
	struct sockaddr_in serveraddr;

	sd = socket(PF_INET, SOCK_DGRAM, 0);
	if (sd < 0)
	{
		return sd;
	}
	
	if ((r = setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(on))) < 0)
	{
		closesocket(sd);
		return r;
	}

        /* bind to an address */
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(SSDP_PORT);
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); /* receive multicast */
	if ((r = bind(sd, (struct sockaddr *)&serveraddr, sizeof(serveraddr))) < 0)
	{
		closesocket(sd);
		return -1; 
	}
	optval =0;
	if ((r = setsockopt(sd, IPPROTO_IP, IP_MULTICAST_LOOP, (char *) &optval, sizeof(optval))) < 0)
	{
		return r;
	}
	/* Set multicast interface. */
	struct in_addr addr;	
	memset((void *)&addr, 0, sizeof(struct in_addr));
	addr.s_addr = if_addr;
	r = setsockopt(sd, IPPROTO_IP, IP_MULTICAST_IF,(char *)&addr, sizeof(addr));
	if (r == -1)
	{
		closesocket(sd);
		return -1;
	}
	/*
	* setup time-to-live
	*/
	optval = 10; /* Hop count */
	r = setsockopt(sd, IPPROTO_IP, IP_MULTICAST_TTL, &optval, sizeof(optval));
	if (r)  
	{
		closesocket(sd);
		return r;
	}
	// add membership to receiving socket
	memset(&mreq, 0, sizeof(struct ip_mreq));
	mreq.imr_interface.s_addr = if_addr;
	mreq.imr_multiaddr.s_addr = inet_addr(SSDP_ADDR);
	if ((r = setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *) &mreq, sizeof(mreq))) < 0)
	{
		closesocket(sd);
		return r;
	}
		
	return sd;
}

USER_FUNC static void upnp_main_thread(void* arg)
{
	int upnp_fd;
	uint32_t if_addr;
	fd_set sockfd_set;
	int max_fd ;
	 struct timeval tv;	
	 char *pkt_buffer;
	 int ret;
#define UPNP_RECV_PACKET_SIZE      (1024)
	hfnet_enable_multicast(1);
	pkt_buffer = hfmem_malloc(UPNP_RECV_PACKET_SIZE);
	if(pkt_buffer==NULL)
	{
		u_printf("no mem\n");
		return;
	}
	while(1)
	{
		if((if_addr=get_if_ip_addr())==0)
		{
			msleep(500);
			continue;
		}
		upnp_fd = create_multicast_socket(if_addr);
		if(upnp_fd<0)
			return;
		else
			break;
	}
	max_fd =upnp_fd;
	
	while(1)
	{
		int recvsize=0;
		FD_ZERO(&sockfd_set);
		FD_SET(upnp_fd,&sockfd_set);
		tv.tv_sec=30;
		tv.tv_usec=0;
		u_printf("time=%d\n",hfsys_get_time());
		ret=select(max_fd+1,&sockfd_set,NULL,NULL,&tv);
		if(ret<=0)
		{
			send_alive_packet(upnp_fd,pkt_buffer,UPNP_RECV_PACKET_SIZE,if_addr);
			continue;
		}
		if(FD_ISSET(upnp_fd, &sockfd_set))
		{
			struct sockaddr_in fromaddr;
			socklen_t sockaddr_size = sizeof(struct sockaddr_in);
			memset(pkt_buffer,0,UPNP_RECV_PACKET_SIZE);
			recvsize = recvfrom(upnp_fd, (char*)pkt_buffer, UPNP_RECV_PACKET_SIZE-4, 0,(struct sockaddr *) &fromaddr, &sockaddr_size);
			if (recvsize < 0)
			{
				u_printf("recv() fail\n");
			}
			else
			{
				u_printf("recv length=%d\n",recvsize);
				if(strstr(pkt_buffer,"ssdp:discover")!=NULL&&strstr(pkt_buffer,"M-SEARCH")!=NULL)
				{
					send_msearch_rsp_packet(upnp_fd,pkt_buffer,UPNP_RECV_PACKET_SIZE,if_addr,&fromaddr);
				}
				u_printf("%s\n",pkt_buffer);
			}
			
		}
	}

	if(pkt_buffer!=NULL)
		hfmem_free(pkt_buffer);
	
	return ;
}

USER_FUNC void upnp_start(void)
{
	hfthread_create(upnp_main_thread,"upnp_main",256,(void*)1,1,NULL,NULL);
}


int USER_FUNC app_main (void)
{
	time_t now=time(NULL);
	
	HF_Debug(DEBUG_LEVEL,"[THREAD DEMO]sdk version(%s),the app_main start time is %d %s\n",hfsys_get_sdk_version(),now,ctime(&now));
	if(hfgpio_fmap_check(HFM_TYPE_LPB100)!=0)
	{
		while(1)
		{
			HF_Debug(DEBUG_ERROR,"gpio map file error\n");
			msleep(1000);
		}
	}
	while(!hfnet_wifi_is_active())
	{
		msleep(50);
	}
	
	if(hfnet_start_assis(ASSIS_PORT)!=HF_SUCCESS)
	{
		HF_Debug(DEBUG_WARN,"start httpd fail\n");
	}	
	if(hfnet_start_httpd(HFTHREAD_PRIORITIES_MID)!=HF_SUCCESS)
	{
		HF_Debug(DEBUG_WARN,"start httpd fail\n");
	}
	if(hfnet_start_uart(HFTHREAD_PRIORITIES_LOW,NULL)!=HF_SUCCESS)
	{
		HF_Debug(DEBUG_WARN,"start uart fail!\n");
	}
	if(hfnet_start_socketa(HFTHREAD_PRIORITIES_LOW,(hfnet_callback_t)NULL)!=HF_SUCCESS)
	{
		HF_Debug(DEBUG_WARN,"start socketa fail\n");
	}
	if(hfnet_start_socketb(HFTHREAD_PRIORITIES_LOW,(hfnet_callback_t)NULL)!=HF_SUCCESS)
	{
		HF_Debug(DEBUG_WARN,"start socketb fail\n");
	}
	
	upnp_start();
	
	return 1;
	
}

#endif


