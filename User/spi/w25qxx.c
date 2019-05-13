 /**
  ******************************************************************************
  * @file    w25qxx.c
  * @author  Jerry
  * @date    03-Dec-2018
  *
  * @brief   SPI�豸����
  *
  ******************************************************************************
  */
#include "include.h" 

//uint16_t W25QXX_TYPE=W25Q128;	//Ĭ����W25Q128

uint8_t FLASH_ext_bufx[SPI_PAGESIZE /2 ];

// 4KbytesΪһ��Sector
//16������Ϊ1��Block
//W25Q128
//����Ϊ16M�ֽ�,����128��Block,4096��Sector 

void SPI_CS_Sel(uint8_t CSn, FunctionalState enValue)
{
	switch(CSn)
	{
	// ����Ƭѡ�źŵ�ѡ��
	case CS_0:
		if (enValue)
		{
			SPI_CS0_CS = 0;	// �͵�ƽѡ��
		}
		else
		{
			SPI_CS0_CS = 1;	// �ߵ�ƽ��ѡ��
		}
		break;
	default:
		break;
	}
}

													 
//��ʼ��SPI FLASH��IO��
void W25QXX_Init(void)
{	
  	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(SPI_CS0_RCC, ENABLE );//PORTBʱ��ʹ�� 

	GPIO_InitStructure.GPIO_Pin = SPI_CS0_PIN;  // PB12 ���� 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(SPI_CS0_GRP, &GPIO_InitStructure);
 	GPIO_SetBits(SPI_CS0_GRP,SPI_CS0_PIN);
 
        SPI_CS0_CS = 1;				//SPI FLASH��ѡ��
	SPI2_Init();		   	//��ʼ��SPI
	SPI2_SetSpeed(SPI_BaudRatePrescaler_2);//����Ϊ18Mʱ��,����ģʽ
#if 0
	W25QXX_TYPE=W25QXX_ReadID();//��ȡFLASH ID.  

	while(W25QXX_ReadID()!=W25Q128)		//��ⲻ��W25Q128
	{
		debug_puts("W25Q128 Check Failed!");
		Delay_Ms(500);
		debug_puts("Please Check!        ");	
		Delay_Ms(500);
		LED1_TOGGLE;		//DS0��˸
	}
#endif
}  

