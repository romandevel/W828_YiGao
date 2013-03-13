#include "hyTypes.h"
#include "hyhwTcc7901.h"
#include "hyhwDai.h"
#include "hyhwAudioCfg.h"
#include "hyhwAudioSubsystem.h"
#include "hyhwWm8753.h"

#define RECORD_DEBUG  0

#if  RECORD_DEBUG

#define SOURCE_SAMPLE_RATE		(8000)
#define PLAY_FIFO_LEN			128

#define RECV_DATA_LEN			128
#define PLAY_BUFFER_SIZE		(512)	//16bit
#define SEND_DATA_LEN			128
#define REC_BUFFER_SIZE			(512)	//16bit


#define RECORD_FILE_NAME "C:/MICREC.WAV"
#define RECORD_FILE_ADDR 0x24800000

volatile U32 gu32StopRecFlag = 0;

static char *pTestAlawData = (char *)RECORD_FILE_ADDR;
static U32  gu32RecordSize = 0;
static U32  gu32SendSize = 0;

extern U8	*pAlawData_Send,*pAlawData_Recv;
extern S16 *pRecDataLeft,*pRecDataRight;
extern S16	*pPhone_AudioOutLeft, *pPhone_AudioOutRight;

//
#define SEND_FILE_NAME "C:/RECV.WAV"
#define SEND_FILE_ADDR 0x24A00000
static char *pTestSendData = (char *)SEND_FILE_ADDR;



const U8 WavHead[]={
0x52,0x49,0x46,0x46,0x30,0x72,0x2B,0x00,0x57,0x41,0x56,0x45,0x66,0x6D,0x74,0x20,
0x10,0x00,0x00,0x00,0x01,0x00,0x01,0x00,0x40,0x1F,0x00,0x00,0x88,0x58,0x01,0x00,
0x02,0x00,0x10,0x00,0x64,0x61,0x74,0x61,0x10,0x71,0x2B,0x00                 

};

void JieDebug_AudioRecord(void)
{
   	int readSize,fp;
	
	//if(0!=hyhwmDai_checkRxIntFlag())
	//if(0!=hyhwDai_DmacheckInterruptState(1))
	{//录音
		hyhwAudioSubsys_ISR_record();
		readSize = hyhwAudioSubsys_CopyOutRecordData(pRecDataLeft, NULL, SEND_DATA_LEN/*<<1*/);
		if (readSize > 0)
		{
			readSize = readSize*2;
			memcpy(pTestAlawData, (char *)pRecDataLeft, readSize);
			pTestAlawData += readSize;
			gu32RecordSize +=  readSize;
		}
	}
}

