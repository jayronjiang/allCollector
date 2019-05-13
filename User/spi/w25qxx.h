#ifndef __W25QXX_H
#define __W25QXX_H			    

#define W25Q80 	0XEF13 	
#define W25Q16 	0XEF14
#define W25Q32 	0XEF15
#define W25Q64 	0XEF16
#define W25Q128	0XEF17		   


typedef enum 
{	
	CS_0 = 0,

	CS_NUM
}CSN_NUM_LIST;

// SPI片选引脚初始化,片选0
#define SPI_CS0_GRP	GPIOB		// CS0  PB12
#define SPI_CS0_RCC	RCC_APB2Periph_GPIOB
#define SPI_CS0_PIN	GPIO_Pin_12
#define SPI_CS0_CS	PBout(12)


//#define W25QXX_CS 	PBout(12)  		//W25QXX的片选信号
#define SPI_PAGESIZE 	256              //SPI的一个Flash页为256字节(Xmodem-128字) 

extern uint16_t W25QXX_TYPE;					//定义W25QXX芯片型号
extern uint8_t FLASH_ext_bufx[SPI_PAGESIZE /2 ];				 
////////////////////////////////////////////////////////////////////////////
 
//指令表
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

void SPI_CS_Sel(uint8_t CSn, FunctionalState enValue);
void W25QXX_Init(void);
uint16_t  W25QXX_ReadID(uint8_t CSn);  	    		//读取FLASH ID
uint8_t W25QXX_ReadSR(uint8_t CSn);        		//读取状态寄存器 
void W25QXX_Write_SR(uint8_t CSn,uint8_t sr);  			//写状态寄存器
void W25QXX_Write_Enable(uint8_t CSn);  		//写使能 
void W25QXX_Write_Disable(uint8_t CSn);		//写保护
void W25QXX_Write_NoCheck(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);
void W25QXX_Read(uint8_t CSn,uint8_t* pBuffer,uint32_t ReadAddr, uint16_t NumByteToRead);   //读取flash
void W25QXX_Write(uint8_t* pBuffer,uint32_t WriteAddr, uint16_t NumByteToWrite);//写入flash
void W25QXX_Erase_Chip(uint8_t CSn);    	  	//整片擦除
void W25QXX_Erase_Sector(uint8_t CSn,uint32_t Dst_Addr);	//扇区擦除
void W25QXX_Wait_Busy(void);           	//等待空闲
void W25QXX_PowerDown(uint8_t CSn);        	//进入掉电模式
void W25QXX_WAKEUP(uint8_t CSn);				//唤醒
uint8_t  SPI_Read_ID1(uint8_t CSn);
uint16_t  SPI_Read_ID2(uint8_t CSn,uint8_t ID_Addr);
uint32_t  SPI_Read_ID3(uint8_t CSn);
uint8_t   SPI_M25PE_RDLR(uint8_t CSn,uint32_t Dst_Addr);
void    SPI_Erase_Block(uint8_t CSn,uint32_t Dst_Addr);
void W25QXX_Write_Page(uint8_t CSn,uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);
void SPI_Read_Start(uint8_t CSn,uint32_t Dst_Addr);
void SPI_Read_End(uint8_t CSn);
#endif