//��ȡW25QXX��״̬�Ĵ���
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
//TB,BP2,BP1,BP0:FLASH����д��������
//WEL:дʹ������
//BUSY:æ���λ(1,æ;0,����)
//Ĭ��:0x00
uint8_t W25QXX_ReadSR(uint8_t CSn)   
{  
	uint8_t byte=0;
	
	SPI_CS_Sel(CSn, ENABLE);
	//SPI_CS0_CS=0;                            //ʹ������   
	SPI2_ReadWriteByte(W25X_ReadStatusReg); //���Ͷ�ȡ״̬�Ĵ�������    
	byte=SPI2_ReadWriteByte(0Xff);          //��ȡһ���ֽ�  
	SPI_CS_Sel(CSn, DISABLE);
	//SPI_CS0_CS=1;                            //ȡ��Ƭѡ     
	return byte;   
} 
//дW25QXX״̬�Ĵ���
//ֻ��SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)����д!!!
void W25QXX_Write_SR(uint8_t CSn,uint8_t sr)   
{
	SPI_CS_Sel(CSn, ENABLE);
	//W25QXX_CS=0;                            //ʹ������   
	SPI2_ReadWriteByte(W25X_WriteStatusReg);//����дȡ״̬�Ĵ�������    
	SPI2_ReadWriteByte(sr);               	//д��һ���ֽ�  
	//W25QXX_CS=1;                            //ȡ��Ƭѡ     
	SPI_CS_Sel(CSn, DISABLE);
}   
//W25QXXдʹ��	
//��WEL��λ   
void W25QXX_Write_Enable(uint8_t CSn)   
{
	SPI_CS_Sel(CSn, ENABLE);
	//W25QXX_CS=0;                          	//ʹ������   
	SPI2_ReadWriteByte(W25X_WriteEnable); 	//����дʹ��  
	SPI_CS_Sel(CSn, DISABLE);
	//W25QXX_CS=1;                           	//ȡ��Ƭѡ     	      
} 
//W25QXXд��ֹ	
//��WEL����  
void W25QXX_Write_Disable(uint8_t CSn)   
{  
	//W25QXX_CS=0;                            //ʹ������   
	SPI_CS_Sel(CSn, ENABLE);
	SPI2_ReadWriteByte(W25X_WriteDisable);  //����д��ָֹ��    
	SPI_CS_Sel(CSn, DISABLE);
	//W25QXX_CS=1;                            //ȡ��Ƭѡ     	      
} 		
//��ȡоƬID
//����ֵ����:				   
//0XEF13,��ʾоƬ�ͺ�ΪW25Q80  
//0XEF14,��ʾоƬ�ͺ�ΪW25Q16    
//0XEF15,��ʾоƬ�ͺ�ΪW25Q32  
//0XEF16,��ʾоƬ�ͺ�ΪW25Q64 
//0XEF17,��ʾоƬ�ͺ�ΪW25Q128 	  
uint16_t W25QXX_ReadID(uint8_t CSn)
{
	uint16_t Temp = 0;	
	
	//W25QXX_CS=0;
	SPI_CS_Sel(CSn, ENABLE);
	SPI2_ReadWriteByte(0x90);//���Ͷ�ȡID����	    
	SPI2_ReadWriteByte(0x00); 	    
	SPI2_ReadWriteByte(0x00); 	    
	SPI2_ReadWriteByte(0x00); 	 			   
	Temp|=SPI2_ReadWriteByte(0xFF)<<8;  
	Temp|=SPI2_ReadWriteByte(0xFF);
	SPI_CS_Sel(CSn, DISABLE);
	//W25QXX_CS=1;				    
	return Temp;
}


uint8_t   SPI_M25PE_RDLR(uint8_t CSn, uint32_t Dst_Addr)
{   
	uint8_t Temp = 0;
	
	//W25QXX_CS=0;                            //ʹ������   
	SPI_CS_Sel(CSn, ENABLE);
	SPI2_ReadWriteByte(M25PE_RDLR);                   //  fast read command    
	SPI2_ReadWriteByte(((Dst_Addr & 0xFFFFFF) >> 16));     //  send 3 address bytes    
	SPI2_ReadWriteByte(((Dst_Addr & 0xFFFF) >> 8));   
	SPI2_ReadWriteByte(Dst_Addr & 0xFF);     
    	Temp = SPI2_ReadWriteByte(0xFF);   
	SPI_CS_Sel(CSn, DISABLE);
    	//W25QXX_CS = 1;                                        //  disable device      
    	return Temp;                                        //  return one byte read 
}

