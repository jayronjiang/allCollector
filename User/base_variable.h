/********************************************************************************
*  ��Ȩ����: 	��ͨ�о�Ժ			
*
*  �ļ���:       base_variable.h 
*
*  ����:             Jerry
*
*  ��������: 2018.10.17
*
*  ����: ����ṹ������������ݵĺ궨�壬�ᱻ���е��ļ�����
*
*  ����: 
*
*  ��ʷ:  
********************************************************************************/

#ifndef _BASE_VARIABLE_H
#define _BASE_VARIABLE_H

#define DEBUG_EN

//16λ��32λƽ̨�����ǲ�һ���ģ�Ҫע��
typedef unsigned char        	BOOLEAN;
typedef unsigned char 	     	UINT8;
typedef unsigned char 	     	INT8U;
typedef signed char   	    	INT8;
typedef signed char   	    	INT8S;
typedef unsigned short int  	UINT16;
typedef unsigned short int  	INT16U;
typedef signed short int    	INT16;
typedef signed short int    	INT16S;
typedef unsigned int 	     	UINT32;
typedef unsigned int 	     	INT32U;
typedef signed int   	     		INT32;
typedef signed int   	     		INT32S;
typedef unsigned       __int64 	INT64U;
typedef signed __int64     	INT64S;
typedef unsigned       __int64 	UINT64;
typedef signed __int64     	INT64;
typedef unsigned char 		bool;		// ����bool���ͣ�ֻʹ��TRUE��FALSE 2��ֵ

#define BIT0 0X01
#define BIT1 0X02
#define BIT2 0X04
#define BIT3 0X08
#define BIT4 0X10
#define BIT5 0X20
#define BIT6 0X40
#define BIT7 0X80
#define BIT8 0X100
#define BIT9 0X200
#define BIT10 0X400
#define BIT11 0X800
#define BIT12 0X1000
#define BIT13 0X2000
#define BIT14 0X4000
#define BIT15 0X8000
#define BIT16 0X10000
#define BIT17 0X20000
#define BIT18 0X40000
#define BIT19 0X80000
#define BIT20 0X100000
#define BIT21 0X200000
#define BIT22 0X400000
#define BIT23 0X800000
#define BIT24 0X1000000
#define BIT25 0X2000000
#define BIT26 0X4000000
#define BIT27 0X8000000
#define BIT28 0X10000000
#define BIT29 0X20000000
#define BIT30 0X40000000
#define BIT31 0X80000000

#define	BIT(i)	(1 << (i))
#define   LBIT(i)	(1ULL<<(i))	/*32λƽ̨long long�Ͳ���64λ,long��32λ*/


#define TRUE	       		1
#define FALSE          	0

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //��� 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //����

/*�ṹ������������*/
typedef union int_union
{
	INT16U i;
	INT8U b[2];
}INTEGER_UNION;

/*�ṹ��������������*/
typedef union long_union
{
	INT32U i;
	INT8U b[4];
}LONG_UNION;

#endif

