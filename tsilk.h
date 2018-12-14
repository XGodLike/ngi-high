#ifndef _TSILK_HEADER_FILE_
#define _TSILK_HEADER_FILE_

/* version : 2016-07-28-15 */
#define _TSILK_VERSION 2016072815

#ifdef __cplusplus
extern "C" {
#endif

/*
	���������ã�һ��tsilk���
	ht : ��Ҫ���õľ��0��ʾ�½���
	edc : 0 Ĭ�ϣ�1 ѹ����2 ��ѹ��Ĭ�ϣ�����ʱ��ʾ���䡣�½�ʱ��ʾѹ����
	hz : 0 Ĭ�ϣ�8000��16000��Ĭ�ϣ�����ʱ��ʾ���䡣�½�ʱ��ʾ16000��
	level : 0 Ĭ�ϣ�[1-10] ѹ������ѹ���ȼ���Ĭ�ϣ�����ʱ��ʾ���䡣�½�ʱ��ʾ10��
	return : tsilk���0��ʾʧ�ܡ�
*/
void * tsilk_init(void * ht, int edc, int hz, int level);

/* �ͷ�tsilk���˾����ʹ��ʱ�����ͷš� */
void tsilk_fini(void * ht);

/*
	����Ҫ���?ѹ�����ѹ�������
	return : 
		-1 ʧ�ܣ��˾���ٿ��ã����ú������ʹ�ã�����ͷŵ���
		�ɹ���
		 0 ����ݿ�ȡ��
		>0 ��ȡ��ݳ���
		ע: ��src==0��src_len==0�����ؿɶ���ݳ��ȡ�
*/
int tsilk_in(void * ht, const void * src, int src_len);

/*
	��ȡҪ���?ѹ�����ѹ�������
	return : 
		-1 ʧ�ܣ��˾���ٿ��ã����ú������ʹ�ã�����ͷŵ���
		�ɹ���
		 0 ����ݿ�ȡ
		>0 ȡ����ݳ���
		ע�� ��dst==0��dst_len==0�����ؿɶ���ݳ��ȡ�
*/
int tsilk_out(void * ht, void * dst, int dst_len);

#ifdef __cplusplus
}
#endif

#endif
