#include "stdafx.h"
#include "crypt.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define g_aloc(p,n) do{if(p){char *_dx=p;do{p=(char*)realloc(_dx,n);}while(!p);}else{do{p=(char*)malloc(n);}while(!p);}}while(0)
#define g_free(x) do{if(*(x)){free(*(x));*(x)=0;}}while(0)


int tieg_code_urlencode(const void * x_vSrc, int x_nSrcLen, void * x_vDst, int x_nDstLen)
{
	if (x_vSrc == NULL || x_nSrcLen <= 0)
	{
		return 0;
	}

	int err = 0;
	const char * sIn = (char *)x_vSrc;
	const int nInSize = x_nSrcLen;
	char * sOut = NULL;
	char buf[4] = { 0 };
	unsigned char x = 0;
	int len = 0;

	len = nInSize * 3 + 1;
	sOut = (char *)malloc(len);
	if (sOut == NULL)
	{
		goto lab_err;
	}
	memset(sOut, 0, len);

	for (int i = 0; i < nInSize; i++)
	{
		memset(buf, 0, 4);
		x = (unsigned char)sIn[i];

		if (isalnum(x))
		{
			buf[0] = (char)x;
		}
		else
		{
			sprintf(buf, "%c%02X", '%', x);
		}

		strcat(sOut, buf);
	}

	len = (int)strlen(sOut);

	if (x_vDst != NULL && len <= x_nDstLen)
	{
		memcpy(x_vDst, sOut, len);
	}

	err = len;

lab_err:
	g_free(&sOut);
	return err;
}

int tieg_code_urldecode(const void * x_vSrc, int x_nSrcLen, void * x_vDst, int x_nDstLen)
{
	if (x_vSrc == NULL || x_nSrcLen <= 0)
	{
		return 0;
	}

	int err = 0;
	const char * sIn = (char *)x_vSrc;
	const int nInSize = x_nSrcLen;
	char * sOut = NULL;
	char buf[4] = { 0 };
	unsigned char x = 0;
	unsigned int _x = 0;
	const char * p = sIn;
	int len = 0;

	len = nInSize + 1;
	sOut = (char *)malloc(len);
	if (sOut == NULL)
	{
		goto lab_err;
	}
	memset(sOut, 0, len);

	for (int i = 0; i < nInSize; i++)
	{
		if (p[i] == '%')
		{
			buf[0] = toupper(p[i + 1]);
			buf[1] = toupper(p[i + 2]);
			buf[2] = 0;
			sscanf(buf, "%02X", &_x);
			x = (unsigned char)_x;
			i += 2;
		}
		else if (p[i] == '+')
		{
			x = 0x20;
		}
		else
		{
			x = p[i];
		}

		buf[0] = (char)x;
		buf[1] = 0;
		strcat(sOut, buf);
	}

	len = (int)strlen(sOut);

	if(x_vDst!=NULL && len<=x_nDstLen)
	{
		memcpy(x_vDst, sOut, len);
	}

	err = len;

lab_err:
	g_free(&sOut);
	return err;
}


int vcyber_encrypt(const char * key, const void * src, int src_len, char * dst, int dst_len)
{
	if(!key || !key[0] || !src || src_len<1 || dst_len<0)
	{
		return 0;
	}

	int rt = 0;
	char buf[4] = {0};
	unsigned char uc = 0;

	int key_len = (int)strlen(key);
	char * sSrc = NULL;
	int nSrcSize = src_len * 3;
	char * sDst = NULL;
	int nDstSize = 0;

	g_aloc(sSrc, nSrcSize);
	memset(sSrc, 0, nSrcSize);

	/* url encode */
	{
		rt = tieg_code_urlencode(src, src_len, sSrc, nSrcSize);
		if(rt==0 || rt>nSrcSize)
		{
			nDstSize = 0;
			goto lab_err;
		}
		nSrcSize = rt;
	}

	nDstSize = nSrcSize * 2;
	g_aloc(sDst, nDstSize);
	memset(sDst, 0, nDstSize);

	for(int i=0, j=0; i<nSrcSize; i++, j++)
	{
		if(j==key_len)
		{
			j = 0;
		}

		uc = ((char *)sSrc)[i] ^ key[j];
		uc = uc >> 4 | uc << 4;
		
		memset(buf, 0, 4);
		sprintf(buf, "%02x", uc);
		memcpy(sDst + i * 2, buf, 2);
	}

	if(dst && dst_len>=nDstSize)
	{
		strncpy(dst, sDst, nDstSize);
	}

	lab_err:
	g_free(&sSrc);
	g_free(&sDst);
	return nDstSize;
}

int vcyber_decrypt(const char * key, const char * src, int src_len, void * dst, int dst_len)
{
	if(!key || !key[0] || !src || !src[0] || src_len<2 || src_len%2 || dst_len<0)
	{
		return 0;
	}

	int rt = 0;
	char buf[4] = {0};
	unsigned char uc = 0;
	unsigned int _uc = 0;
	int key_len = (int)strlen(key);
	char * sDst = NULL;
	int nDstSize = src_len / 2;
	int k = 0;

	g_aloc(sDst, nDstSize);
	memset(sDst, 0, nDstSize);
	
	for(int i=0, j=0; i<src_len; i+=2, j++)
	{
		if(j==key_len)
		{
			j = 0;
		}

		sscanf(src + i, "%02x", &_uc);
		uc = (unsigned char)_uc;

		uc = uc >> 4 | uc << 4;
		uc = uc ^ key[j];
		
		sDst[k++] = uc;		
	}

	/* url decode */
	{
		rt = tieg_code_urldecode(sDst, nDstSize, sDst, nDstSize);
		if(rt==0 || rt>nDstSize)
		{
			nDstSize = 0;
			goto lab_err;
		}
		nDstSize = rt;
	}

	if(dst && dst_len>=nDstSize)
	{
		memcpy(dst, sDst, nDstSize);
	}

	lab_err:
	g_free(&sDst);
	return nDstSize;	
}
