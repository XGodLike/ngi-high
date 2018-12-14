#include "string.h"
#include "silk/SKP_Silk_SDK_API.h"
#include <stdlib.h>

#define MAX_NB_BYTES 200
#define  BOOL int

#define MAX_BYTES_PER_FRAME    250 // Equals peak bitrate of 100 kbps 
#define MAX_INPUT_FRAMES        5
#define FRAME_LENGTH_MS         20
#define MAX_API_FS_KHZ          48
//#define MAX_BYTES_PER_FRAME_DECODER     1024
#define MAX_INPUT_FRAMES        5
#define MAX_FRAME_LENGTH        480
#define FRAME_LENGTH_MS         20
#define MAX_API_FS_KHZ          48
#define MAX_LBRR_DELAY          2

/** modeID for the defined narrowband mode */
#define SPEEX_MODEID_NB 0

/** modeID for the defined wideband mode */
#define SPEEX_MODEID_WB 1

/** modeID for the defined ultra-wideband mode */
#define SPEEX_MODEID_UWB 2



struct CSilkHandle
{
	SKP_SILK_SDK_EncControlStruct encControl; 
	int smplsSinceLastPacket;
	void* pHandle;
};

class CVcyberSpeex
{
public:
	CVcyberSpeex(void);
	~CVcyberSpeex(void);

	static char* SilkEncode(void** pHandle,BOOL bEnd,char* pIn,int nInLen,int nBandMode,int nQuality,int* nOutLen);
	static char* SilkDecode(void** pHandle,BOOL bEnd,char* pIn,int nInLen,int nBandMode,int* nOutLen);
};

