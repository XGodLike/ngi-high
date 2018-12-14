//#include "StdAfx.h"
#include "VcyberSpeex.h"
//#include "stdio.h"
#define FALSE   0
#define TRUE    1

CVcyberSpeex::CVcyberSpeex(void)
{
}


CVcyberSpeex::~CVcyberSpeex(void)
{
}

char* CVcyberSpeex::SilkEncode(void** pHandle,BOOL bEnd,char* pIn,int nInLen,int nBandMode,int nQuality,int* nOutLen)
{
	if(nBandMode != 0 && nBandMode != 1) nBandMode = 1;
	SKP_int32 ret;
	SKP_int32 tempInLen;
	SKP_int16 tempOutLen;
	SKP_uint8 payload[ MAX_BYTES_PER_FRAME * MAX_INPUT_FRAMES ];
	SKP_int16 in[ FRAME_LENGTH_MS * MAX_API_FS_KHZ * MAX_INPUT_FRAMES ];
	SKP_int32 encSizeBytes;

	SKP_int32 API_fs_Hz = 16000;
	SKP_int32 max_internal_fs_Hz = 16000;
	if(nBandMode == 0)  
	{
		API_fs_Hz = 12000;
		max_internal_fs_Hz = 8000;
	}
	else if(nBandMode == 1) 
	{
		API_fs_Hz = 24000;
		max_internal_fs_Hz = 16000;
	}
	SKP_int32 targetRate_bps = 8000;
	if(nQuality<0&&nQuality>10)
	{
		nQuality =10;
	}
	targetRate_bps = targetRate_bps*(nBandMode+1) + 800*(nBandMode+1)*nQuality;


	//API_fs_Hz = 24000;
	//targetRate_bps = 25000;
	SKP_int32 packetSize_ms = 20;
	SKP_int32 frameSizeReadFromFile_ms = 20;
	SKP_int32 packetLoss_perc = 0;
	SKP_int32 complexity_mode = 1;
	SKP_int32 DTX_enabled = 0, INBandFEC_enabled = 0, quiet = 0;
	SKP_int32 nOutCount = 0;
	SKP_int32 nReadCount = 0;
	SKP_int32 readLen = frameSizeReadFromFile_ms * API_fs_Hz/1000*2;
	SKP_SILK_SDK_EncControlStruct encStatus;  // Struct for status of encoder
	CSilkHandle* csh  =NULL;
	char* pOut = NULL;

	//if(nBandMode == 2) nFrameSize = 640;


	if(*pHandle ==NULL)
	{
		csh  = (CSilkHandle*)malloc(sizeof( CSilkHandle));
		csh ->smplsSinceLastPacket = 0;
		ret = SKP_Silk_SDK_Get_Encoder_Size( &encSizeBytes );
		if( ret ) {
			exit( 0 );
		}
		csh->pHandle = malloc( encSizeBytes );
		ret = SKP_Silk_SDK_InitEncoder( csh->pHandle , &encStatus );
		if( ret ) {
			exit( 0 );
		}
		csh->encControl.API_sampleRate        = API_fs_Hz;
		csh->encControl.maxInternalSampleRate = max_internal_fs_Hz;
		csh->encControl.packetSize            = ( packetSize_ms * API_fs_Hz ) / 1000;
		csh->encControl.packetLossPercentage  = packetLoss_perc;
		csh->encControl.useInBandFEC          = INBandFEC_enabled;
		csh->encControl.useDTX                = DTX_enabled;
		csh->encControl.complexity            = complexity_mode;
		csh->encControl.bitRate               = ( targetRate_bps > 0 ? targetRate_bps : 0 );
		*pHandle = csh;
	}
	else
	{
		csh = (CSilkHandle*)(*pHandle);
	}
	while( 1 ) 
	{
		tempOutLen = MAX_BYTES_PER_FRAME * MAX_INPUT_FRAMES;
		tempInLen = nReadCount+readLen>nInLen?nInLen%readLen:readLen;
		memcpy(in,pIn + nReadCount,tempInLen);
		nReadCount+= tempInLen;
		if( tempInLen/2< ( ( frameSizeReadFromFile_ms * API_fs_Hz ) / 1000 ) ) {
			break;
		}
		ret = SKP_Silk_SDK_Encode( csh->pHandle, &(csh->encControl), in, tempInLen/2, payload, &tempOutLen );
		csh ->smplsSinceLastPacket += ( SKP_int )tempInLen/2;
		if( ( ( 1000 * csh ->smplsSinceLastPacket) / API_fs_Hz ) == packetSize_ms ) 
		{
			pOut = (char*)realloc(pOut,nOutCount+tempOutLen+2);
			memcpy(pOut + nOutCount,&tempOutLen,2);
			nOutCount+=2;
			memcpy(pOut + nOutCount,payload,tempOutLen);
			nOutCount += tempOutLen;	
			csh ->smplsSinceLastPacket = 0;
		}
	}
	if(bEnd)
	{
		free( csh->pHandle );
		free(csh);
		*pHandle = NULL;
	}
	if(nOutLen != 0) *nOutLen = nOutCount;
	return pOut;
}