void JieDebug_AudioRecordStart(void)
{
   S16 *pRecBufLeft,*pRecBufRight;

        hyhwAudioCfg_Init();
        
        memset((char *)RECORD_FILE_ADDR,0,1024*1024*3);
        
		hyhwAudioSubsys_Disable();
		
		//接收Rtp音频数据的buffer，存放的是A Law数据，需要转换为PCM后送codec buffer
		pAlawData_Recv			= (U8 *)hsaDecodeAudio_GetInputBuffer();
	//	memset(pAlawData_Recv, 0, RECV_DATA_LEN);
		
		//发送录音数据的buffer，该buffer中存放的是A Law数据，是由录音得到的PCM数转换而来
		pAlawData_Send			= (U8 *)(&(pAlawData_Recv[RECV_DATA_LEN]));
	//	memset(pAlawData_Send, 0, SEND_DATA_LEN);
		
		//播放的管理buffer，需要以 PLAY_BUFFER_SIZE*2 对齐
		pPhone_AudioOutLeft		= (S16 *)hsaDecodeAudio_GetOutputBuffer();
		pPhone_AudioOutLeft		= (S16 *)((U32)((U32)pPhone_AudioOutLeft + ((PLAY_BUFFER_SIZE<<1)-1)) & (~((PLAY_BUFFER_SIZE<<1)-1)));
		pPhone_AudioOutRight	= (S16 *)(&(pPhone_AudioOutLeft[PLAY_BUFFER_SIZE]));
	//	memset((char*)pPhone_AudioOutLeft, 0, PLAY_BUFFER_SIZE<<1);
	//	memset((char*)pPhone_AudioOutRight, 0, PLAY_BUFFER_SIZE<<1);
		
		
		//录音数据的临时缓存，从 Audio 管理的buffer中copy出PCM数据
		pRecDataLeft	= (S16 *)(&(pPhone_AudioOutRight[PLAY_BUFFER_SIZE]));
	//	memset((char*)pRecDataLeft, 0, SEND_DATA_LEN<<1);
		
		//录音的管理bufffer，需要以 REC_BUFFER_SIZE*2 对齐
		pRecBufLeft		= (S16 *)(&(pRecDataLeft[SEND_DATA_LEN<<1]));
		pRecBufLeft		= (S16 *)((U32)((U32)pRecBufLeft + ((REC_BUFFER_SIZE<<1)-1)) & (~((REC_BUFFER_SIZE<<1)-1)));
		pRecBufRight	= (S16 *)(&(pRecBufLeft[REC_BUFFER_SIZE]));
	//	memset((char*)pRecBufLeft, 0, REC_BUFFER_SIZE<<1);
	//	memset((char*)pRecBufRight, 0, REC_BUFFER_SIZE<<1);


		hyhwAudioSubsys_Init(pPhone_AudioOutLeft, pPhone_AudioOutRight, PLAY_BUFFER_SIZE, 1); 
		hyhwAudioSubsys_Init(pRecBufLeft, pRecBufRight, REC_BUFFER_SIZE, 0);
		
		//hyhwCodec_Configuration(hwCodec_PlaybackRecordMic_en);
		//hyhwCodec_GSMModePlayRecordHeadphone();
		//hyhwCodec_GSMModePlayRecordSpeaker();
		hyhwCodec_Configuration(hwCodec_RecordMic_en);
	    hyhwAudioCfg_SetSamplingRate(SOURCE_SAMPLE_RATE);

		hyhwInt_ConfigCodecISR(JieDebug_AudioRecord);
		hyhwAudioSubsys_ClearAudioBuffer();

		HYC_AdjustVolume(11); //24 jie 5-20
		//hyhwWm8753_SetAudioOutVolume(WM8753_OUT12, 0x60);
	//	hyhwCodec_SetAudioInVolume(0x28);//录音最大增益
	//	hyhwCodec_SetADCVolume(0xf0);//4-22
		
		//启动录音
		hyhwAudioSubsys_EnableRecord(); 
}


void JieDebug_RecordMic(U8 *tempbuf, U32 readSize)
{
	if (readSize > 0 && gu32RecordSize < 0x200000)
	{
		
		
		readSize = readSize*2;
		memcpy(pTestAlawData, (char *)tempbuf, readSize);
		pTestAlawData += readSize;
		gu32RecordSize +=  readSize;
	
	}
    
}

void JieDebug_RecvMic(U8 *tempbuf, U32 readSize)
{
	if (readSize > 0 && gu32SendSize < 0x200000)
	{
		
		
		//readSize = readSize*2;
		memcpy(pTestSendData, (char *)tempbuf, readSize);
		pTestSendData += readSize;
		gu32SendSize +=  readSize;
	
	}
    
}

//按键操作录音
void JieDebug_AudioKeyRecStop(void)
{
    int fp;
    U32 totalsize;
    U8  tempbuf[80]={0};
    U32 headlen = sizeof(WavHead);
    
    //if (0 == gu32StopRecFlag) return;
    
    //hyhwAudioSubsys_Disable();
   
    memcpy(tempbuf,WavHead,sizeof(WavHead));
    
    //if ()
    
    totalsize =  sizeof(WavHead)+gu32RecordSize;
       
    tempbuf[4] = totalsize&0xFF;
    tempbuf[5] = (totalsize>>8)&0xFF;
    tempbuf[6] = (totalsize>>16)&0xFF;
    tempbuf[7] = (totalsize>>24)&0xFF;
    
    tempbuf[headlen-4] = gu32RecordSize&0xFF;
    tempbuf[headlen-3] = (gu32RecordSize>>8)&0xFF;
    tempbuf[headlen-2] = (gu32RecordSize>>16)&0xFF;
    tempbuf[headlen-1] = (gu32RecordSize>>24)&0xFF;
     

    fp = AbstractFileSystem_Open(RECORD_FILE_NAME,2);
    if (fp < 0)
    {
        fp = AbstractFileSystem_Create(RECORD_FILE_NAME);
        AbstractFileSystem_Write(fp,(char *)tempbuf,headlen);     
    	AbstractFileSystem_Write(fp,(char *)RECORD_FILE_ADDR,gu32RecordSize);            			
    }
    else
    {
        AbstractFileSystem_Write(fp,(char *)tempbuf,headlen); 
        AbstractFileSystem_Write(fp,(char *)RECORD_FILE_ADDR,gu32RecordSize);
    	  
    }

    AbstractFileSystem_Close(fp);
	   
    gu32RecordSize = 0;
    pTestAlawData = (char *)RECORD_FILE_ADDR;
}

