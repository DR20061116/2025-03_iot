/**
 ************************************************************
 ************************************************************
 ************************************************************
 *	文件名： 	onenet.c
 *
 *	作者： 		张继瑞
 *
 *	日期： 		2017-05-08
 *	修改版：    2026 适配STM32+ESP8266+OneNET MQTT/OneJson
 *	版本： 		V1.2
 *	说明： 		修复JSON格式错误、字段映射错误、移除无效依赖，适配多传感器上报
 *	修改记录：	修复JSON语法错误、标识符变量对应关系、删除无用代码
 ************************************************************
 ************************************************************
 ************************************************************
 */

// 单片机头文件
#include "stm32f10x.h"
// 网络设备
#include "esp8266.h"
// 协议文件
#include "onenet.h"
#include "mqttkit.h"
// 算法
#include "base64.h"
#include "hmac_sha1.h"
// 硬件驱动
#include "usart.h"
#include "delay.h"
// C库
#include <string.h>
#include <stdio.h>

// ==================== OneNET 平台核心配置（修改为你的设备信息） ====================
#define PROID		    "Bb3Bt7Z0j0"
#define ACCESS_KEY		"ZlFlUzRtQWpqSmxRWEZsQzZ4N1hrZDcxTkJ2V1V4YTc=" 
#define DEVICE_NAME		"d1"

// ==================== 全局变量声明 ====================
char devid[16];
char key[48];
// 外部传感器全局变量（与main.c一一对应）
extern u8 temp, hum;
extern u8 temp_H, temp_L, hum_H, hum_L;
extern u16 pm25, pm25_H, pm25_L;
extern u16 light, light_H, light_L;
extern u16 co2, co2_H, co2_L;
extern u16 tvoc, tvoc_H, tvoc_L;

// ESP8266接收缓冲区外部声明
extern unsigned char esp8266_buf[512];

// 加密方式固定配置
#define METHOD		"sha1"
// 调试串口宏定义
#define USART_DEBUG   USART1

/*
************************************************************
*	函数名称：	OTA_UrlEncode
*	函数功能：	sign签名URL编码，适配OneNET鉴权规则
*	入口参数：	sign：待编码的签名字符串
*	返回参数：	0-成功 其他-失败
************************************************************
*/
static unsigned char OTA_UrlEncode(char *sign)
{
	char sign_t[40];
	unsigned char i = 0, j = 0;
	unsigned char sign_len = strlen(sign);
	
	if(sign == NULL || sign_len < 28)
		return 1;
	
	// 备份原始签名字符串
	for(; i < sign_len; i++)
	{
		sign_t[i] = sign[i];
		sign[i] = 0;
	}
	sign_t[i] = 0;
	
	// 逐字符转义
	for(i = 0, j = 0; i < sign_len; i++)
	{
		switch(sign_t[i])
		{
			case '+': strcat(sign + j, "%2B"); j += 3; break;
			case ' ': strcat(sign + j, "%20"); j += 3; break;
			case '/': strcat(sign + j, "%2F"); j += 3; break;
			case '?': strcat(sign + j, "%3F"); j += 3; break;
			case '%': strcat(sign + j, "%25"); j += 3; break;
			case '#': strcat(sign + j, "%23"); j += 3; break;
			case '&': strcat(sign + j, "%26"); j += 3; break;
			case '=': strcat(sign + j, "%3D"); j += 3; break;
			default:  sign[j] = sign_t[i]; j++; break;
		}
	}
	sign[j] = 0;
	return 0;
}

