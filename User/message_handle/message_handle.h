#ifndef __MESSAGE_HANDLE_H
#define	__MESSAGE_HANDLE_H

/*自助收费和人工收费工控机对于车控器来说是一样的*/
#define BD1_UART			UART1_COM	//定义与第一个PC通信的为哪个串口?
#if (BD_USART_NUM >= 2)
#define BD2_UART			UART2_COM	//定义与第二个PC通信的为哪个串口?
#endif
#if (BD_USART_NUM >= 3)
#define BD3_UART			UART4_COM	//定义与第二个PC通信的为哪个串口?
#endif
#if (BD_USART_NUM >= 4)
#define BD4_UART			UART5_COM	//定义与第二个PC通信的为哪个串口?
#endif
/*定义成超出串口的值,表示不需要这个功能*/
#define TRANS_UART		UART2_COM	//定义透传的串口
#define PC_UART			UART1_COM	// 定义上位机是哪个串口
#define REAL_DATA_UART	UART5_COM	// 板载传感器是UART5
#define AIR_COND_UART	UART4_COM	// 读电表数据串口
#define UPS_UART			UART2_COM	// UART2是232的
#define SPD_UART			UART4_COM	// 读UPS,注意UPS是232的需要单独一个口
#define TEMP_UART		UART4_COM	// 读电表数据串口
#define WATER_UART		UART4_COM	// 水浸串口
#define LOCKER_UART		UART4_COM	// 开锁串口
#define SMOKE_UART		UART4_COM	// 烟感串口
#define BREAKER_UART		UART4_COM	// 断路器
#define ARD_UART			UART4_COM	// 自动重合闸

#define SEL_R		0		// 接收
#define SEL_S		1		// 发送


// 信息打包类型
#define NOT_USED_MSG		0xFF		// 未使用
#define RES_MSG				0x01		// 回复帧
#define ERR_RES_MSG			0x02		// 回复帧
#define TRANS_MSG			0x0A		// 透传命令

typedef  struct 
{
	uint8_t *pTxBuf;
	uint8_t *pRxBuf;
	uint16_t TxLen;
	uint16_t RxLen;
}PROTOCOL_BUF;

extern PROTOCOL_BUF	ProtocolBuf[UART_NUM];

void Comm1_Init(uint32_t baudrate);
#if (BD_USART_NUM >= 2)
void Comm2_Init(uint32_t baudrate);
#endif
#if (BD_USART_NUM >= 3)
void Comm4_Init(uint32_t baudrate);
#endif
#if (BD_USART_NUM >= 4)
void Comm5_Init(uint32_t baudrate);
#endif
void message_pack(USART_LIST uart_no, uint8_t msg_type,PROTOCOL_BUF *buf);
void message_send_printf(USART_LIST destUtNo,USART_LIST scUtNo,bool pack_en, uint8_t msg_type);
void comm_rec_proc(void);

#endif /* __MESSAGE_HANDLE_H */
