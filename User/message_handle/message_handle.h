#ifndef __MESSAGE_HANDLE_H
#define	__MESSAGE_HANDLE_H

/*�����շѺ��˹��շѹ��ػ����ڳ�������˵��һ����*/
#define PC1_UART			UART1_COM	//�������һ��PCͨ�ŵ�Ϊ�ĸ�����?
#if (PC_USART_NUM == 2)
#define PC2_UART			UART2_COM	//������ڶ���PCͨ�ŵ�Ϊ�ĸ�����?
#endif
/*����ɳ������ڵ�ֵ,��ʾ����Ҫ�������*/
#define FEE_UART			UART_NUM			//��������Ժ������Ĵ���
#define TRANS_UART		UART_NUM			//����͸���Ĵ���

// λ����
#define MSG_SOF			0x02		// ��Ϣ�Ŀ�ʼ
#define MSG_EOF			0x03		// ��Ϣ�Ľ���

#define COM_CTRL_LENTH 		24		// ��Ϣ�ܳ�24
#define COM_FX1_LENTH 		52		// ����,��Ϣ�ܳ�24
#define COM_FX2_LENTH 		6		// ���Ժ��̵�,����6
#define COM_FX3_LENTH 		4		// �������,��ȫ��4

// ��Ϣ���Ͷ���
#define INTT_MSG				0
#define FEE_G_MSG			1
#define FEE_R_MSG			2
#define VER_PRINT_MSG		3		//��ӡ�汾��
#define MEM_DUMP_MSG		4		//�����ڴ�,ʲô����
#define B_RES_MSG			5		//����ϢB�ظ�
#define ERR_RES_MSG			6		//��Ϣ��ʽ����
#define SPK_MSG				7		// ��������
#define COST_ON_MSG				8		// �Ѷ���ʾ
#define COST_OFF_MSG				9		// �Ѷ����
#define TRANS_MSG			0x0A		// ͸������

#define ALL8_MSG				0x10		// ��ʾȫ8����������
#define VOXPLAY_MSG			0x11		// �������Բ��Ų�������

#define TEST_MSG				99		// ���Թ���

#define LOCAL_ADD			('1')		//0x31

#define ERR_OK				0		/*У��OK*/
#define SITEID_ERROR			1		/*���ص�ַ����*/
#define LENGTH_ERROR			2		/*�����Ȳ���*/
#define CRC_ERROR			3		/*CRCУ�����*/
#define SOF_ERROR			4		/*ǰ���������������*/
#define DUMMY_ERROR			5		/*����֡Ϊ�����һ�������*/
#define TRANS_REQ			6		/*��У��,ֱ��ת��������*/


// ͨ��Э����ֽڶ���
#define BSOF					0	//��ʼ��
#define COM_T				1	//ͨ����A/B/C��
#define ADDR					2	//�豸��ַ
#define CTRL_T				3	//�����ֽ�
#define COST_0				4	//���
#define COST_1				5	//���
#define COST_2				6	//���
#define COST_3				7	//���
#define LEFT_0				8	//���
#define LEFT_1				9	//���
#define LEFT_2				10	//���
#define LEFT_3				11	//���
#define ENTRY_0				12	//���վ
#define ENTRY_1				13	//���վ
#define ENTRY_2				14	//���վ
#define ENTRY_3				15	//���վ
#define ENTRY_4				16	//���վ
#define ENTRY_5				17	//���վ
#define VECLE				18	//����
#define WEIGHT				19	//����
#define CTRL_B				20	//�����ֽ�2
#define SQU					21	//ͨѶ���
#define BEOF					22	//����
#define BCC					23	//У����

// ���������붨��
#define FX_SOF				0xF5		// ��ʼ��
#define FX_DISPLAY			0x01		// ������,��ʾ����
#define FX_DISCLR				0x02		// ������,����
#define FX_DISALL				0x03		// ������,ȫ������
#define FX_LED				0x10		// ������,���̵ƿ���
#define FX_EOF				0xFE		// ������


typedef  struct 
{
	uint8_t *pTxBuf;
	uint8_t *pRxBuf;
	uint16_t TxLen;
	uint16_t RxLen;
}PROTOCOL_BUF;

extern PROTOCOL_BUF	ProtocolBuf[UART_NUM];

void Comm1_Init(uint32_t baudrate);
#if (BD_USART_NUM == 2)
void Comm2_Init(uint32_t baudrate);
#endif
void message_pack(USART_LIST uart_no, uint8_t msg_type,PROTOCOL_BUF *buf);
void message_send_printf(USART_LIST uartNo,bool pack_en, uint8_t msg_type);
void Comm_Proc(void);

#endif /* __MESSAGE_HANDLE_H */