/*
************************************************************
*	函数名称：	OneNET_Authorization
*	函数功能：	生成OneNET MQTT连接鉴权Token
*	入口参数：	ver：协议版本、res：产品ID、et：过期时间
*				access_key：平台密钥、dev_name：设备名
*				authorization_buf：token存储缓冲区、flag：鉴权类型
*	返回参数：	0-成功 其他-失败
************************************************************
*/
static unsigned char OneNET_Authorization(char *ver, char *res, unsigned int et, char *access_key, char *dev_name,
											char *authorization_buf, unsigned short authorization_buf_len, _Bool flag)
{
	size_t olen = 0;
	char sign_buf[64];
	char hmac_sha1_buf[64];
	char access_key_base64[64];
	char string_for_signature[72];

	// 参数合法性校验
	if(ver == NULL || res == NULL || et < 1564562581 || access_key == NULL
		|| authorization_buf == NULL || authorization_buf_len < 120)
		return 1;
	
	// AccessKey Base64解码
	memset(access_key_base64, 0, sizeof(access_key_base64));
	BASE64_Decode((unsigned char *)access_key_base64, sizeof(access_key_base64), &olen, (unsigned char *)access_key, strlen(access_key));
	
	// 拼接鉴权明文
	memset(string_for_signature, 0, sizeof(string_for_signature));
	if(flag)
		snprintf(string_for_signature, sizeof(string_for_signature), "%d\n%s\nproducts/%s\n%s", et, METHOD, res, ver);
	else
		snprintf(string_for_signature, sizeof(string_for_signature), "%d\n%s\nproducts/%s/devices/%s\n%s", et, METHOD, res, dev_name, ver);
	
	// HMAC-SHA1加密
	memset(hmac_sha1_buf, 0, sizeof(hmac_sha1_buf));
	hmac_sha1((unsigned char *)access_key_base64, strlen(access_key_base64),
				(unsigned char *)string_for_signature, strlen(string_for_signature),
				(unsigned char *)hmac_sha1_buf);
	
	// 加密结果Base64编码
	olen = 0;
	memset(sign_buf, 0, sizeof(sign_buf));
	BASE64_Encode((unsigned char *)sign_buf, sizeof(sign_buf), &olen, (unsigned char *)hmac_sha1_buf, strlen(hmac_sha1_buf));

	// 签名URL编码
	OTA_UrlEncode(sign_buf);
	
	// 生成最终Token
	if(flag)
		snprintf(authorization_buf, authorization_buf_len, "version=%s&res=products%%2F%s&et=%d&method=%s&sign=%s", ver, res, et, METHOD, sign_buf);
	else
		snprintf(authorization_buf, authorization_buf_len, "version=%s&res=products%%2F%s%%2Fdevices%%2F%s&et=%d&method=%s&sign=%s", ver, res, dev_name, et, METHOD, sign_buf);
	
	return 0;
}

//==========================================================
//	函数名称：	OneNET_RegisterDevice
//	函数功能：	HTTP接口注册设备（备用功能）
//	返回参数：	0-成功 1-失败
//==========================================================
_Bool OneNET_RegisterDevice(void)
{
	_Bool result = 1;
	unsigned short send_len = 11 + strlen(DEVICE_NAME);
	char *send_ptr = NULL, *data_ptr = NULL;
	char authorization_buf[144];
	
	send_ptr = malloc(send_len + 240);
	if(send_ptr == NULL)
		return result;
	
	// 连接OneNET HTTP服务器
	while(ESP8266_SendCmd("AT+CIPSTART=\"TCP\",\"183.230.40.33\",80\r\n", "CONNECT"))
		delay_ms(500);
	
	// 生成鉴权信息
	OneNET_Authorization("2018-10-31", PROID, 1956499200, ACCESS_KEY, NULL,
							authorization_buf, sizeof(authorization_buf), 1);
	
	// 拼接注册请求报文
	snprintf(send_ptr, 240 + send_len, "POST /mqtt/v1/devices/reg HTTP/1.1\r\n"
					"Authorization:%s\r\n"
					"Host:ota.heclouds.com\r\n"
					"Content-Type:application/json\r\n"
					"Content-Length:%d\r\n\r\n"
					"{\"name\":\"%s\"}",
					authorization_buf, 11 + strlen(DEVICE_NAME), DEVICE_NAME);
	
	ESP8266_SendData((unsigned char *)send_ptr, strlen(send_ptr));
	
	// 解析平台返回结果
	data_ptr = (char *)ESP8266_GetIPD(250);
	if(data_ptr)
	{
		data_ptr = strstr(data_ptr, "device_id");
	}
	if(data_ptr)
	{
		char name[16];
		int pid = 0;
		if(sscanf(data_ptr, "device_id\" : \"%[^\"]\",\r\n\"name\" : \"%[^\"]\",\r\n\r\n\"pid\" : %d,\r\n\"key\" : \"%[^\"]\"", devid, name, &pid, key) == 4)
		{
			result = 0;
		}
	}
	
	// 资源释放与连接关闭
	free(send_ptr);
	ESP8266_SendCmd("AT+CIPCLOSE\r\n", "OK");
	return result;
}

