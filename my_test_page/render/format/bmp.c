#include <config.h>
#include <pic_operation.h>
#include <picfmt_manager.h>
#include <file.h>
#include <stdlib.h>
#include <string.h>

#pragma pack(push) /* ����ǰpack����ѹջ���� */
#pragma pack(1)    /* �����ڽṹ�嶨��֮ǰʹ��,����Ϊ���ýṹ���и���Ա��1�ֽڶ��� */

typedef struct tagBITMAPFILEHEADER { /* bmfh */
	unsigned short bfType;              /*˵���ļ������ͣ���ֵ������0x4D42��Ҳ�����ַ�'BM'��*/
	unsigned long  bfSize;              /*˵����λͼ�ļ��Ĵ�С�����ֽ�Ϊ��λ*/
	unsigned short bfReserved1;         /*��������������Ϊ0*/
	unsigned short bfReserved2;         /*��������������Ϊ0*/
	unsigned long  bfOffBits;           /*˵�����ļ�ͷ��ʼ��ʵ�ʵ�ͼ������֮����ֽڵ�ƫ������
										   ��������Ƿǳ����õģ���Ϊλͼ��Ϣͷ�͵�ɫ��ĳ��Ȼ���ݲ�ͬ������仯��
										   ��������������ƫ��ֵѸ�ٵĴ��ļ��ж�ȡ��λ���ݡ�*/
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER { /* bmih */
	unsigned long  biSize;              /*˵��BITMAPINFOHEADER�ṹ����Ҫ��������*/
	unsigned long  biWidth;             /*˵��ͼ��Ŀ�ȣ�������Ϊ��λ��*/
	unsigned long  biHeight;            /*˵��ͼ��ĸ߶ȣ�������Ϊ��λ���������BMP�ļ����ǵ����λͼ��Ҳ����ʱ���߶�ֵ��һ��������*/
	unsigned short biPlanes;            /*ΪĿ���豸˵��λ��������ֵ�����Ǳ���Ϊ1��*/
	unsigned short biBitCount;          /*˵��������/���أ���ֵΪ1��4��8��16��24����32��*/
	unsigned long  biCompression;       /*˵��ͼ������ѹ�������ͣ�ͬ������ֻ����û��ѹ�������ͣ�BI_RGB��*/
	unsigned long  biSizeImage;         /*˵��ͼ��Ĵ�С�����ֽ�Ϊ��λ������BI_RGB��ʽʱ��������Ϊ0��*/
	unsigned long  biXPelsPerMeter;     /*˵��ˮƽ�ֱ��ʣ�������/�ױ�ʾ��*/
	unsigned long  biYPelsPerMeter;     /*˵����ֱ�ֱ��ʣ�������/�ױ�ʾ��*/
	unsigned long  biClrUsed;           /*˵��λͼʵ��ʹ�õĲ�ɫ���е���ɫ����������Ϊ0�Ļ�����˵��ʹ�����е�ɫ�����*/
	unsigned long  biClrImportant;      /*˵����ͼ����ʾ����ҪӰ�����ɫ��������Ŀ�������0����ʾ����Ҫ��*/
} BITMAPINFOHEADER;

#pragma pack(pop) /* �ָ���ǰ��pack���� */

static int isBMPFormat(PT_FileMap ptFileMap);
static int GetPixelDatasFrmBMP(PT_FileMap ptFileMap, PT_PixelDatas ptPixelDatas);
static int FreePixelDatasForBMP(PT_PixelDatas ptPixelDatas);

static T_PicFileParser g_tBMPParser = {
	.name           = "bmp",
	.isSupport      = isBMPFormat,
	.GetPixelDatas  = GetPixelDatasFrmBMP,
	.FreePixelDatas = FreePixelDatasForBMP,	
};

/**********************************************************************
 * �������ƣ� isBMPFormat
 * ���������� BMPģ���Ƿ�֧�ָ��ļ�,�����ļ��Ƿ�ΪBMP�ļ�
 * ��������� ptFileMap - �ں��ļ���Ϣ
 * ��������� ��
 * �� �� ֵ�� 0 - ��֧��, 1 - ֧��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
static int isBMPFormat(PT_FileMap ptFileMap)
{
    unsigned char *aFileHead = ptFileMap->pucFileMapMem;
    
	if (aFileHead[0] != 0x42 || aFileHead[1] != 0x4d)
		return 0;
	else
		return 1;
}

/**********************************************************************
 * �������ƣ� CovertOneLine
 * ���������� ��BMP�ļ���һ�е���������,ת��Ϊ������ʾ�豸��ʹ�õĸ�ʽ
 * ��������� iWidth      - ���,�����ٸ�����
 *            iSrcBpp     - BMP�ļ���һ�������ö���λ����ʾ
 *            iDstBpp     - ��ʾ�豸��һ�������ö���λ����ʾ
 *            pudSrcDatas - BMP�ļ���������ݵ�λ��
 *            pudDstDatas - ת���������ݴ洢��λ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
static int CovertOneLine(int iWidth, int iSrcBpp, int iDstBpp, unsigned char *pudSrcDatas, unsigned char *pudDstDatas)
{
	unsigned int dwRed;
	unsigned int dwGreen;
	unsigned int dwBlue;
	unsigned int dwColor;

	unsigned short *pwDstDatas16bpp = (unsigned short *)pudDstDatas;
	unsigned int   *pwDstDatas32bpp = (unsigned int *)pudDstDatas;

	int i;
	int pos = 0;

	if (iSrcBpp != 24)
	{
		return -1;
	}

	if (iDstBpp == 24)
	{
		memcpy(pudDstDatas, pudSrcDatas, iWidth*3);
	}
	else
	{
		for (i = 0; i < iWidth; i++)
		{
			dwBlue  = pudSrcDatas[pos++];
			dwGreen = pudSrcDatas[pos++];
			dwRed   = pudSrcDatas[pos++];
			if (iDstBpp == 32)
			{
				dwColor = (dwRed << 16) | (dwGreen << 8) | dwBlue;
				*pwDstDatas32bpp = dwColor;
				pwDstDatas32bpp++;
			}
			else if (iDstBpp == 16)
			{
				/* 565 */
				dwRed   = dwRed >> 3;
				dwGreen = dwGreen >> 2;
				dwBlue  = dwBlue >> 3;
				dwColor = (dwRed << 11) | (dwGreen << 5) | (dwBlue);
				*pwDstDatas16bpp = dwColor;
				pwDstDatas16bpp++;
			}
		}
	}
	return 0;
}

