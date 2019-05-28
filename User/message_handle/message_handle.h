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
#define FEE_UART			UART_NUM	//定义与费显和声音的串口
#define TRANS_UART		UART5_COM	//定义透传的串口
#define PC_UART			UART1_COM	// 定义上位机是哪个串口
#define MEAS_UART		UART4_COM	// 读电表数据串口
#define AIR_COND_UART	UART4_COM	// 读电表数据串口
#define UPS_UART			UART2_COM	// 读电表数据串口
#define SPD_UART			UART4_COM	// 读UPS,注意UPS是232的需要单独一个口
#define TEMP_UART		UART4_COM	// 读电表数据串口

// 位定义
#define MSG_SOF			0x02		// 信息的开始
#define MSG_EOF			0x03		// 信息的结束

#define COM_CTRL_LENTH 		24		// 信息总长24
#define COM_FX1_LENTH 		52		// 费显,信息总长24
#define COM_FX2_LENTH 		6		// 费显红绿灯,长度6
#define COM_FX3_LENTH 		4		// 清除费显,或全亮4

// 信息类型定义
#define NOT_USED_MSG		0xFF		// 未使用
#define RES_MSG				0x01		// 回复帧
#define ERR_RES_MSG			0x02		// 回复帧
#define TRANS_MSG			0x0A		// 透传命令

#define TEST_MSG				99		// 测试功能

#define LOCAL_ADD			('1')		//0x31

#define ERR_OK				0		/*校验OK*/
#define SITEID_ERROR			1		/*本地地址不对*/
#define LENGTH_ERROR			2		/*包长度不对*/
#define CRC_ERROR			3		/*CRC校验错误*/
#define SOF_ERROR			4		/*前导符或结束符错误*/
#define DUMMY_ERROR			5		/*数据帧为不需进一步处理的*/
#define TRANS_REQ			6		/*不校验,直接转发的数据*/


// 通信协议的字节定义
#define BSOF					0	//起始码
#define COM_T				1	//通信类A/B/C类
#define ADDR					2	//设备地址
#define CTRL_T				3	//控制字节
#define COST_0				4	//金额
#define COST_1				5	//金额
#define COST_2				6	//金额
#define COST_3				7	//金额
#define LEFT_0				8	//余额
#define LEFT_1				9	//余额
#define LEFT_2				10	//余额
#define LEFT_3				11	//余额
#define ENTRY_0				12	//入口站
#define ENTRY_1				13	//入口站
#define ENTRY_2				14	//入口站
#define ENTRY_3				15	//入口站
#define ENTRY_4				16	//入口站
#define ENTRY_5				17	//入口站
#define VECLE				18	//车型
#define WEIGHT				19	//车种
#define CTRL_B				20	//控制字节2
#define SQU					21	//通讯序号
#define BEOF					22	//结束
#define BCC					23	//校验码

// 费显命令码定义
#define FX_SOF				0xF5		// 起始码
#define FX_DISPLAY			0x01		// 操作码,显示费显
#define FX_DISCLR				0x02		// 操作码,清屏
#define FX_DISALL				0x03		// 操作码,全屏点亮
#define FX_LED				0x10		// 操作码,红绿灯控制
#define FX_EOF				0xFE		// 结束码


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
void Comm_Proc(void);

#endif /* __MESSAGE_HANDLE_H */