//==========================================================
//	函数名称：	OneNet_DevLink
//	函数功能：	MQTT连接OneNET平台，连接成功后自动订阅主题
//	返回参数：	0-连接成功 1-连接失败
//==========================================================
_Bool OneNet_DevLink(void)
{
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};
	unsigned char *dataPtr;
	char authorization_buf[160];
	_Bool status = 1;
	
	// 生成MQTT连接鉴权信息
	OneNET_Authorization("2018-10-31", PROID, 1956499200, ACCESS_KEY, DEVICE_NAME,
								authorization_buf, sizeof(authorization_buf), 0);
	
	// 调试日志输出
	UsartPrintf(USART_DEBUG, "MQTT Connecting... ProductID:%s DeviceName:%s\r\n", PROID, DEVICE_NAME);
	
	// 封装MQTT连接报文
	if(MQTT_PacketConnect(PROID, authorization_buf, DEVICE_NAME, 256, 1, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0)
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);
		dataPtr = ESP8266_GetIPD(250);
		
		if(dataPtr != NULL)
		{
			// 解析连接应答报文
			if(MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_CONNACK)
			{
				uint8_t conn_ret = MQTT_UnPacketConnectAck(dataPtr);
				switch(conn_ret)
				{
					case 0:
						status = 0;
						UsartPrintf(USART_DEBUG, "MQTT Connected Success!\r\n");
						// 核心：连接成功后订阅属性设置主题
						OneNET_Subscribe();
						UsartPrintf(USART_DEBUG, "Subscribe Topic Success!\r\n");
						break;
					case 1: UsartPrintf(USART_DEBUG, "Fail: Version Not Support\r\n"); break;
					case 2: UsartPrintf(USART_DEBUG, "Fail: ClientID Invalid\r\n"); break;
					case 3: UsartPrintf(USART_DEBUG, "Fail: Server Unavailable\r\n"); break;
					case 4: UsartPrintf(USART_DEBUG, "Fail: Auth Failed\r\n"); break;
					case 5: UsartPrintf(USART_DEBUG, "Fail: Permission Denied\r\n"); break;
					default: UsartPrintf(USART_DEBUG, "Fail: Unknown Error\r\n"); break;
				}
			}
		}
		// 释放报文内存
		MQTT_DeleteBuffer(&mqttPacket);
	}
	else
	{
		UsartPrintf(USART_DEBUG, "MQTT Packet Pack Fail\r\n");
	}
	return status;
}

//==========================================================
//	函数名称：	OneNet_SendData
//	函数功能：	单次全量上报数据，修复JSON格式/字段映射，兼容OneJson规范
//==========================================================
void OneNet_SendData(void)
{
    MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};
    char buf[512];
    short body_len = 0, i = 0;

    memset(buf, 0, sizeof(buf));
    // OneJson标准格式：必填version=1.0
    strcpy(buf, "{\"id\":\"123\",\"version\":\"1.0\",\"params\":{");

    // ========== 实时监测数据（只读） ==========
    sprintf(buf+strlen(buf), "\"t1\":{\"value\":%d},",temp);
    sprintf(buf+strlen(buf), "\"h1\":{\"value\":%d},",hum);
    sprintf(buf+strlen(buf), "\"l1\":{\"value\":%d},",light);
    sprintf(buf+strlen(buf), "\"c1\":{\"value\":%d},",co2);
    sprintf(buf+strlen(buf), "\"p1\":{\"value\":%d},",pm25);
    sprintf(buf+strlen(buf), "\"o1\":{\"value\":%d},",tvoc);

    // ========== 阈值配置数据（读写） ==========
    sprintf(buf+strlen(buf), "\"t2\":{\"value\":%d},",temp_H);
    sprintf(buf+strlen(buf), "\"t3\":{\"value\":%d},",temp_L);
    sprintf(buf+strlen(buf), "\"h2\":{\"value\":%d},",hum_H);
    sprintf(buf+strlen(buf), "\"h3\":{\"value\":%d},",hum_L);
    sprintf(buf+strlen(buf), "\"l2\":{\"value\":%d},",light_H);
    sprintf(buf+strlen(buf), "\"l3\":{\"value\":%d},",light_L);
    sprintf(buf+strlen(buf), "\"c2\":{\"value\":%d},",co2_H);
    sprintf(buf+strlen(buf), "\"c3\":{\"value\":%d},",co2_L);
    sprintf(buf+strlen(buf), "\"p2\":{\"value\":%d},",pm25_H);
    sprintf(buf+strlen(buf), "\"p3\":{\"value\":%d},",pm25_L);
    sprintf(buf+strlen(buf), "\"o2\":{\"value\":%d},",tvoc_H);
    // 最后一个字段：无逗号，修复JSON语法错误
    sprintf(buf+strlen(buf), "\"o3\":{\"value\":%d}",tvoc_L);

    // 闭合JSON结构体
    strcat(buf, "}}");
    body_len = strlen(buf);

    // 缓冲区安全校验
    if(body_len > 0 && body_len < 512)
    {
        if(MQTT_PacketSaveData(PROID, DEVICE_NAME, body_len, NULL, &mqttPacket) == 0)
        {
            for(i = 0; i < body_len; i++)
                mqttPacket._data[mqttPacket._len++] = buf[i];
                
            ESP8266_SendData(mqttPacket._data, mqttPacket._len);
            // 打印完整报文，方便调试核对
            UsartPrintf(USART_DEBUG, "上报成功，JSON报文：%s\r\n", buf);
            UsartPrintf(USART_DEBUG, "报文总长度：%d 字节\r\n", mqttPacket._len);
            MQTT_DeleteBuffer(&mqttPacket);
        }
        else
        {
            UsartPrintf(USART_DEBUG, "错误：MQTT封包失败\r\n");
        }
    }
    else
    {
        UsartPrintf(USART_DEBUG, "错误：JSON数据过长或为空\r\n");
    }
}

