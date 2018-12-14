#ifndef _TSILK_VERSION
#include "tsilk.h"
#endif

#if _TSILK_VERSION==0x782AD46F

#include "silk/SKP_Silk_SDK_API.h"
#include <stdlib.h>
#include <string.h>

#ifndef g_aloc
#	define g_aloc(p,n) do{void * x=p;do{p=realloc(x,n);}while(!p);}while(0)
#endif

#ifndef g_free
#	define g_free(x) do{if(x){free(x);x=0;}}while(0)
#endif

#ifndef g_zero
#	define g_zero(p,n) memset(p, 0, n)
#endif

typedef union _UnTed
{
	SKP_SILK_SDK_EncControlStruct en;
	SKP_SILK_SDK_DecControlStruct de;
} UnTed;

typedef struct _StData
{
	char * buf;
	int length;
	int size;
} StData;

typedef struct _StTsilk
{
	void * hd;
	int edc;
	int level;
	int hz;
	UnTed ted;
	StData in;
	StData out;
} StTsilk;

void * tsilk_init(void * ht, int edc, int hz, int level)
{
	int err = -1;
	StTsilk * ptk = (StTsilk *)ht;

	{
		int nIsNew = 0;

		if(!ptk)
		{
			g_aloc(ptk, sizeof(StTsilk));
			g_zero(ptk, sizeof(StTsilk));
			nIsNew = 1;
		}

		if(nIsNew)
		{
			ptk->edc = edc == 2 ? 2 : 1;
			ptk->hz = hz == 8000 ? 8000 : 16000;
			ptk->level = (level>0 && level<11) ? level : 10;
		}
		else
		{
			if(edc)
			{
				ptk->edc = edc == 2 ? 2 : 1;
			}

			if(hz)
			{
				ptk->hz = hz == 8000 ? 8000 : 16000;
			}

			if(level)
			{
				ptk->level = (level>0 && level<11) ? level : 10;
			}
		}
	}

	{
		int rt = 0;
		int nSize = 0;

		if(ptk->edc == 1)
		{
			rt = SKP_Silk_SDK_Get_Encoder_Size(&nSize);
			if(rt)
			{
				goto lab_err;
			}

			g_aloc(ptk->hd, nSize);
			g_zero(ptk->hd, nSize);

			rt = SKP_Silk_SDK_InitEncoder(ptk->hd, &ptk->ted.en);
			if(rt)
			{
				goto lab_err;
			}

			ptk->ted.en.API_sampleRate        = ptk->hz==8000 ? 12000 : 24000;
			ptk->ted.en.maxInternalSampleRate = 24000;
			ptk->ted.en.packetSize            = 480;
			ptk->ted.en.bitRate               = ptk->hz + ptk->hz/10 * ptk->level;
			ptk->ted.en.packetLossPercentage  = 0;
			ptk->ted.en.complexity            = 2;
			ptk->ted.en.useInBandFEC          = 0;
			ptk->ted.en.useDTX                = 0;
		}
		else
		{
			rt = SKP_Silk_SDK_Get_Decoder_Size(&nSize);
			if(rt)
			{
				goto lab_err;
			}

			g_aloc(ptk->hd, nSize);
			g_zero(ptk->hd, nSize);

			rt = SKP_Silk_SDK_InitDecoder(ptk->hd);
			if(rt)
			{
				goto lab_err;
			}

			ptk->ted.de.API_sampleRate = ptk->hz==8000 ? 12000 : 24000;
			ptk->ted.de.frameSize = 0;
			ptk->ted.de.framesPerPacket = 1;
			ptk->ted.de.moreInternalDecoderFrames = 0;
			ptk->ted.de.inBandFECOffset = 0;
		}
	}

	{
		if(ptk->in.size < 1024)
		{
			g_aloc(ptk->in.buf, 1024);
			ptk->in.size = 1024;
		}

		if(ptk->out.size < 8192)
		{
			g_aloc(ptk->out.buf, 8192);
			ptk->out.size = 8192;
		}

		ptk->in.length = 0;
		ptk->out.length = 0;
	}

	err = 0;
	lab_err:
	if(err)
	{
		tsilk_fini(ptk);
		ptk = 0;
	}
	return ptk;
}

void tsilk_fini(void * ht)
{
	if(ht)
	{
		StTsilk * ptk = (StTsilk *)ht;
		g_free(ptk->out.buf);
		g_free(ptk->in.buf);
		g_free(ptk->hd);
		g_free(ptk);
	}
}

