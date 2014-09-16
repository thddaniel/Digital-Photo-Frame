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
				/* 刚按下 */
				/* 记录刚开始压下的点 */
				tSampPressed = tSamp;
				bStart = 1;
			}


			if (tSamp.iPressure<= 0)
			{
				/* 松开 */
				tSampReleased = tSamp;

				/* 处理数据 */
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
						/* 翻到上一页 */
						ShowPrePage();
					}
					else if (iDeltaX < 0 - giXres/5)
					{
						/* 翻到下一页 */
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
 * 函数名称： TxtPageInit
 * 功能描述： 注册"显示txt文件"
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int TxtPageInit(void)
{
	return RegisterPageAction(&g_tTxtPageAction);
}
