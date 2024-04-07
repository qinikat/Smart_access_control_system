#include <string.h>
#include "main.h"
#include "AS608.h"
#include "usart.h"
#include "stdio.h"
uint32_t AS608Addr = 0XFFFFFFFF; // 默认
extern uint8_t aRxBuffer[RXBUFFERSIZE_UART1];

// 串口发送一个字节
static void Com_SendData(uint8_t data)
{
	HAL_UART_Transmit(&huart1, &data, 1, 50);
}
// 发送包头
static void SendHead(void)
{
	Com_SendData(0xEF);
	Com_SendData(0x01);
}
// 发送地址
static void SendAddr(void)
{
	Com_SendData(AS608Addr >> 24);
	Com_SendData(AS608Addr >> 16);
	Com_SendData(AS608Addr >> 8);
	Com_SendData(AS608Addr);
}
// 发送包标识,
static void SendFlag(uint8_t flag)
{
	Com_SendData(flag);
}
// 发送包长度
static void SendLength(int length)
{
	Com_SendData(length >> 8);
	Com_SendData(length);
}
// 发送指令码
static void Sendcmd(uint8_t cmd)
{
	Com_SendData(cmd);
}
// 发送校验和
static void SendCheck(uint16_t check)
{
	Com_SendData(check >> 8);
	Com_SendData(check);
}
// 判断中断接收的数组有没有应答包
// waittime为等待中断接收数据的时间（单位1ms）
// 返回值：数据包首地址
extern uint8_t RX_len; // 接收字节计数
static uint8_t *JudgeStr(uint16_t waittime)
{
	char *data;
	uint8_t str[8];
	str[0] = 0xef;
	str[1] = 0x01;
	str[2] = AS608Addr >> 24;
	str[3] = AS608Addr >> 16;
	str[4] = AS608Addr >> 8;
	str[5] = AS608Addr;
	str[6] = 0x07;
	str[7] = '\0';

	while (--waittime)
	{
		HAL_Delay(1);
		if (RX_len) // 接收到一次数据
		{
			RX_len = 0;
			data = strstr((const char *)&aRxBuffer, (const char *)str);
			if (data)
				return (uint8_t *)data;
		}
	}
	return 0;
}
// 录入图像 GZ_GetImage
// 功能:探测手指，探测到后录入指纹图像存于ImageBuffer。
// 模块返回确认字
uint8_t GZ_GetImage(void)
{
	uint16_t temp;
	uint8_t ensure;
	uint8_t *data;
	SendHead();
	SendAddr();
	SendFlag(0x01); // 命令包标识
	SendLength(0x03);
	Sendcmd(0x01);
	temp = 0x01 + 0x03 + 0x01;
	SendCheck(temp);
	data = JudgeStr(2000);
	if (data)
		ensure = data[9];
	else
		ensure = 0xff;
	return ensure;
}
// 生成特征 GZ_GenChar
// 功能:将ImageBuffer中的原始图像生成指纹特征文件存于CharBuffer1或CharBuffer2
// 参数:BufferID --> charBuffer1:0x01	charBuffer1:0x02
// 模块返回确认字
uint8_t GZ_GenChar(uint8_t BufferID)
{
	uint16_t temp;
	uint8_t ensure;
	uint8_t *data;
	SendHead();
	SendAddr();
	SendFlag(0x01); // 命令包标识
	SendLength(0x04);
	Sendcmd(0x02);
	Com_SendData(BufferID);
	temp = 0x01 + 0x04 + 0x02 + BufferID;
	SendCheck(temp);
	data = JudgeStr(2000);
	if (data)
		ensure = data[9];
	else
		ensure = 0xff;
	return ensure;
}
// 精确比对两枚指纹特征 GZ_Match
// 功能:精确比对CharBuffer1 与CharBuffer2 中的特征文件
// 模块返回确认字
uint8_t GZ_Match(void)
{
	uint16_t temp;
	uint8_t ensure;
	uint8_t *data;
	SendHead();
	SendAddr();
	SendFlag(0x01); // 命令包标识
	SendLength(0x03);
	Sendcmd(0x03);
	temp = 0x01 + 0x03 + 0x03;
	SendCheck(temp);
	data = JudgeStr(2000);
	if (data)
		ensure = data[9];
	else
		ensure = 0xff;
	return ensure;
}
// 搜索指纹 GZ_Search
// 功能:以CharBuffer1或CharBuffer2中的特征文件搜索整个或部分指纹库.若搜索到，则返回页码。
// 参数:  BufferID @ref CharBuffer1	CharBuffer2
// 说明:  模块返回确认字，页码（相配指纹模板）
uint8_t GZ_Search(uint8_t BufferID, uint16_t StartPage, uint16_t PageNum, SearchResult *p)
{
	uint16_t temp;
	uint8_t ensure;
	uint8_t *data;
	SendHead();
	SendAddr();
	SendFlag(0x01); // 命令包标识
	SendLength(0x08);
	Sendcmd(0x04);
	Com_SendData(BufferID);
	Com_SendData(StartPage >> 8);
	Com_SendData(StartPage);
	Com_SendData(PageNum >> 8);
	Com_SendData(PageNum);
	temp = 0x01 + 0x08 + 0x04 + BufferID + (StartPage >> 8) + (uint8_t)StartPage + (PageNum >> 8) + (uint8_t)PageNum;
	SendCheck(temp);
	data = JudgeStr(2000);
	if (data)
	{
		ensure = data[9];
		p->pageID = (data[10] << 8) + data[11];
		p->mathscore = (data[12] << 8) + data[13];
	}
	else
		ensure = 0xff;
	return ensure;
}
// 合并特征（生成模板）GZ_RegModel
// 功能:将CharBuffer1与CharBuffer2中的特征文件合并生成 模板,结果存于CharBuffer1与CharBuffer2
// 说明:  模块返回确认字
uint8_t GZ_RegModel(void)
{
	uint16_t temp;
	uint8_t ensure;
	uint8_t *data;
	SendHead();
	SendAddr();
	SendFlag(0x01); // 命令包标识
	SendLength(0x03);
	Sendcmd(0x05);
	temp = 0x01 + 0x03 + 0x05;
	SendCheck(temp);
	data = JudgeStr(2000);
	if (data)
		ensure = data[9];
	else
		ensure = 0xff;
	return ensure;
}
// 储存模板 GZ_StoreChar
// 功能:将 CharBuffer1 或 CharBuffer2 中的模板文件存到 PageID 号flash数据库位置。
// 参数:  BufferID @ref charBuffer1:0x01	charBuffer1:0x02
//        PageID（指纹库位置号）
// 说明:  模块返回确认字
uint8_t GZ_StoreChar(uint8_t BufferID, uint16_t PageID)
{
	uint16_t temp;
	uint8_t ensure;
	uint8_t *data;
	SendHead();
	SendAddr();
	SendFlag(0x01); // 命令包标识
	SendLength(0x06);
	Sendcmd(0x06);
	Com_SendData(BufferID);
	Com_SendData(PageID >> 8);
	Com_SendData(PageID);
	temp = 0x01 + 0x06 + 0x06 + BufferID + (PageID >> 8) + (uint8_t)PageID;
	SendCheck(temp);
	data = JudgeStr(2000);
	if (data)
		ensure = data[9];
	else
		ensure = 0xff;
	return ensure;
}
// 删除模板 GZ_DeletChar
// 功能:  删除flash数据库中指定ID号开始的N个指纹模板
// 参数:  PageID(指纹库模板号)，N删除的模板个数。
// 说明:  模块返回确认字
uint8_t GZ_DeletChar(uint16_t PageID, uint16_t N)
{
	uint16_t temp;
	uint8_t ensure;
	uint8_t *data;
	SendHead();
	SendAddr();
	SendFlag(0x01); // 命令包标识
	SendLength(0x07);
	Sendcmd(0x0C);
	Com_SendData(PageID >> 8);
	Com_SendData(PageID);
	Com_SendData(N >> 8);
	Com_SendData(N);
	temp = 0x01 + 0x07 + 0x0C + (PageID >> 8) + (uint8_t)PageID + (N >> 8) + (uint8_t)N;
	SendCheck(temp);
	data = JudgeStr(2000);
	if (data)
		ensure = data[9];
	else
		ensure = 0xff;
	return ensure;
}
// 清空指纹库 GZ_Empty
// 功能:  删除flash数据库中所有指纹模板
// 参数:  无
// 说明:  模块返回确认字
uint8_t GZ_Empty(void)
{
	uint16_t temp;
	uint8_t ensure;
	uint8_t *data;
	SendHead();
	SendAddr();
	SendFlag(0x01); // 命令包标识
	SendLength(0x03);
	Sendcmd(0x0D);
	temp = 0x01 + 0x03 + 0x0D;
	SendCheck(temp);
	data = JudgeStr(2000);
	if (data)
		ensure = data[9];
	else
		ensure = 0xff;
	return ensure;
}
// 写系统寄存器 GZ_WriteReg
// 功能:  写模块寄存器
// 参数:  寄存器序号RegNum:4\5\6
// 说明:  模块返回确认字
uint8_t GZ_WriteReg(uint8_t RegNum, uint8_t DATA)
{
	uint16_t temp;
	uint8_t ensure;
	uint8_t *data;
	SendHead();
	SendAddr();
	SendFlag(0x01); // 命令包标识
	SendLength(0x05);
	Sendcmd(0x0E);
	Com_SendData(RegNum);
	Com_SendData(DATA);
	temp = RegNum + DATA + 0x01 + 0x05 + 0x0E;
	SendCheck(temp);
	data = JudgeStr(2000);
	if (data)
		ensure = data[9];
	else
		ensure = 0xff;
	return ensure;
}
// 读系统基本参数 GZ_ReadSysPara
// 功能:  读取模块的基本参数（波特率，包大小等)
// 参数:  无
// 说明:  模块返回确认字 + 基本参数（16bytes）
uint8_t GZ_ReadSysPara(SysPara *p)
{
	uint16_t temp;
	uint8_t ensure;
	uint8_t *data;
	SendHead();
	SendAddr();
	SendFlag(0x01); // 命令包标识
	SendLength(0x03);
	Sendcmd(0x0F);
	temp = 0x01 + 0x03 + 0x0F;
	SendCheck(temp);
	data = JudgeStr(1000);
	if (data)
	{
		ensure = data[9];
		p->GZ_max = (data[14] << 8) + data[15];
		p->GZ_level = data[17];
		p->GZ_addr = (data[18] << 24) + (data[19] << 16) + (data[20] << 8) + data[21];
		p->GZ_size = data[23];
		p->GZ_N = data[25];
	}
	else
		ensure = 0xff;
	return ensure;
}
// 设置模块地址 GZ_SetAddr
// 功能:  设置模块地址
// 参数:  GZ_addr
// 说明:  模块返回确认字
uint8_t GZ_SetAddr(uint32_t GZ_addr)
{
	uint16_t temp;
	uint8_t ensure;
	uint8_t *data;
	SendHead();
	SendAddr();
	SendFlag(0x01); // 命令包标识
	SendLength(0x07);
	Sendcmd(0x15);
	Com_SendData(GZ_addr >> 24);
	Com_SendData(GZ_addr >> 16);
	Com_SendData(GZ_addr >> 8);
	Com_SendData(GZ_addr);
	temp = 0x01 + 0x07 + 0x15 + (uint8_t)(GZ_addr >> 24) + (uint8_t)(GZ_addr >> 16) + (uint8_t)(GZ_addr >> 8) + (uint8_t)GZ_addr;
	SendCheck(temp);
	AS608Addr = GZ_addr; // 发送完指令，更换地址
	data = JudgeStr(2000);
	if (data)
		ensure = data[9];
	else
		ensure = 0xff;
	AS608Addr = GZ_addr;
	if (ensure == 0x00) // 设置地址成功
	{
	}
	return ensure;
}
// 功能： 模块内部为用户开辟了256bytes的FLASH空间用于存用户记事本,
//	该记事本逻辑上被分成 16 个页。
// 参数:  NotePageNum(0~15),Byte32(要写入内容，32个字节)
// 说明:  模块返回确认字
uint8_t GZ_WriteNotepad(uint8_t NotePageNum, uint8_t *Byte32)
{
	uint16_t temp;
	uint8_t ensure, i;
	uint8_t *data;
	SendHead();
	SendAddr();
	SendFlag(0x01); // 命令包标识
	SendLength(36);
	Sendcmd(0x18);
	Com_SendData(NotePageNum);
	for (i = 0; i < 32; i++)
	{
		Com_SendData(Byte32[i]);
		temp += Byte32[i];
	}
	temp = 0x01 + 36 + 0x18 + NotePageNum + temp;
	SendCheck(temp);
	data = JudgeStr(2000);
	if (data)
		ensure = data[9];
	else
		ensure = 0xff;
	return ensure;
}
// 读记事GZ_ReadNotepad
// 功能：  读取FLASH用户区的128bytes数据
// 参数:  NotePageNum(0~15)
// 说明:  模块返回确认字+用户信息
uint8_t GZ_ReadNotepad(uint8_t NotePageNum, uint8_t *Byte32)
{
	uint16_t temp;
	uint8_t ensure, i;
	uint8_t *data;
	SendHead();
	SendAddr();
	SendFlag(0x01); // 命令包标识
	SendLength(0x04);
	Sendcmd(0x19);
	Com_SendData(NotePageNum);
	temp = 0x01 + 0x04 + 0x19 + NotePageNum;
	SendCheck(temp);
	data = JudgeStr(2000);
	if (data)
	{
		ensure = data[9];
		for (i = 0; i < 32; i++)
		{
			Byte32[i] = data[10 + i];
		}
	}
	else
		ensure = 0xff;
	return ensure;
}
// 高速搜索GZ_HighSpeedSearch
// 功能：以 CharBuffer1或CharBuffer2中的特征文件高速搜索整个或部分指纹库。
//		  若搜索到，则返回页码,该指令对于的确存在于指纹库中 ，且登录时质量
//		  很好的指纹，会很快给出搜索结果。
// 参数:  BufferID， StartPage(起始页)，PageNum（页数）
// 说明:  模块返回确认字+页码（相配指纹模板）
uint8_t GZ_HighSpeedSearch(uint8_t BufferID, uint16_t StartPage, uint16_t PageNum, SearchResult *p)
{
	uint16_t temp;
	uint8_t ensure;
	uint8_t *data;
	SendHead();
	SendAddr();
	SendFlag(0x01); // 命令包标识
	SendLength(0x08);
	Sendcmd(0x1b);
	Com_SendData(BufferID);
	Com_SendData(StartPage >> 8);
	Com_SendData(StartPage);
	Com_SendData(PageNum >> 8);
	Com_SendData(PageNum);
	temp = 0x01 + 0x08 + 0x1b + BufferID + (StartPage >> 8) + (uint8_t)StartPage + (PageNum >> 8) + (uint8_t)PageNum;
	SendCheck(temp);
	data = JudgeStr(2000);
	if (data)
	{
		ensure = data[9];
		p->pageID = (data[10] << 8) + data[11];
		printf("pageID:%d\r\n", p->pageID);
		p->mathscore = (data[12] << 8) + data[13];
		printf("mathscore:%d\r\n", p->mathscore);
	}
	else
		ensure = 0xff;
	return ensure;
}
// 读有效模板个数 GZ_ValidTempleteNum
// 功能：读有效模板个数
// 参数: 无
// 说明: 模块返回确认字+有效模板个数ValidN
uint8_t GZ_ValidTempleteNum(uint16_t *ValidN)
{
	uint16_t temp;
	uint8_t ensure;
	uint8_t *data;
	SendHead();
	SendAddr();
	SendFlag(0x01); // 命令包标识
	SendLength(0x03);
	Sendcmd(0x1d);
	temp = 0x01 + 0x03 + 0x1d;
	SendCheck(temp);
	data = JudgeStr(2000);
	if (data)
	{
		ensure = data[9];
		*ValidN = (data[10] << 8) + data[11];
	}
	else
		ensure = 0xff;

	return ensure;
}
// 与AS608握手 GZ_HandShake
// 参数: GZ_Addr地址指针
// 说明: 模块返新地址（正确地址）
uint8_t GZ_HandShake(uint32_t *GZ_Addr)
{
	SendHead();
	SendAddr();
	Com_SendData(0X01);
	Com_SendData(0X00);
	Com_SendData(0X00);
	HAL_Delay(200);
	if (RX_len)
	{
		RX_len = 0;
		if ( // 判断是不是模块返回的应答包
			aRxBuffer[0] == 0XEF && aRxBuffer[1] == 0X01 && aRxBuffer[6] == 0X07)
		{
			*GZ_Addr = (aRxBuffer[2] << 24) + (aRxBuffer[3] << 16) + (aRxBuffer[4] << 8) + (aRxBuffer[5]);
			return 0;
		}
	}
	return 1;
}
// 模块应答包确认码信息解析
// 功能：解析确认码错误信息返回信息
// 参数: ensure
const char *EnsureMessage(uint8_t ensure)
{
	const char *p;
	switch (ensure)
	{
	case 0x00:
		p = "OK";
		break;
	case 0x01:
		p = "数据包接收错误";
		break;
	case 0x02:
		p = "传感器上没有手指";
		break;
	case 0x03:
		p = "录入指纹图像失败";
		break;
	case 0x04:
		p = "指纹图像太干、太淡而生不成特征";
		break;
	case 0x05:
		p = "指纹图像太湿、太糊而生不成特征";
		break;
	case 0x06:
		p = "指纹图像太乱而生不成特征";
		break;
	case 0x07:
		p = "指纹图像正常，但特征点太少（或面积太小）而生不成特征";
		break;
	case 0x08:
		p = "指纹不匹配";
		break;
	case 0x09:
		p = "没搜索到指纹";
		break;
	case 0x0a:
		p = "特征合并失败";
		break;
	case 0x0b:
		p = "访问指纹库时地址序号超出指纹库范围";
	case 0x10:
		p = "删除模板失败";
		break;
	case 0x11:
		p = "清空指纹库失败";
		break;
	case 0x15:
		p = "缓冲区内没有有效原始图而生不成图像";
		break;
	case 0x18:
		p = "读写 FLASH 出错";
		break;
	case 0x19:
		p = "未定义错误";
		break;
	case 0x1a:
		p = "无效寄存器号";
		break;
	case 0x1b:
		p = "寄存器设定内容错误";
		break;
	case 0x1c:
		p = "记事本页码指定错误";
		break;
	case 0x1f:
		p = "指纹库满";
		break;
	case 0x20:
		p = "地址错误";
		break;
	default:
		p = "模块返回确认码有误";
		break;
	}
	return p;
}

///////////////////////以上为AS608驱动函数/////////////////////////
///////////////////////以上为AS608驱动函数/////////////////////////
///////////////////////以上为AS608驱动函数/////////////////////////



