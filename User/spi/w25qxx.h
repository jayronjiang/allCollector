#ifndef __FLASH_H
#define __FLASH_H			    

#ifdef SPEAKER_ENABLE
//W25Xϵ��/Qϵ��оƬ�б�	   
//W25Q80  ID  0XEF13
//W25Q16  ID  0XEF14
//W25Q32  ID  0XEF15
//W25Q64  ID  0XEF16	
//W25Q128 ID  0XEF17	
#define W25Q80 	0XEF13 	
#define W25Q16 	0XEF14
#define W25Q32 	0XEF15
#define W25Q64 	0XEF16
#define W25Q128	0XEF17		   

#define	W25QXX_CS 		PBout(12)  		//W25QXX��Ƭѡ�ź�
#define 	SPI_PAGESIZE 	256              //SPI��һ��FlashҳΪ256�ֽ�(Xmodem-128��) 

extern uint16_t W25QXX_TYPE;					//����W25QXXоƬ�ͺ�
extern uint8_t FLASH_ext_bufx[SPI_PAGESIZE /2 ];				 
////////////////////////////////////////////////////////////////////////////
 
//ָ���
#define W25X_WriteEnable		0x06 
#define W25X_WriteDisable		0x04 
#define W25X_ReadStatusReg		0x05 
#define W25X_WriteStatusReg		0x01 
#define W25X_ReadData			0x03 
#define W25X_FastReadData		0x0B 
#define W25X_FastReadDual		0x3B 
#define W25X_PageProgram		0x02 
#define W25X_BlockErase			0xD8 
#define W25X_SectorErase		0x20 
#define W25X_ChipErase			0xC7 
#define W25X_PowerDown			0xB9 
#define W25X_ReleasePowerDown	0xAB 
#define W25X_DeviceID			0xAB 
#define W25X_ManufactDeviceID	0x90 
#define W25X_JedecDeviceID		0x9F 

#define M25PE_RDLR              0xE8      //read lock register	   1 instruction 3 address  out 1 data
#define M25PE_WRLR              0xE5      //write to lock register

void W25QXX_Init(void);
uint16_t  W25QXX_ReadID(void);  	    		//��ȡFLASH ID
uint8_t  W25QXX_ReadSR(void);        		//��ȡ״̬�Ĵ��� 
void W25QXX_Write_SR(uint8_t sr);  			//д״̬�Ĵ���
void W25QXX_Write_Enable(void);  		//дʹ�� 
void W25QXX_Write_Disable(void);		//д����
void W25QXX_Write_NoCheck(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);
void W25QXX_Read(uint8_t* pBuffer,uint32_t ReadAddr, uint16_t NumByteToRead);   //��ȡflash
void W25QXX_Write(uint8_t* pBuffer,uint32_t WriteAddr, uint16_t NumByteToWrite);//д��flash
void W25QXX_Erase_Chip(void);    	  	//��Ƭ����
void W25QXX_Erase_Sector(uint32_t Dst_Addr);	//��������
void W25QXX_Wait_Busy(void);           	//�ȴ�����
void W25QXX_PowerDown(void);        	//�������ģʽ
void W25QXX_WAKEUP(void);				//����
uint8_t  SPI_Read_ID1(void);
uint16_t  SPI_Read_ID2(uint8_t ID_Addr);
uint32_t  SPI_Read_ID3(void);
uint8_t   SPI_M25PE_RDLR(uint32_t Dst_Addr);
void    SPI_Erase_Block(uint32_t Dst_Addr);
void W25QXX_Write_Page(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);
void SPI_Read_Start(uint32_t Dst_Addr);
void SPI_Read_End(void);
#endif

#endif

