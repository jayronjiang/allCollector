
///////////////////////////////////////////////////////////////////////////////
//    ����֧�ֵ�����ID
//	  ID��Ӧ���������ġ�����������Ϣ�ṹ�顱��FLASH�е�λ��
////////////////////////////////////////////////////////////////////////////////
	
#define ID_NUM_0	0		//��
#define ID_NUM_1	1		//һ
#define ID_NUM_2	2		//��
#define ID_NUM_3	3		//��
#define ID_NUM_4	4		//��
#define ID_NUM_5	5		//��
#define ID_NUM_6	6		//��
#define ID_NUM_7	7		//��
#define ID_NUM_8	8		//��
#define ID_NUM_9	9		//��
#define ID_NUM_Shi	10		//ʮ
#define ID_NUM_Bai	11		//��
#define ID_NUM_Qian	12		//ǧ
#define ID_Yuan		13		//Ԫ
#define ID_NiHao	14		//���
#define ID_Qing		15		//��
#define ID_Jiao		16		//��
#define ID_XieXie	17		//лл
#define ID_QTX		18		//��ͨ��
#define ID_XingChe	19		//�ͳ�
#define ID_QingQu	20		//��ȡ
#define ID_HYGL		21		//��ӭ����
#define ID_MFC		22		//��ѳ�
#define ID_YFC		23		//Ԥ����
#define ID_XZC		24		//������
#define ID_JJC		25		//������
#define ID_WFC		26		//δ����
#define ID_GWC		27		//����
#define ID_BJC		28		//���ɳ�
#define ID_YuE		29		//���
#define ID_BSCG		30		//��ʶ�ɹ�
#define ID_TXK		31		//ͨ�п�
#define ID_YTK		32		//��ͨ��
#define ID_XKCG		33		//д���ɹ�
#define ID_QingAn	34		//�밴
#define ID_LSJ		35		//��ɫ��
#define ID_QuKa		36		//ȡ��
#define ID_JHAQD	37		//ϵ�ð�ȫ��
#define ID_YLPA		38		//һ·ƽ��
#define ID_SKSB		39		//ˢ��ʧ��
#define ID_ChongXing 40		//����
#define ID_ZhuXiao	41		//ע��
#define ID_Delay	42		// �հ�
	
#define MAX_VOX_ID ID_Delay
////////////////////////////////////////////////////

//�����ӿں���

void Vox_PlayList_Add(unsigned char ID);
void Vox_Prepare_PlayCode(void);
void Vox_Dismiss_Play(void);
void Vox_Wait_AllPlayDone(void);
void Test_All_Vox_ID(void);
unsigned char Vox_PlayList_Empty(void);
unsigned char Vox_CurrPlaying_Done(void);