//==========================================================
//	函数名称：	OneNET_Publish
//	函数功能：	自定义主题消息发布（通用接口）
//==========================================================
void OneNET_Publish(const char *topic, const char *msg)
{
	MQTT_PACKET_STRUCTURE mqtt_packet = {NULL, 0, 0, 0};
	if(MQTT_PacketPublish(MQTT_PUBLISH_ID, topic, msg, strlen(msg), MQTT_QOS_LEVEL0, 0, 1, &mqtt_packet) == 0)
	{
		ESP8266_SendData(mqtt_packet._data, mqtt_packet._len);
		MQTT_DeleteBuffer(&mqtt_packet);
	}
}

//==========================================================
//	函数名称：	OneNET_Subscribe
//	函数功能：	订阅云端属性设置主题，接收下发的阈值指令
//==========================================================
void OneNET_Subscribe(void)
{
    MQTT_PACKET_STRUCTURE mqtt_packet = {NULL, 0, 0, 0};
    char topic_buf[256];
    const char *topics[1];

    // OneNET标准属性设置主题
    snprintf(topic_buf, sizeof(topic_buf), "$sys/%s/%s/thing/property/set", PROID, DEVICE_NAME);
    topics[0] = topic_buf;

    if(MQTT_PacketSubscribe(MQTT_SUBSCRIBE_ID, MQTT_QOS_LEVEL0, topics, 1, &mqtt_packet) == 0)
    {
        ESP8266_SendData(mqtt_packet._data, mqtt_packet._len);
        MQTT_DeleteBuffer(&mqtt_packet);
    }
}

