#ifndef __MESSAGE_HANDLE_H
#define	__MESSAGE_HANDLE_H

/*�����շѺ��˹��շѹ��ػ����ڳ�������˵��һ����*/
#define BD1_UART			UART1_COM	//�������һ��PCͨ�ŵ�Ϊ�ĸ�����?
#if (BD_USART_NUM >= 2)
#define BD2_UART			UART2_COM	//������ڶ���PCͨ�ŵ�Ϊ�ĸ�����?
#endif
#if (BD_USART_NUM >= 3)
#define BD3_UART			UART4_COM	//������ڶ���PCͨ�ŵ�Ϊ�ĸ�����?
#endif
#if (BD_USART_NUM >= 4)
#define BD4_UART			UART5_COM	//������ڶ���PCͨ�ŵ�Ϊ�ĸ�����?
#endif
/*����ɳ������ڵ�ֵ,��ʾ����Ҫ�������*/
#define TRANS_UART		UART2_COM	//����͸���Ĵ���
#define PC_UART			UART1_COM	// ������λ�����ĸ�����
#define REAL_DATA_UART	UART5_COM	// ���ش�������UART5
#define AIR_COND_UART	UART4_COM	// ��������ݴ���
#define UPS_UART			UART2_COM	// UART2��232��
#define SPD_UART			UART4_COM	// ��UPS,ע��UPS��232����Ҫ����һ����
#define TEMP_UART		UART4_COM	// ��������ݴ���
#define WATER_UART		UART4_COM	// ˮ������
#define LOCKER_UART		UART4_COM	// ��������
#define SMOKE_UART		UART4_COM	// �̸д���
#define BREAKER_UART		UART4_COM	// ��·��
#define ARD_UART			UART4_COM	// �Զ��غ�բ

#define SEL_R		0		// ����
#define SEL_S		1		// ����


// ��Ϣ�������
#define NOT_USED_MSG		0xFF		// δʹ��
#define RES_MSG				0x01		// �ظ�֡
#define ERR_RES_MSG			0x02		// �ظ�֡
#define TRANS_MSG			0x0A		// ͸������

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