uint8_t  SPI_Read_ID1(uint8_t CSn)
{
	uint8_t Temp = 0;

	SPI_CS_Sel(CSn, ENABLE);
	//W25QXX_CS=0;                            //ʹ������   
	SPI2_ReadWriteByte(W25X_DeviceID); //���Ͷ�ȡ״̬�Ĵ������� 
	SPI2_ReadWriteByte(0x00);
	SPI2_ReadWriteByte(0x00);
	SPI2_ReadWriteByte(0x00);
	Temp =SPI2_ReadWriteByte(0Xff);          //��ȡһ���ֽ�  
	//W25QXX_CS=1;                            //ȡ��Ƭѡ  
	SPI_CS_Sel(CSn, DISABLE);
	return Temp;
}   
uint16_t  SPI_Read_ID2(uint8_t CSn,uint8_t ID_Addr)   
{   
	uint16_t Temp = 0;
	//W25QXX_CS=0;	
	SPI_CS_Sel(CSn, ENABLE);
	SPI2_ReadWriteByte(W25X_ManufactDeviceID);//���Ͷ�ȡID����	    
	SPI2_ReadWriteByte(0x00); 
	SPI2_ReadWriteByte(0x00);
	SPI2_ReadWriteByte(ID_Addr);		   
	Temp|=SPI2_ReadWriteByte(0xFF)<<8;
	Temp|=SPI2_ReadWriteByte(0xFF);
	SPI_CS_Sel(CSn, DISABLE);
	//W25QXX_CS=1;
	return Temp;
}   
uint32_t  SPI_Read_ID3(uint8_t CSn)   
{   
	uint32_t Temp = 0;

	SPI_CS_Sel(CSn, ENABLE);
	//W25QXX_CS=0;			    
	SPI2_ReadWriteByte(W25X_JedecDeviceID);		//  send read ID command (9Fh)    
	Temp|=SPI2_ReadWriteByte(0xFF)<<16;		//  receive Manufature or Device ID byte
	Temp|=SPI2_ReadWriteByte(0xFF)<<8;		//  receive Device or Manufacture ID byte 
	Temp|=SPI2_ReadWriteByte(0xFF);
	//W25QXX_CS=1;							//  disable device 
	SPI_CS_Sel(CSn, DISABLE);
	return Temp;
} 

void SPI_Read_Start(uint8_t CSn,uint32_t Dst_Addr)
{
    SPI_CS_Sel(CSn, ENABLE);
    //W25QXX_CS = 0;                                        //  enable device    
    SPI2_ReadWriteByte(W25X_ReadData);                       //  read command    
    SPI2_ReadWriteByte(((Dst_Addr & 0xFFFFFF) >> 16));     //  send 3 address bytes    
    SPI2_ReadWriteByte(((Dst_Addr & 0xFFFF) >> 8));   
    SPI2_ReadWriteByte(Dst_Addr & 0xFF);   
}

void SPI_Read_End(uint8_t CSn)
{   
	//W25QXX_CS = 1;
	SPI_CS_Sel(CSn, DISABLE);
}

//��ȡSPI FLASH  
//��ָ����ַ��ʼ��ȡָ�����ȵ�����
//pBuffer:���ݴ洢��
//ReadAddr:��ʼ��ȡ�ĵ�ַ(24bit)
//NumByteToRead:Ҫ��ȡ���ֽ���(���65535)
void W25QXX_Read(uint8_t CSn,uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead)   
{ 
 	uint16_t  i;
	
	//W25QXX_CS=0;                            	//ʹ������   
	SPI_CS_Sel(CSn, ENABLE);
	SPI2_ReadWriteByte(W25X_ReadData);         	//���Ͷ�ȡ����   
	SPI2_ReadWriteByte((uint8_t)((ReadAddr)>>16));  	//����24bit��ַ    
	SPI2_ReadWriteByte((uint8_t)((ReadAddr)>>8));   
	SPI2_ReadWriteByte((uint8_t)ReadAddr);   
	for(i=0;i<NumByteToRead;i++)
	{ 
		pBuffer[i]=SPI2_ReadWriteByte(0XFF);   	//ѭ������  
	}
	SPI_CS_Sel(CSn, DISABLE);
	//W25QXX_CS=1;  				    	      
}  

