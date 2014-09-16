#include <config.h>
#include <render.h>
#include <stdlib.h>
#include <file.h>
#include <string.h>
#include <unistd.h>
#include <input_manager.h>
#include <draw.h>

extern int giXres;
extern int giYres;

static void TxtPageRun(PT_PageParams ptParentPageParams)
{

	T_InputEvent tSamp;
	T_InputEvent tSampPressed;
	T_InputEvent tSampReleased;
	int bStart = 0;
	int iDeltaX,iDeltaY;


	tSampPressed.iX = 0;
	tSampReleased.iX = 0;
	tSampPressed.iY= 0;
	tSampReleased.iY= 0;
printf("giXres = %d\n",giXres);
printf("giYres = %d\n",giYres);

	ShowNextPage();

	while (1)
	{
	 if(GetInputEvent(&tSamp) == 0)
	 {
		if ((tSamp.iPressure> 0) && (bStart == 0))
			{
				/* �հ��� */
				/* ��¼�տ�ʼѹ�µĵ� */
				tSampPressed = tSamp;
				bStart = 1;
			}


			if (tSamp.iPressure<= 0)
			{
				/* �ɿ� */
				tSampReleased = tSamp;

				/* �������� */
				if (!bStart)
				{
					printf("error!\n");
				}
				else
				{
					iDeltaX = tSampReleased.iX- tSampPressed.iX;
					iDeltaY = tSampReleased.iY- tSampPressed.iY;
printf("tSampReleased.iX = %d\n",tSampReleased.iX);
printf("tSampPressed.iX = %d\n",tSampPressed.iX);
printf("iDeltaX = %d\n",iDeltaX);

printf("tSampReleased.iY = %d\n",tSampReleased.iY);
printf("tSampPressed.iY = %d\n",tSampPressed.iY);
printf("iDeltaY = %d\n",iDeltaY);
					
					bStart = 0;


					if (iDeltaX > giXres/5)
					{
						/* ������һҳ */
						ShowPrePage();
					}
					else if (iDeltaX < 0 - giXres/5)
					{
						/* ������һҳ */
						ShowNextPage();	
					}
					else if ((iDeltaY > giYres/2)||(iDeltaY < 0 - giYres/2))
					{
						if(0 == ExitPage())
						{
						return;
						}
					}
				}
			}
	 }
	}

}

static T_PageAction g_tTxtPageAction = {
	.name          = "txt",
	.Run           = TxtPageRun,
};

/**********************************************************************
 * �������ƣ� TxtPageInit
 * ���������� ע��"��ʾtxt�ļ�"
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
int TxtPageInit(void)
{
	return RegisterPageAction(&g_tTxtPageAction);
}
