#include "temux.h"

#define  MAX_VALUE_OF_7BIT              127


MuxControlStructT    MuxCtrlStruct;

const  uint8  MuxFcsTable[] =
{
    /*reversed, 8-bit, poly=0x07*/
    0x00, 0x91, 0xE3, 0x72, 0x07, 0x96, 0xE4, 0x75, 0x0E, 0x9F, 0xED, 0x7C, 0x09, 0x98, 0xEA, 0x7B,
    0x1C, 0x8D, 0xFF, 0x6E, 0x1B, 0x8A, 0xF8, 0x69, 0x12, 0x83, 0xF1, 0x60, 0x15, 0x84, 0xF6, 0x67,
    0x38, 0xA9, 0xDB, 0x4A, 0x3F, 0xAE, 0xDC, 0x4D, 0x36, 0xA7, 0xD5, 0x44, 0x31, 0xA0, 0xD2, 0x43,
    0x24, 0xB5, 0xC7, 0x56, 0x23, 0xB2, 0xC0, 0x51, 0x2A, 0xBB, 0xC9, 0x58, 0x2D, 0xBC, 0xCE, 0x5F,
    0x70, 0xE1, 0x93, 0x02, 0x77, 0xE6, 0x94, 0x05, 0x7E, 0xEF, 0x9D, 0x0C, 0x79, 0xE8, 0x9A, 0x0B,
    0x6C, 0xFD, 0x8F, 0x1E, 0x6B, 0xFA, 0x88, 0x19, 0x62, 0xF3, 0x81, 0x10, 0x65, 0xF4, 0x86, 0x17,
    0x48, 0xD9, 0xAB, 0x3A, 0x4F, 0xDE, 0xAC, 0x3D, 0x46, 0xD7, 0xA5, 0x34, 0x41, 0xD0, 0xA2, 0x33,
    0x54, 0xC5, 0xB7, 0x26, 0x53, 0xC2, 0xB0, 0x21, 0x5A, 0xCB, 0xB9, 0x28, 0x5D, 0xCC, 0xBE, 0x2F,
    0xE0, 0x71, 0x03, 0x92, 0xE7, 0x76, 0x04, 0x95, 0xEE, 0x7F, 0x0D, 0x9C, 0xE9, 0x78, 0x0A, 0x9B,
    0xFC, 0x6D, 0x1F, 0x8E, 0xFB, 0x6A, 0x18, 0x89, 0xF2, 0x63, 0x11, 0x80, 0xF5, 0x64, 0x16, 0x87,
    0xD8, 0x49, 0x3B, 0xAA, 0xDF, 0x4E, 0x3C, 0xAD, 0xD6, 0x47, 0x35, 0xA4, 0xD1, 0x40, 0x32, 0xA3,
    0xC4, 0x55, 0x27, 0xB6, 0xC3, 0x52, 0x20, 0xB1, 0xCA, 0x5B, 0x29, 0xB8, 0xCD, 0x5C, 0x2E, 0xBF,
    0x90, 0x01, 0x73, 0xE2, 0x97, 0x06, 0x74, 0xE5, 0x9E, 0x0F, 0x7D, 0xEC, 0x99, 0x08, 0x7A, 0xEB,
    0x8C, 0x1D, 0x6F, 0xFE, 0x8B, 0x1A, 0x68, 0xF9, 0x82, 0x13, 0x61, 0xF0, 0x85, 0x14, 0x66, 0xF7,
    0xA8, 0x39, 0x4B, 0xDA, 0xAF, 0x3E, 0x4C, 0xDD, 0xA6, 0x37, 0x45, 0xD4, 0xA1, 0x30, 0x42, 0xD3,
    0xB4, 0x25, 0x57, 0xC6, 0xB3, 0x22, 0x50, 0xC1, 0xBA, 0x2B, 0x59, 0xC8, 0xBD, 0x2C, 0x5E, 0xCF
};