/**********************************************************************
 * �������ƣ� GetPixelDatasFrmBMP
 * ���������� ��BMP�ļ��е���������,ȡ����ת��Ϊ������ʾ�豸��ʹ�õĸ�ʽ
 * ��������� ptFileMap    - �ں��ļ���Ϣ
 * ��������� ptPixelDatas - �ں���������
 *            ptPixelDatas->iBpp ������Ĳ���, ��ȷ����BMP�ļ��õ�������Ҫת��Ϊ��BPP
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���          �޸�����
 * -----------------------------------------------
 * 2013/02/08        V1.0     Τ��ɽ          ����
 ***********************************************************************/
static int GetPixelDatasFrmBMP(PT_FileMap ptFileMap, PT_PixelDatas ptPixelDatas)
{
	BITMAPFILEHEADER *ptBITMAPFILEHEADER;
	BITMAPINFOHEADER *ptBITMAPINFOHEADER;

    unsigned char *aFileHead;

	int iWidth;
	int iHeight;
	int iBMPBpp;
	int y;

	unsigned char *pucSrc;
	unsigned char *pucDest;
	int iLineWidthAlign;
	int iLineWidthReal;

    aFileHead = ptFileMap->pucFileMapMem;

	ptBITMAPFILEHEADER = (BITMAPFILEHEADER *)aFileHead;
	ptBITMAPINFOHEADER = (BITMAPINFOHEADER *)(aFileHead + sizeof(BITMAPFILEHEADER));

	iWidth = ptBITMAPINFOHEADER->biWidth;
	iHeight = ptBITMAPINFOHEADER->biHeight;
	iBMPBpp = ptBITMAPINFOHEADER->biBitCount;

	if (iBMPBpp != 24)
	{
		DBG_PRINTF("iBMPBpp = %d\n", iBMPBpp);
		DBG_PRINTF("sizeof(BITMAPFILEHEADER) = %d\n", sizeof(BITMAPFILEHEADER));
		return -1;
	}

	ptPixelDatas->iWidth  = iWidth;
	ptPixelDatas->iHeight = iHeight;
	//ptPixelDatas->iBpp    = iBpp;
	ptPixelDatas->iLineBytes    = iWidth * ptPixelDatas->iBpp / 8;
    ptPixelDatas->iTotalBytes   = ptPixelDatas->iHeight * ptPixelDatas->iLineBytes;
	ptPixelDatas->aucPixelDatas = malloc(ptPixelDatas->iTotalBytes);
	if (NULL == ptPixelDatas->aucPixelDatas)
	{
		return -1;
	}

	iLineWidthReal = iWidth * iBMPBpp / 8;
	iLineWidthAlign = (iLineWidthReal + 3) & ~0x3;   /* ��4ȡ�� */
		
	pucSrc = aFileHead + ptBITMAPFILEHEADER->bfOffBits;
	pucSrc = pucSrc + (iHeight - 1) * iLineWidthAlign;

	pucDest = ptPixelDatas->aucPixelDatas;
	
	/*һ��һ��ȡ��������*/
	for (y = 0; y < iHeight; y++)
	{		
		//memcpy(pucDest, pucSrc, iLineWidthReal);
		CovertOneLine(iWidth, iBMPBpp, ptPixelDatas->iBpp, pucSrc, pucDest);
		pucSrc  -= iLineWidthAlign;
		pucDest += ptPixelDatas->iLineBytes;
	}
	return 0;	
}

/**********************************************************************
 * �������ƣ� FreePixelDatasForBMP
 * ���������� GetPixelDatasFrmBMP�ķ�����,������������ռ�ڴ��ͷŵ�
 * ��������� ptPixelDatas - �ں���������
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���          �޸�����
 * -----------------------------------------------
 * 2013/02/08        V1.0     Τ��ɽ          ����
 ***********************************************************************/
static int FreePixelDatasForBMP(PT_PixelDatas ptPixelDatas)
{
	free(ptPixelDatas->aucPixelDatas);
	return 0;
}

/**********************************************************************
 * �������ƣ� BMPParserInit
 * ���������� ע��"BMP�ļ�����ģ��"
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
int BMPParserInit(void)
{
	return RegisterPicFileParser(&g_tBMPParser);
}