char* CVcyberSpeex::SilkDecode(void** pHandle,BOOL bEnd,char* pIn,int nInLen,int nBandMode,int* nOutLen)
{
	if(nBandMode != 0 && nBandMode != 1) nBandMode = 1;
	SKP_int16 ret, len, tot_len;
	SKP_int16 nBytes;
	SKP_uint8 *payload=NULL;
	char*pOut = NULL;
	SKP_int16 out[ ( ( FRAME_LENGTH_MS * MAX_API_FS_KHZ ) << 1 ) * MAX_INPUT_FRAMES ], *outPtr;
	SKP_int32  API_Fs_Hz;
	SKP_int32 decSizeBytes;
	SKP_int32 frames;
	SKP_int32 nOutCount = 0;
	SKP_int32 nReadCount = 0;
	if(nBandMode == 0)  
	{
		API_Fs_Hz = 12000;
	}
	else if(nBandMode == 1) 
	{
		API_Fs_Hz = 24000;
	}
	SKP_SILK_SDK_DecControlStruct DecControl;
	if( API_Fs_Hz == 0 ) {
		DecControl.API_sampleRate = 16000;
	} else {
		DecControl.API_sampleRate = API_Fs_Hz;
	}
	DecControl.framesPerPacket = 1;
	if(*pHandle ==NULL)
	{
		/* Create decoder */
		ret = SKP_Silk_SDK_Get_Decoder_Size( &decSizeBytes );
		if( ret ) {
			exit(0);
		}
		*pHandle = malloc( decSizeBytes );
		ret = SKP_Silk_SDK_InitDecoder( *pHandle );
		if( ret ) {
			exit(0);
		}
	}

	while( 1 ) {
		memcpy( &nBytes,pIn + nReadCount, sizeof( SKP_int16 ));
		nReadCount+= sizeof( SKP_int16 );
		payload =(unsigned char*)malloc(nBytes);
		memcpy( payload,pIn + nReadCount, nBytes);
		nReadCount+=nBytes;
		frames = 0;
		outPtr = out;
		tot_len = 0;
		len =  ( ( FRAME_LENGTH_MS * MAX_API_FS_KHZ ) << 1 ) * MAX_INPUT_FRAMES;
		do {
			ret = SKP_Silk_SDK_Decode( *pHandle, &DecControl, 0, payload, nBytes, outPtr, &len );
			if( ret ) {
				exit(0);
			}
			frames++;
			outPtr  += len;
			tot_len += len;
			if( frames > MAX_INPUT_FRAMES ) {
				/* Hack for corrupt stream that could generate too many frames */
				outPtr  = out;
				tot_len = 0;
				frames  = 0;
			}
		} while( DecControl.moreInternalDecoderFrames ); 
		free(payload);
		pOut = (char*)realloc(pOut,nOutCount+tot_len*2);
		memcpy(pOut + nOutCount,out,tot_len*2);
		nOutCount += tot_len*2;	
		if(nReadCount == nInLen)
		{
			break;
		}
	}
	if(bEnd)
	{
		free( *pHandle );
		*pHandle = NULL;
	}
	if(nOutLen != 0) *nOutLen = nOutCount;
	return pOut;
}