static void  fcsCalculate(MuxWriteInfoT *pWriteInfo, uint8 dlc, MuxFrameT frameType) 
{
	MuxRWStateT state;
	uint8       tempByte;
	uint8       fcs = 0xff;

	state = MUX_RW_STATE_ADDR;
	while(state != MUX_RW_STATE_FCS)
	{
		switch(state)
		{
		case MUX_RW_STATE_ADDR:
			tempByte = pWriteInfo->frame.addr;
			state = MUX_RW_STATE_CONTROL;
			break;

		case MUX_RW_STATE_CONTROL:
			tempByte = pWriteInfo->frame.control;

			if(pWriteInfo->curMode == MUX_TRANSP_NONE)
			{
				state = MUX_RW_STATE_LENGTH_LOW;
			}
			else if( frameType == MUX_UIH)
			{
				state = MUX_RW_STATE_FCS;
			}
			else
			{
				state = MUX_RW_STATE_SIGNAL;
			}
			break;

		case MUX_RW_STATE_LENGTH_LOW:
			tempByte = pWriteInfo->frame.length[0];
			if( (tempByte & 0x01) == 0)
			{
				state = MUX_RW_STATE_LENGTH_HIGH;
			}
			else if( frameType == MUX_UIH)
			{
				state = MUX_RW_STATE_FCS;
			}
			else
			{
				state = MUX_RW_STATE_SIGNAL;
			}
			break;

		case MUX_RW_STATE_LENGTH_HIGH:
			tempByte = pWriteInfo->frame.length[1];

			if( frameType == MUX_UIH)
				state = MUX_RW_STATE_FCS;
			else
				state = MUX_RW_STATE_SIGNAL;
			break;

		case MUX_RW_STATE_SIGNAL:  /*must be UI or I frame*/
			if(pWriteInfo->frame.writeInfo.buffer == NULL)
				state = MUX_RW_STATE_FCS;
			else
				state = MUX_RW_STATE_INFO;

			if(pWriteInfo->frame.isSignalOcetExist)
			{
				tempByte = pWriteInfo->frame.signal;
			}
			else
			{
				continue;
			}
			break;

		case MUX_RW_STATE_INFO:
			tempByte = pWriteInfo->frame.writeInfo.buffer[pWriteInfo->frame.writeInfo.pos++];
			if(pWriteInfo->frame.writeInfo.pos >= pWriteInfo->frame.writeInfo.totalLen)
			{
				pWriteInfo->frame.writeInfo.pos = 0;
				state = MUX_RW_STATE_FCS;
			}
			break;

		case MUX_RW_STATE_START:
		case MUX_RW_STATE_FCS:
		case MUX_RW_STATE_END:
		default:
			tempByte = 0;
			//	     MUX_FAULT();
			break;
		}

		fcs = MuxFcsTable[fcs^tempByte];
	}
	pWriteInfo->frame.fcs = 0xff -fcs;

	return;
}

static bool fcsCheck(uint8 *pData, uint16 len, MuxFrameT frameType)
{
	bool            result;
	uint16          i;
	uint8          *pTemp;
	uint8           dlc;
	uint8           fcs = 0xff;
	uint16          size;
	MuxDataDlcObjT *pDlc;

	pTemp = pData;
	dlc = MUX_GET_FIELD(*pTemp, MUX_DLC_FIELD);
	pDlc = MuxCtrlStruct.pDlcCtrl[dlc];

	if(frameType != MUX_UIH)
	{
		for(i = 0; i < len; i++)
		{
			fcs = MuxFcsTable[fcs^*pTemp++];
		}

		result = (fcs == MUX_VALID_CHECKSUM);
	}
	else
	{
		size = 2;    /*addr and control field*/
		pTemp += 2;  /*point to addr[0]*/

		if(MuxCtrlStruct.transpType == MUX_TRANSP_NONE)
		{
			if(MUX_GET_FIELD(*pTemp, MUX_EABIT_FIELD) == 0)
				size += 2;
			else
				size += 1;
		}

		pTemp = pData;
		for(i = 0; i < size; i++)
		{
			fcs = MuxFcsTable[fcs^*pTemp++];
		}

		fcs = MuxFcsTable[fcs^(pData[len -1])];
		result = (fcs == MUX_VALID_CHECKSUM); 
	}

	return result;    
}


static bool MuxMsFcFlag = FALSE;

/*
* MuxMsFs
*
* @param dlc     :the data link channel
* @param isFc    :set TRUE if Flow control is on, or FALSE
*/
void MuxMsFc(uint8 dlc, bool isFc)
{
	MuxMsFcFlag = isFc;

}

bool  MuxIsTxPermit(uint8 dlc)
{
	bool txOk = TRUE;

	if(dlc == 0)
		return txOk;

	return !txOk;
}