//SPI��һҳ(0~65535)��д������256���ֽڵ�����
//��ָ����ַ��ʼд�����256�ֽڵ�����
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���!!!	 
void W25QXX_Write_Page(uint8_t CSn,uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)
{
 	uint16_t i;  
	
	W25QXX_Write_Enable(CS_0);                  	//SET WEL 
	//W25QXX_CS=0;                            	//ʹ������
	SPI_CS_Sel(CSn, ENABLE);
	SPI2_ReadWriteByte(W25X_PageProgram);      	//����дҳ����   
	SPI2_ReadWriteByte((uint8_t)((WriteAddr)>>16)); 	//����24bit��ַ    
	SPI2_ReadWriteByte((uint8_t)((WriteAddr)>>8));   
	SPI2_ReadWriteByte((uint8_t)WriteAddr);   
	for(i=0;i<NumByteToWrite;i++) SPI2_ReadWriteByte(pBuffer[i]);//ѭ��д��  
	//W25QXX_CS=1;                            	//ȡ��Ƭѡ 
	SPI_CS_Sel(CSn, DISABLE);
	W25QXX_Wait_Busy();					   		//�ȴ�д�����
} 
//�޼���дSPI FLASH 
//����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
//�����Զ���ҳ���� 
//��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)
//CHECK OK
void W25QXX_Write_NoCheck(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{ 			 		 
	uint16_t pageremain;	   
	pageremain=256-WriteAddr%256; //��ҳʣ����ֽ���		 	    
	if(NumByteToWrite<=pageremain) pageremain=NumByteToWrite;//������256���ֽ�
	while(1)
	{	   
		W25QXX_Write_Page(CS_0,pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)break;//д�������
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //��ȥ�Ѿ�д���˵��ֽ���
			if(NumByteToWrite>256)pageremain=256; //һ�ο���д��256���ֽ�
			else pageremain=NumByteToWrite; 	  //����256���ֽ���
		}
	};	    
} 
//дSPI FLASH  
//��ָ����ַ��ʼд��ָ�����ȵ�����
//�ú�������������!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)						
//NumByteToWrite:Ҫд����ֽ���(���65535)   
uint8_t W25QXX_BUFFER[4096];		 
void W25QXX_Write(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{ 
	uint32_t secpos;
	uint16_t secoff;
	uint16_t secremain;	   
 	uint16_t i;    
	uint8_t * W25QXX_BUF;	  
   	W25QXX_BUF=W25QXX_BUFFER;	     
 	secpos=WriteAddr/4096;	//������ַ  
	secoff=WriteAddr%4096;		//�������ڵ�ƫ��
	secremain=4096-secoff;		//����ʣ��ռ��С   
 
 	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//������4096���ֽ�
	while(1) 
	{	
		W25QXX_Read(CS_0,W25QXX_BUF,secpos*4096,4096);//������������������
		for(i=0;i<secremain;i++)//У������
		{
			if(W25QXX_BUF[secoff+i]!=0XFF)break;//��Ҫ����  	  
		}
		if(i<secremain)//��Ҫ����
		{
			W25QXX_Erase_Sector(CS_0,secpos);		//�����������
			for(i=0;i<secremain;i++)	   		//����
			{
				W25QXX_BUF[i+secoff]=pBuffer[i];	  
			}
			W25QXX_Write_NoCheck(W25QXX_BUF,secpos*4096,4096);//д����������  

		}else W25QXX_Write_NoCheck(pBuffer,WriteAddr,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 				   
		if(NumByteToWrite==secremain)break;//д�������
		else//д��δ����
		{
			secpos++;//������ַ��1
			secoff=0;//ƫ��λ��Ϊ0 	 

		   	pBuffer+=secremain;  				//ָ��ƫ��
			WriteAddr+=secremain;				//д��ַƫ��	   
		   	NumByteToWrite-=secremain;			//�ֽ����ݼ�
			if(NumByteToWrite>4096)secremain=4096;//��һ����������д����
			else secremain=NumByteToWrite;		//��һ����������д����
		}	 
	};	 
}
//��������оƬ		  
//�ȴ�ʱ�䳬��...
void W25QXX_Erase_Chip(uint8_t CSn)   
{                                   
	W25QXX_Write_Enable(CS_0);                 	 	//SET WEL 
	W25QXX_Wait_Busy();   
  	//W25QXX_CS=0;                            	//ʹ������   
  	SPI_CS_Sel(CSn, ENABLE);
	SPI2_ReadWriteByte(W25X_ChipErase);        	//����Ƭ��������
	SPI_CS_Sel(CSn, DISABLE);
	//W25QXX_CS=1;                            	//ȡ��Ƭѡ     	      
	W25QXX_Wait_Busy();   				   		//�ȴ�оƬ��������
}   
//����һ������
//Dst_Addr:������ַ ����ʵ����������
//����һ��ɽ��������ʱ��:150ms
void W25QXX_Erase_Sector(uint8_t CSn,uint32_t Dst_Addr)   
{  
	//����falsh�������,������ 
#ifdef DEBUG_EN
 	//printf("fe:%x\r\n",Dst_Addr);
#endif
 	Dst_Addr*=4096;
	W25QXX_Write_Enable(CSn);                  	//SET WEL 	 
	W25QXX_Wait_Busy();   
  	//W25QXX_CS=0;                            	//ʹ������
  	SPI_CS_Sel(CSn, ENABLE);
	SPI2_ReadWriteByte(W25X_SectorErase);      	//������������ָ�� 
	SPI2_ReadWriteByte((uint8_t)((Dst_Addr)>>16));  	//����24bit��ַ    
	SPI2_ReadWriteByte((uint8_t)((Dst_Addr)>>8));   
	SPI2_ReadWriteByte((uint8_t)Dst_Addr);  
	//W25QXX_CS=1;                            	//ȡ��Ƭѡ    
	SPI_CS_Sel(CSn, DISABLE);
	W25QXX_Wait_Busy();   				   		//�ȴ��������
} 

//����һ��block
//Dst_Addr:������ַ ����ʵ����������
void    SPI_Erase_Block(uint8_t CSn,uint32_t Dst_Addr)   
{   
    	W25QXX_Write_Enable(CS_0);                  	//SET WEL 	 
    	W25QXX_Wait_Busy();
	SPI_CS_Sel(CSn, ENABLE);
  	//W25QXX_CS=0;                            	//ʹ������   
    	SPI2_ReadWriteByte(W25X_BlockErase);      	//������������ָ�� 
   	SPI2_ReadWriteByte((uint8_t)((Dst_Addr & 0xFFFFFF)>>16));  	//����24bit��ַ    
    	SPI2_ReadWriteByte((uint8_t)((Dst_Addr & 0xFFFF)>>8));   
    	SPI2_ReadWriteByte((uint8_t)Dst_Addr & 0xFF);  
	//W25QXX_CS=1;                            	//ȡ��Ƭѡ     
	SPI_CS_Sel(CSn, DISABLE);
    	W25QXX_Wait_Busy();   	
}   


//�ȴ�����
void W25QXX_Wait_Busy(void)   
{   
	while((W25QXX_ReadSR(CS_0)&0x01)==0x01);  		// �ȴ�BUSYλ���
}  
//�������ģʽ
void W25QXX_PowerDown(uint8_t CSn)   
{ 
	SPI_CS_Sel(CSn, ENABLE);
	//W25QXX_CS=0;                           	 	//ʹ������   
	SPI2_ReadWriteByte(W25X_PowerDown);        //���͵�������  
	//W25QXX_CS=1;                            	//ȡ��Ƭѡ  
	SPI_CS_Sel(CSn, DISABLE);
	Delay_Xus(3);                               //�ȴ�TPD  
}   
//����
void W25QXX_WAKEUP(uint8_t CSn)   
{  
	SPI_CS_Sel(CSn, ENABLE);
  	//W25QXX_CS=0;                            	//ʹ������   
	SPI2_ReadWriteByte(W25X_ReleasePowerDown);	//  send W25X_PowerDown command 0xAB    
	//W25QXX_CS=1;                            	//ȡ��Ƭѡ     	      
	SPI_CS_Sel(CSn, DISABLE);
	Delay_Xus(3);                            	//�ȴ�TRES1
}

