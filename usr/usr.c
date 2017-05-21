#include "usr.h"
#include <hsf.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>


uint8_t sta_mac[6]={0};
uint8_t proSendBuff[48]={0};
/// <summary>
/// 链接路由器参数是否配置完毕
/// </summary>
int is_connect_to_router(void)
{
	char *words[6]={NULL};
	char rsp[64]={0};
	hfat_send_cmd("AT+WSSSID\r\n",sizeof("AT+WSSSID\r\n"),rsp,64);
	if(hfat_get_words(rsp,words, 3)>0)
	{
		if(0!=strncmp(words[0],"ok",strlen("ok"))||0!=strncmp(words[1],"fengwen_test",strlen("fengwen_test")))
		{
		  return -1;
		}
	}	
  memset(rsp,0x00,sizeof(rsp));
	hfat_send_cmd("AT+WSKEY\r\n",sizeof("AT+WSKEY\r\n"),rsp,64);
	if(hfat_get_words(rsp,words, 5)>0)
	{
		if(0!=strncmp(words[0],"ok",strlen("ok"))||0!=strncmp(words[3],"1234567890",strlen("1234567890")))
		{
		  return -1;
		}
	}	
  return 0;	
}
/// <summary>
/// 链接到路由器
/// </summary>
int connect_to_router(void)
{
	char *words[6]={NULL};
	char rsp[64]={0};
	hfat_send_cmd("AT+WSSSID=fengwen_test\r\n",sizeof("AT+WSSSID=fengwen_test\r\n"),rsp,64);
	if(hfat_get_words(rsp,words, 2)>0)
	{
		if(0!=strncmp(words[0],"ok",strlen("ok")))
		{
		  return -1;
		}
	}	
  memset(rsp,0x00,sizeof(rsp));
	hfat_send_cmd("AT+WSKEY=WPA2PSK,AES,1234567890\r\n",sizeof("AT+WSKEY=WPA2PSK,AES,1234567890\r\n"),rsp,64);
	if(hfat_get_words(rsp,words, 2)>0)
	{
		if(0!=strncmp(words[0],"ok",strlen("ok")))
		{
		  return -1;
		}
	}	
  return 0;	
}
/// <summary>
/// 链接服务器参数是否配置完毕
/// </summary>
int is_connect_to_server(void)
{
	char *words[6]={NULL};
	char rsp[64]={0};
	hfat_send_cmd("AT+SOCKB\r\n",sizeof("AT+SOCKB\r\n"),rsp,64);
	if(hfat_get_words(rsp,words, 5)>0)
	{
		if((0==strncmp(words[0],"ok",strlen("ok")))&&(0==strncmp(words[1],"TCP",strlen("TCP")))  \
			&&(0==strncmp(words[2],"12345",strlen("12345")))&&(0==strncmp(words[3],"192.168.1.110",strlen("192.168.1.110")))
		)
		{
		  return 0;
		}
	}	
	return -1;	
}
/// <summary>
/// SOCKB是否连接服务器成功
/// </summary>
int is_sockb_connect(void)
{
	char *words[6]={NULL};
	char rsp[64]={0};
	hfat_send_cmd("AT+TCPDISB\r\n",sizeof("AT+TCPDISB\r\n"),rsp,64);
	if(hfat_get_words(rsp,words, 3)>0)
	{
		if((0==strncmp(words[0],"ok",strlen("ok")))&&(0==strncmp(words[1],"on",strlen("on"))))
		{
		  return 0;
		}
	}	
	return -1;	
}
/// <summary>
/// SOCKB链接服务器
/// </summary>
int sockb_connect_to_server(void)
{
	char *words[6]={NULL};
	char rsp[64]={0};
	hfat_send_cmd("AT+TCPDISB=on\r\n",sizeof("AT+TCPDISB=on\r\n"),rsp,64);
	msleep(2000);
	if(hfat_get_words(rsp,words, 2)>0)
	{
		if((0==strncmp(words[0],"ok",strlen("ok"))))
		{
		  return 0;
		}
	}	
	return -1;	
}
/// <summary>
/// 模块工作模式
/// </summary>
int get_work_mode(void)
{
	char *words[6]={NULL};
	char rsp[64]={0};
	hfat_send_cmd("AT+WMODE\r\n",sizeof("AT+WMODE\r\n"),rsp,64);
	if(hfat_get_words(rsp,words, 3)>0)
	{
		if((0==strncmp(words[0],"ok",strlen("ok")))&&(0==strncmp(words[1],"STA",strlen("STA"))))
		{
		  return 0;
		}
	}	
	return -1;	
}
/// <summary>
/// 设置工作模式
/// </summary>
int set_work_mode(void)
{
	char *words[6]={NULL};
	char rsp[64]={0};
	hfat_send_cmd("AT+WMODE=STA\r\n",sizeof("AT+WMODE=STA\r\n"),rsp,64);
	if(hfat_get_words(rsp,words, 3)>0)
	{
		if((0==strncmp(words[0],"ok",strlen("ok"))))
		{
		  return 0;
		}
	}	
	return -1;	
}
/// <summary>
/// 链接到服务器
/// </summary>
int connect_to_server(void)
{
  char *words[6]={NULL};
	char rsp[64]={0};
	hfat_send_cmd("AT+SOCKB=TCP,12345,192.168.1.110\r\n",sizeof("AT+SOCKB=TCP,12345,192.168.1.110\r\n"),rsp,64);
	if(hfat_get_words(rsp,words, 2)>0)
	{
		if(0==strncmp(words[0],"ok",strlen("ok")))
		{
		  return 0;
		}
	}	
	return -1;	
}
/// <summary>
/// 获取模块的mac地址
/// </summary>
int get_sta_mac(void)
{
  char *words[6]={NULL};
	char rsp[64]={0};
	hfat_send_cmd("AT+WSMAC\r\n",sizeof("AAT+WSMAC\r\n"),rsp,64);
	if(hfat_get_words(rsp,words, 3)>0)
	{
		if(0==strncmp(words[0],"ok",strlen("ok")))
		{
			HexStrToByte(words[1],sta_mac,12);
		  return 0;
		}
	}	
	return -1;	
}
/// <summary>
/// 进入透传模式
/// </summary>
int enter_throut_mode(void)
{
  char *words[6]={NULL};
	char rsp[64]={0};
	hfat_send_cmd("AT+ENTM\r\n",sizeof("AT+ENTM\r\n"),rsp,64);
	if(hfat_get_words(rsp,words, 3)>0)
	{
		if(0==strncmp(words[0],"ok",strlen("ok")))
		{
		  return 0;
		}
	}	
	return -1;	
}
/// <summary>
/// 重启设备
/// </summary>
void restart_device(void)
{
	char rsp[64]={0};
	hfat_send_cmd("AT+Z\r\n",sizeof("AT+Z\r\n"),rsp,64);
}
/// <summary>
/// 生成电陶协议校验码
/// </summary>
int proChecksumGen(uint8_t * pro_buff)
{
	int i;
	uint8_t sum=0;
	for(i=0;i<pro_buff[1]-1;i++)
	{
		sum+=pro_buff[i];
	}
	sum=~sum;
	sum+=1;
	return sum;
}
/// <summary>
/// 封装电陶协议
/// </summary>
void proSendWrapper(uint16_t pack_decs,uint16_t msg_type,uint8_t* msg,uint8_t msg_len)
{
	int i;
	proSendBuff[0]=0xaa;
	proSendBuff[1]=0x11+msg_len;
	proSendBuff[2]=pack_decs>>8;  
	proSendBuff[3]=pack_decs>>0;  
	for(i=0;i<8;i++)  proSendBuff[i+4]=0;
	proSendBuff[12]=msg_type>>8;  
	proSendBuff[13]=msg_type>>0; 	
	for(i=0;i<2;i++)  proSendBuff[i+14]=0;
	if(msg!=NULL)
	{
		for(i=0;i<msg_len;i++) proSendBuff[i+16]=msg[i];	
	}
	proSendBuff[proSendBuff[1]-1]=proChecksumGen(proSendBuff);
  hfuart_send(HFUART0,(char *)proSendBuff,proSendBuff[1],100);
}
/// <summary>
/// 字符串转数组
/// </summary>
void HexStrToByte(const char* source, unsigned char* dest, int sourceLen)  
{  
    short i;  
    unsigned char highByte, lowByte;  
      
    for (i = 0; i < sourceLen; i += 2)  
    {  
        highByte = toupper(source[i]);  
        lowByte  = toupper(source[i + 1]);  
        if (highByte > 0x39)  
            highByte -= 0x37;  
        else  
            highByte -= 0x30;  
        if (lowByte > 0x39)  
            lowByte -= 0x37;  
        else  
            lowByte -= 0x30;  
        dest[i / 2] = (highByte << 4) | lowByte;  
    }  
    return ;  
}  