/*
* MuxFrameConstruct
*
* @param dlc   :data link channel
* @param state :frame state, receive or send, command or response
* @param pData :data buffer
* @param len   :data len
* @param pbit  :P/F bit in control field 
* 
* @ret TRUE if send frame succeed, or FALSE
*/
int MuxFrameConstruct(int fd,uint8 dlc, MuxCtrlHdlrStateT state, MuxFrameT framType, uint8 *pData, uint16 len, bool pbit)
{
	char           *sendBuf = NULL;
	char           *psend = NULL;
	bool            urgent = FALSE;
	//MuxWriteInfoT  *pFrameInfo;
	MuxWriteInfoT  tFrameInfo;
	int rc = 0,send_length = 0;

	//if((state == MUX_CMD_SEND)  && pbit && MuxCtrlStruct.pBitSend )
	//	return FALSE;

	//pFrameInfo = (MuxWriteInfoT*)malloc(sizeof(MuxWriteInfoT));
	//if (pFrameInfo == NULL)
	//	return FALSE;

	//memset(pFrameInfo, 0, sizeof(MuxWriteInfoT));
	memset((char *)&tFrameInfo, 0, sizeof(MuxWriteInfoT));
	
	tFrameInfo.isValid = TRUE;
	tFrameInfo.frame.flagStart = MUX_BASIC_MODE_FLAG;
	tFrameInfo.frame.flagEnd = MUX_BASIC_MODE_FLAG;

	/*Address field*/
	tFrameInfo.frame.addr = MUX_MAKE_FIELD(dlc, MUX_DLC_FIELD);
	tFrameInfo.frame.addr |= MUX_MAKE_FIELD(1, MUX_EABIT_FIELD);
	//if( state == MUX_RES_SEND)
	if( state == MUX_CMD_SEND)
	{
		tFrameInfo.frame.addr |= MUX_MAKE_FIELD(1, MUX_CRBIT_FIELD);
	}

	/*Control field*/
	tFrameInfo.frame.control = framType;
	if(pbit)
	{
		urgent = TRUE;
		if(state == MUX_CMD_SEND)
		{
			MuxCtrlStruct.pBitSend = TRUE;
		}
		tFrameInfo.frame.control |= MUX_STD_MASK(MUX_PBIT_FIELD);
	}

	/*length field*/
	if(len > MAX_VALUE_OF_7BIT)
	{
		tFrameInfo.frame.length[0] = (len & 0x7f) << 1;
		tFrameInfo.frame.length[1] = ((((uint16)len) << 1) & 0xff00)>>8;
		//tFrameInfo.frame.length[1] = ((((uint32)len) << 1) & 0xff00)>>8;
	}
	else
	{
		tFrameInfo.frame.length[0] = (len << 1) |0x01;
		tFrameInfo.frame.length[1] = 0x00;
	}	

	/*signal field , in ctrl channel there is no signal field*/
	//tFrameInfo.frame.isSignalOcetExist = FALSE;

	/*information field*/
	tFrameInfo.frame.writeInfo.buffer = pData;
	tFrameInfo.frame.writeInfo.totalLen = len;
	tFrameInfo.frame.writeInfo.pos = 0;

	tFrameInfo.curMode = MuxCtrlStruct.transpType;
	tFrameInfo.state = MUX_RW_STATE_START;
	tFrameInfo.priority = 0;

	//tFrameInfo.resInfo.TaskId = EXE_DUMMY_TASK;
	tFrameInfo.isFollowed = FALSE;

	/*frame is ready*/
	fcsCalculate(&tFrameInfo, dlc, framType);

	sendBuf = (char *)malloc(7 + tFrameInfo.frame.writeInfo.totalLen);
	if (sendBuf == NULL)
	{
		//free(pFrameInfo);
		return FALSE;
	}
	psend = sendBuf;
	*sendBuf++ = tFrameInfo.frame.flagStart;
	*sendBuf++ = tFrameInfo.frame.addr;
	*sendBuf++ = tFrameInfo.frame.control;
	*sendBuf++ = tFrameInfo.frame.length[0];
	if(tFrameInfo.frame.length[1] != 0)
	{
		*sendBuf++ = tFrameInfo.frame.length[1];
	}
	memcpy(sendBuf, tFrameInfo.frame.writeInfo.buffer, tFrameInfo.frame.writeInfo.totalLen);
	sendBuf   += tFrameInfo.frame.writeInfo.totalLen;
	*sendBuf++ = tFrameInfo.frame.fcs;
	*sendBuf   = tFrameInfo.frame.flagEnd;
	
	if(0)//!MuxIsTxPermit(dlc) || MuxCtrlStruct.isTeSleep != MUX_TE_STATE_WAKEUP)
	{
		//将writeinfo添加至待发队列
		if(MuxCtrlStruct.isTeSleep == MUX_TE_STATE_SLEEP)
		{
			MuxCtrlStruct.isUeWakeupTe = TRUE;
			MuxCtrlStruct.isTeSleep = MUX_TE_STATE_WAKEUPING;
		}
	}
	else
	{
		//发送数据
		
		if(tFrameInfo.frame.length[1] != 0)
		{
			send_length = 7 + tFrameInfo.frame.writeInfo.totalLen;			
		}
		else
		{
			send_length = 6 + tFrameInfo.frame.writeInfo.totalLen;		
		}
		rc = gsmcdma_uart_write(fd,psend,send_length);
		/*while(rc < send_length)
		{//kong 2011-05-12  该处不会执行   并且该处代码有误!!!!!
			send_length -= rc;
			rc = gsmcdma_uart_write(fd,psend,send_length);
		}
		if (rc < 0)
		{			
		hyUsbPrintf("gsm mux free src = %x   now = %x\r\n",(U32)psend,(U32)sendBuf);
			//2011-4-20 kong  内存释放错误 psend 才是真正的首地址,sendBuf是偏移后的地址,直接释放该地址可能导致进程直接退出
			//free(sendBuf);
			free(psend);
			//free(pFrameInfo);
			return rc;
		}*/
	}
	free(psend);
	//free(pFrameInfo);
	return rc;
}