int tsilk_in(void * ht, const void * src, int src_len)
{
	int err = -1;
	int rt = 0;
	StTsilk * ptk = (StTsilk *)ht;
	const char * sInBuf = (const char *)src;
	int nInLen = src_len;
	int nInIndex = 0;
	char * sOutBuf = 0;
	int nOutLen = 0;

	if(!ptk)
	{
		goto lab_err;
	}

	if(!sInBuf || nInLen<1)
	{
		err = ptk->out.length;
		goto lab_err;
	}

	if(ptk->edc == 1)
	{
		nInIndex = 0;
		g_aloc(sOutBuf, 1250);

		if(ptk->in.length > 0)
		{
			if(nInLen < 960 - ptk->in.length)
			{
				memcpy(ptk->in.buf + ptk->in.length, sInBuf, nInLen);
				ptk->in.length += nInLen;
				err = ptk->out.length;
				goto lab_err;
			}

			memcpy(ptk->in.buf + ptk->in.length, sInBuf, 960 - ptk->in.length);
			nInIndex = 960 - ptk->in.length;
			ptk->in.length = 0;

			nOutLen = 1250;
			rt = SKP_Silk_SDK_Encode(ptk->hd, &ptk->ted.en, (short *)ptk->in.buf, 480, sOutBuf, (short *)&nOutLen);
			if(rt)
			{
				goto lab_err;
			}

			if(nOutLen > 0)
			{
				if(ptk->out.size - ptk->out.length < nOutLen + (int)sizeof(short))
				{
					ptk->out.size = (ptk->out.length + sizeof(short) + nOutLen) / 8192 * 8192 + 8192;
					g_aloc(ptk->out.buf, ptk->out.size);
				}

				memcpy(ptk->out.buf + ptk->out.length, &nOutLen, sizeof(short));
				ptk->out.length += sizeof(short);
				memcpy(ptk->out.buf + ptk->out.length, sOutBuf, nOutLen);
				ptk->out.length += nOutLen;
			}
		}

		while(1)
		{
			if(nInLen - nInIndex < 960)
			{
				memcpy(ptk->in.buf, sInBuf + nInIndex, nInLen - nInIndex);
				ptk->in.length = nInLen - nInIndex;
				break;
			}

			nOutLen = 1250;
			rt = SKP_Silk_SDK_Encode(ptk->hd, &ptk->ted.en, (short *)(sInBuf + nInIndex), 480, sOutBuf, (short *)&nOutLen);
			if(rt)
			{
				goto lab_err;
			}

			if(nOutLen > 0)
			{
				if(ptk->out.size - ptk->out.length < nOutLen + (int)sizeof(short))
				{
					ptk->out.size = (ptk->out.length + sizeof(short) + nOutLen) / 8192 * 8192 + 8192;
					g_aloc(ptk->out.buf, ptk->out.size);
				}

				memcpy(ptk->out.buf + ptk->out.length, &nOutLen, sizeof(short));
				ptk->out.length += sizeof(short);
				memcpy(ptk->out.buf + ptk->out.length, sOutBuf, nOutLen);
				ptk->out.length += nOutLen;
			}

			nInIndex += 960;
		}
	}
	else
	{
		int nl = 0;
		int nr = 0;

		g_aloc(sOutBuf, 19200);

		if(ptk->in.length > 0)
		{
			if(ptk->in.length < sizeof(short))
			{
				if(nInLen < (int)sizeof(short) - ptk->in.length)
				{
					memcpy(ptk->in.buf + ptk->in.length, sInBuf, nInLen);
					ptk->in.length = nInLen;
					err = ptk->out.length;
					goto lab_err;
				}

				memcpy(ptk->in.buf + ptk->in.length, sInBuf, sizeof(short) - ptk->in.length);
				nInIndex = sizeof(short) - ptk->in.length;
				ptk->in.length = sizeof(short);
			}

			memcpy(&nl, ptk->in.buf, sizeof(short));
			if(nl < 1)
			{
				goto lab_err;
			}

			if(nInLen - nInIndex < nl - ptk->in.length + (int)sizeof(short))
			{
				if(ptk->in.size < ptk->in.length + nInLen - nInIndex)
				{
					ptk->in.size = (ptk->in.length + nInLen - nInIndex) / 1024 * 1024 + 1024;
					g_aloc(ptk->in.buf, ptk->in.size);
				}

				memcpy(ptk->in.buf + ptk->in.length, sInBuf + nInIndex, nInLen - nInIndex);
				ptk->in.length += nInLen - nInIndex;
				err = ptk->out.length;
				goto lab_err;
			}
			else
			{
				if(ptk->in.size < (int)sizeof(short) + nl)
				{
					ptk->in.size = (sizeof(short) + nl) / 1024 * 1024 + 1024;
					g_aloc(ptk->in.buf, ptk->in.size);
				}

				memcpy(ptk->in.buf + ptk->in.length, sInBuf + nInIndex, nl - ptk->in.length + sizeof(short));
				nInIndex += nl - ptk->in.length + sizeof(short);
				ptk->in.length = 0;
			}

			nr = 0;
			do
			{
				rt = SKP_Silk_SDK_Decode(ptk->hd, &ptk->ted.de, 0, ptk->in.buf + sizeof(short), nl, (short *)(sOutBuf + nr), (short *)&nOutLen);
				if(rt)
				{
					goto lab_err;
				}
				nr += nOutLen * sizeof(short);
			}while(ptk->ted.de.moreInternalDecoderFrames);

			if(nr>0)
			{
				if(ptk->out.size < ptk->out.length + nr)
				{
					ptk->out.size = (ptk->out.length + nr) / 8192 * 8192 + 8192;
					g_aloc(ptk->out.buf, ptk->out.size);
				}

				memcpy(ptk->out.buf + ptk->out.length, sOutBuf, nr);
				ptk->out.length += nr;
			}
		}

		while(1)
		{
			if(nInLen - nInIndex < sizeof(short))
			{
				memcpy(ptk->in.buf, sInBuf + nInIndex, nInLen - nInIndex);
				ptk->in.length = nInLen - nInIndex;
				break;
			}

			memcpy(&nl, sInBuf + nInIndex, sizeof(short));
			nInIndex += sizeof(short);
			if(nl<1)
			{
				goto lab_err;
			}

			if(nInLen - nInIndex < nl)
			{
				if(ptk->in.size < nInLen - nInIndex + (int)sizeof(short))
				{
					ptk->in.size = (nInLen - nInIndex + sizeof(short)) / 1024 * 1024 + 1024;
					g_aloc(ptk->in.buf, ptk->in.size);
				}

				memcpy(ptk->in.buf, &nl, sizeof(short));
				memcpy(ptk->in.buf + sizeof(short), sInBuf + nInIndex, nInLen - nInIndex);
				ptk->in.length = sizeof(short) + nInLen - nInIndex;
				break;
			}

			nr = 0;
			do
			{
				rt = SKP_Silk_SDK_Decode(ptk->hd, &ptk->ted.de, 0, sInBuf + nInIndex, nl, (short *)(sOutBuf + nr), (short *)&nOutLen);
				if(rt)
				{
					goto lab_err;
				}
				nr += nOutLen * sizeof(short);
			}while(ptk->ted.de.moreInternalDecoderFrames);

			if(nr>0)
			{
				if(ptk->out.size < ptk->out.length + nr)
				{
					ptk->out.size = (ptk->out.length + nr) / 8192 * 8192 + 8192;
					g_aloc(ptk->out.buf, ptk->out.size);
				}

				memcpy(ptk->out.buf + ptk->out.length, sOutBuf, nr);
				ptk->out.length += nr;
			}

			nInIndex += nl;
		}
	}

	err = ptk->out.length;

	lab_err:
	g_free(sOutBuf);
	return err;
}

int tsilk_out(void * ht, void * dst, int dst_len)
{
	int err = -1;
	StTsilk * ptk = (StTsilk *)ht;
	int nReadLen = 0;

	if(!ptk)
	{
		goto lab_err;
	}

	if(!dst || dst_len<1)
	{
		err = ptk->out.length;
		goto lab_err;
	}

	nReadLen = ptk->out.length > dst_len ? dst_len : ptk->out.length;

	memcpy(dst, ptk->out.buf, nReadLen);
	
	if(nReadLen < ptk->out.length)
	{
		memcpy(ptk->out.buf, ptk->out.buf + nReadLen, ptk->out.length - nReadLen);
	}

	ptk->out.length -= nReadLen;

	err = nReadLen;

	lab_err:
	return err;
}

#endif