//==========================================================
//	函数名称：	OneNet_RevPro
//	函数功能：	解析云端下发指令，更新传感器阈值
//	入口参数：	cmd：平台接收的原始报文
//==========================================================
void OneNet_RevPro(unsigned char *cmd)
{
	char *req_payload = NULL;
	char *cmdid_topic = NULL;
	unsigned short topic_len = 0;
	unsigned short req_len = 0;
	unsigned char qos = 0;
	static unsigned short pkt_id = 0;
	unsigned char type = 0;
	short result = 0;
	char *dataPtr = NULL;
	int num = 0;

	// 解析MQTT报文类型
	type = MQTT_UnPacketRecv(cmd);
	switch(type)
	{
		// 处理云端下发的Publish报文
		case MQTT_PKT_PUBLISH:
			result = MQTT_UnPacketPublish(cmd, &cmdid_topic, &topic_len, &req_payload, &req_len, &qos, &pkt_id);
			if(result == 0 && req_payload != NULL)
			{
				UsartPrintf(USART_DEBUG, "收到云端指令：%s\r\n", req_payload);
				// 字符串匹配解析阈值参数
				// 温度阈值 t2/t3
				if((dataPtr = strstr((char *)req_payload, "\"t2\":")) != NULL)
				{
					dataPtr += 5; num = 0;
					while(*dataPtr >= '0' && *dataPtr <= '9') { num = num*10 + (*dataPtr-'0'); dataPtr++; }
					temp_H = num;
				}
				if((dataPtr = strstr((char *)req_payload, "\"t3\":")) != NULL)
				{
					dataPtr += 5; num = 0;
					while(*dataPtr >= '0' && *dataPtr <= '9') { num = num*10 + (*dataPtr-'0'); dataPtr++; }
					temp_L = num;
				}
				// 湿度阈值 h2/h3
				if((dataPtr = strstr((char *)req_payload, "\"h2\":")) != NULL)
				{
					dataPtr += 5; num = 0;
					while(*dataPtr >= '0' && *dataPtr <= '9') { num = num*10 + (*dataPtr-'0'); dataPtr++; }
					hum_H = num;
				}
				if((dataPtr = strstr((char *)req_payload, "\"h3\":")) != NULL)
				{
					dataPtr += 5; num = 0;
					while(*dataPtr >= '0' && *dataPtr <= '9') { num = num*10 + (*dataPtr-'0'); dataPtr++; }
					hum_L = num;
				}
				// 光照阈值 l2/l3
				if((dataPtr = strstr((char *)req_payload, "\"l2\":")) != NULL)
				{
					dataPtr += 5; num = 0;
					while(*dataPtr >= '0' && *dataPtr <= '9') { num = num*10 + (*dataPtr-'0'); dataPtr++; }
					light_H = num;
				}
				if((dataPtr = strstr((char *)req_payload, "\"l3\":")) != NULL)
				{
					dataPtr += 5; num = 0;
					while(*dataPtr >= '0' && *dataPtr <= '9') { num = num*10 + (*dataPtr-'0'); dataPtr++; }
					light_L = num;
				}
				// CO2阈值 c2/c3
				if((dataPtr = strstr((char *)req_payload, "\"c2\":")) != NULL)
				{
					dataPtr += 5; num = 0;
					while(*dataPtr >= '0' && *dataPtr <= '9') { num = num*10 + (*dataPtr-'0'); dataPtr++; }
					co2_H = num;
				}
				if((dataPtr = strstr((char *)req_payload, "\"c3\":")) != NULL)
				{
					dataPtr += 5; num = 0;
					while(*dataPtr >= '0' && *dataPtr <= '9') { num = num*10 + (*dataPtr-'0'); dataPtr++; }
					co2_L = num;
				}
				// PM2.5阈值 p2/p3
				if((dataPtr = strstr((char *)req_payload, "\"p2\":")) != NULL)
				{
					dataPtr += 5; num = 0;
					while(*dataPtr >= '0' && *dataPtr <= '9') { num = num*10 + (*dataPtr-'0'); dataPtr++; }
					pm25_H = num;
				}
				if((dataPtr = strstr((char *)req_payload, "\"p3\":")) != NULL)
				{
					dataPtr += 5; num = 0;
					while(*dataPtr >= '0' && *dataPtr <= '9') { num = num*10 + (*dataPtr-'0'); dataPtr++; }
					pm25_L = num;
				}
				// TVOC阈值 o2/o3
				if((dataPtr = strstr((char *)req_payload, "\"o2\":")) != NULL)
				{
					dataPtr += 5; num = 0;
					while(*dataPtr >= '0' && *dataPtr <= '9') { num = num*10 + (*dataPtr-'0'); dataPtr++; }
					tvoc_H = num;
				}
				if((dataPtr = strstr((char *)req_payload, "\"o3\":")) != NULL)
				{
					dataPtr += 5; num = 0;
					while(*dataPtr >= '0' && *dataPtr <= '9') { num = num*10 + (*dataPtr-'0'); dataPtr++; }
					tvoc_L = num;
				}
				ESP8266_Clear();
			}
			break;

		// 平台应答报文，无需业务处理
		case MQTT_PKT_PUBACK:
		case MQTT_PKT_SUBACK:
			break;

		default:
			result = -1;
			break;
	}

	ESP8266_Clear();
	if(result == -1)
		return;

	// 释放动态分配的内存，防止内存泄漏
	if(type == MQTT_PKT_PUBLISH)
	{
		MQTT_FreeBuffer(cmdid_topic);
		MQTT_FreeBuffer(req_payload);
	}
}
