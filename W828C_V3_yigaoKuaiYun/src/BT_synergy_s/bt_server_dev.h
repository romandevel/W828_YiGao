#define		TIANHE_DATA_LEN			14
/*-----------------------------------------------------
���
��ȡ����ʱҪ���͵��ַ���:"\x02\x41\x44\x00\x05\x03"
���ӳӷ������������ݸ�ʽ:
02 41 44 2b 30 30 30 30 37 38 32 31 39 03
-----------------------------------------------------*/

#define		DAHUA_DATA_LEN			6
/*-----------------------------------------------------
��
���ӳӷ������������ݸ�ʽ:
20 38 36 32 30 20
-----------------------------------------------------*/

#define		YAOHUA_DATA_LEN			12
/*-----------------------------------------------------
ҫ��
��ȡ����ʱҪ���͵��ַ���:"\x02\x41\x44\x00\x05\x03"
���ӳӷ������������ݸ�ʽ:
02 2b 30 30 30 30 37 38 32 31 39 03
-----------------------------------------------------*/

#define		KELI_DATA_LEN			14
/*-----------------------------------------------------
����
���ӳӷ������������ݸ�ʽ:
23.45kg   ASCII��: =0023.45(kg) 0D 0A
-----------------------------------------------------*/

#define  BT_MAX_SHOW   (20)


enum 
{
	BT_FREE = 0,
	BT_INIT_OK ,
	BT_CONNECT_OK,
	
} ;