/*
* MuxFrameDestruct
*
* @param pData     :the data for analyse
* @param len       :data len
* @param pFrameInfo:out data struct 
* 
* @ret TRUE if send frame succeed, or FALSE
*/
bool MuxFrameDestruct(uint8 *pData, uint16 len, MuxWriteInfoT *pFrameInfo)
{
	uint8          *pTmpData;
	uint16          tmpSize;
	int             dlc;
	MuxFrameT       framType;
	//MuxWriteInfoT  *pFrameInfo;

	//*ppFrameInfo = pFrameInfo = (MuxWriteInfoT*)malloc(sizeof(MuxWriteInfoT));
	//if (pFrameInfo == NULL)
	//	return FALSE;
	memset(pFrameInfo, 0, sizeof(MuxWriteInfoT));

	pTmpData = pData+1;
	tmpSize = len-2;

	if (*pData++ != 0xf9)
	{
		return FALSE;
	}

	/*Address field*/
	pFrameInfo->frame.addr = *pData++;

	/*Control field*/
	pFrameInfo->frame.control = *pData++;
	framType = (MuxFrameT)(pFrameInfo->frame.control&0xef);    

	/*length field*/
	if(MUX_GET_FIELD(*pData, MUX_EABIT_FIELD) == 0)  //
	{
		pFrameInfo->frame.length[0] = ((*pData++ >> 1) & 0x7f);
		pFrameInfo->frame.length[1] = *pData++;
		len = ((uint32)pFrameInfo->frame.length[1] << 7) | pFrameInfo->frame.length[0];
		//pFrameInfo->frame.length[1] = (((uint32)(*pData++) << 8) & 0xff00) >> 1;
		//len = pFrameInfo->frame.length[1] | pFrameInfo->frame.length[0];
	}
	else 
	{
		pFrameInfo->frame.length[0] = ((*pData++ >> 1) & 0x7f);
		len = pFrameInfo->frame.length[0];
	}

	/*signal field , in ctrl channel there is no signal field*/
	//pFrameInfo->frame.isSignalOcetExist = FALSE;

	/*information field*/
	pFrameInfo->frame.writeInfo.buffer = pData;
	pFrameInfo->frame.writeInfo.totalLen = len;
	pFrameInfo->frame.writeInfo.pos = 0;

	/*frame is ready*/
	if (fcsCheck(pTmpData, tmpSize, framType) == FALSE)
	{
		//free(pFrameInfo);
		return FALSE;
	}

	return TRUE;
}


int mux_msc_get(int fd, MuxWriteInfoT *pFrameInfo, uint8 *DLC, uint8 *FC)
{
	char *pData = pFrameInfo->frame.writeInfo.buffer;			

	//Type | Len | DLCI | Signal | 
	if (pData[0] == ( MUX_CTRL_MSG_MSC | MUX_MAKE_FIELD(1, MUX_EABIT_FIELD)  | MUX_MAKE_FIELD(1, MUX_CRBIT_FIELD)))
	{
		*DLC = MUX_GET_FIELD(pData[2], MUX_DLC_FIELD);
		if(*DLC <= 2)
		{
			*FC = (bool)MUX_GET_FIELD(pData[3], MUX_FC_FIELD);

			if(0x06 == pData[3])
			{
				char msc_dlc2[] = {0xF9,0x01,0xFF,0x0B,0xE1,0x07,0x0B,0x07,0x01,0x6C,0xF9};

			    gsmcdma_uart_write(fd, msc_dlc2, 11);
			}
			else if(0x0C == pData[3])
			{
				char msc_dlc2[] = {0xF9,0x01,0xFF,0x0B,0xE1,0x07,0x0B,0x0D,0x01,0x6C,0xF9};

			    gsmcdma_uart_write(fd, msc_dlc2, 11);
			}
			else if(0x8C == pData[3])
			{
				char msc_dlc2[] = {0xF9,0x01,0xFF,0x0B,0xE1,0x07,0x0B,0x8D,0x01,0x6C,0xF9};

			    gsmcdma_uart_write(fd, msc_dlc2, 11);
			}
			
			return 1;//This is	MSC
		}
	}
	return 0;//not MSC
}