void JieDebug_AudioRecordStop(void)
{
    int fp;
    U32 totalsize;
    U8  tempbuf[80]={0};
    U32 headlen = sizeof(WavHead);
    
    if (0 == gu32StopRecFlag) return;
    
    //hyhwAudioSubsys_Disable();
   
    memcpy(tempbuf,WavHead,sizeof(WavHead));
    
    //if ()
    
    totalsize =  sizeof(WavHead)+gu32RecordSize;
       
    tempbuf[4] = totalsize&0xFF;
    tempbuf[5] = (totalsize>>8)&0xFF;
    tempbuf[6] = (totalsize>>16)&0xFF;
    tempbuf[7] = (totalsize>>24)&0xFF;
    
    tempbuf[headlen-4] = gu32RecordSize&0xFF;
    tempbuf[headlen-3] = (gu32RecordSize>>8)&0xFF;
    tempbuf[headlen-2] = (gu32RecordSize>>16)&0xFF;
    tempbuf[headlen-1] = (gu32RecordSize>>24)&0xFF;
     

    fp = AbstractFileSystem_Open(RECORD_FILE_NAME,2);
    if (fp < 0)
    {
        fp = AbstractFileSystem_Create(RECORD_FILE_NAME);
        AbstractFileSystem_Write(fp,(char *)tempbuf,headlen);     
    	AbstractFileSystem_Write(fp,(char *)RECORD_FILE_ADDR,gu32RecordSize);            			
    }
    else
    {
        AbstractFileSystem_Write(fp,(char *)tempbuf,headlen); 
        AbstractFileSystem_Write(fp,(char *)RECORD_FILE_ADDR,gu32RecordSize);
    	  
    }

    AbstractFileSystem_Close(fp);
	   
    gu32RecordSize = 0;
    pTestAlawData = (char *)RECORD_FILE_ADDR;
    
//SEND
    memcpy(tempbuf,WavHead,sizeof(WavHead));
    
    totalsize =  sizeof(WavHead)+gu32SendSize;
       
    tempbuf[4] = totalsize&0xFF;
    tempbuf[5] = (totalsize>>8)&0xFF;
    tempbuf[6] = (totalsize>>16)&0xFF;
    tempbuf[7] = (totalsize>>24)&0xFF;
    
    tempbuf[headlen-4] = gu32SendSize&0xFF;
    tempbuf[headlen-3] = (gu32SendSize>>8)&0xFF;
    tempbuf[headlen-2] = (gu32SendSize>>16)&0xFF;
    tempbuf[headlen-1] = (gu32SendSize>>24)&0xFF;
     

    fp = AbstractFileSystem_Open(SEND_FILE_NAME,2);
    if (fp < 0)
    {
        fp = AbstractFileSystem_Create(SEND_FILE_NAME);
        AbstractFileSystem_Write(fp,(char *)tempbuf,headlen);     
    	AbstractFileSystem_Write(fp,(char *)SEND_FILE_ADDR,gu32SendSize);            			
    }
    else
    {
        AbstractFileSystem_Write(fp,(char *)tempbuf,headlen); 
        AbstractFileSystem_Write(fp,(char *)SEND_FILE_ADDR,gu32SendSize);
    	  
    }

    AbstractFileSystem_Close(fp);
	   
    gu32SendSize = 0;
    pTestSendData = (char *)SEND_FILE_ADDR;
    
    gu32StopRecFlag = 0;
    
}

#endif
