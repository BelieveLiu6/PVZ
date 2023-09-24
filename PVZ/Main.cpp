#include<iostream>
#include "tools.h"//EASYX覆盖图片函数的外置工具
#include<graphics.h>//图形库EASYX
#include <time.h>
using namespace std;

//宏定义设置窗口大小
#define WIN_WIDTH 990
#define WIN_HIGHT 600

IMAGE imgMap1;//地图1
IMAGE imgBar;//卡槽
enum  {WAN_DOU,XIANG_RI_KUI,DA_ZUI_HUA,TU_DOU_DI_LEI,ZHI_WU_COUNT  };//枚举型 自动计数
IMAGE imagCards[ZHI_WU_COUNT]; 
IMAGE *imgeZhiWu[ZHI_WU_COUNT][20];//如果指针指向NULL结束播放图片

int curX, curY;//当前选中的植物，在移动中的位置
int curZhiWu;//标记第几种植物

struct zhiwu//种植植物帧数以及类型
{
	int type;//0没有植物 
	int frameIndex;//序列帧的序号
}map[3][9];
struct sunshineBall
{
	int x, y;//阳光球飘落顺序
	int frameIndex;
	int destY;//飘落的目标位置的y坐标
	bool used;//阳光球使用情况
} balls[10];
IMAGE imgSunshineBall[29];

bool fileExist(const char*name)//文件判断存在
{
	FILE* fp = fopen(name, "r");//判断文件是否存在就是看它能否打开
	if(fp==NULL)
	{
		return false;
	}
	else
	{
		fclose(fp);
		return true;

	}
}

void gameInit()//游戏初始化
{
//加载游戏背景
	loadimage(&imgMap1, "res/Map/map.jpg");
	loadimage(&imgBar, "res/Screen/bar.png");
	memset(imgeZhiWu, 0, sizeof(imgeZhiWu));//将指针数组的内存设置为空
	char name[64];//生成文件名
	for(int i=0;i<ZHI_WU_COUNT;i++)
	{
		sprintf_s(name, sizeof(name), "res/Cards/card_%d.png", i + 1);//生成字符串
		loadimage(&imagCards[i], name);
		for(int j=0;j<20;j++)
		{
			sprintf_s(name, sizeof(name), "res/zhiwu/%d/%d.png",i, j+ 1);//生成字符串
			//先判断文件存在性
			if (fileExist(name))
			{
				imgeZhiWu[i][j] = new IMAGE;
				loadimage(imgeZhiWu[i][j], name);
			}
			else
				break;
		}
	}
	curZhiWu = 0;

	memset(balls,0, sizeof(balls));
	for(int i=0;i<29;i++)
	{
		sprintf_s(name, sizeof(name), "res/sunshine/%d.png", i + 1);
		loadimage(&imgSunshineBall[i], name);
	}
	srand(time(NULL));//随机中间 生成随机数字
}
void updateWindow()//更新窗口图片
{
	BeginBatchDraw();//缓存图像
	putimage(0, 0, &imgMap1);
	putimagePNG(250, 0, &imgBar);
	for(int i=0;i<ZHI_WU_COUNT;i++)
	{
		float x = 338 + i * 65;
		float y = 6;
		putimage(x, y, &imagCards[i]);
	}

	for(int i=0;i<3;i++)
	{
		for(int j=0;j<9;j++)
		{
			if(map[i][j].type>0)
			{
				int x = 256 + j * 81;
				int y = 179 + i * 102+15;
				int ZhiWuType = map[i][j].type - 1;
				int index = map[i][j].frameIndex;
				putimagePNG(x, y, imgeZhiWu[ZhiWuType][index]);
			}
		}
	}



	//渲染 拖动过程的植物
	if (curZhiWu)
	{
		IMAGE* img = imgeZhiWu[curZhiWu - 1][0];
		putimagePNG(curX - img->getwidth() / 2, curY - img->getheight() / 2, img);//拖动植物位置
	}
	EndBatchDraw();//结束缓冲
}
void uesrClick()
{
	ExMessage msg;
	static int status = 0;//植物是否被选中
	if (peekmessage(&msg))
	{
		if(msg.message==WM_LBUTTONDOWN)//鼠标点击
		{
			if(msg.x>338&&msg.x<338+65*ZHI_WU_COUNT&&msg.y>10&&msg.y<100)
			{
				int index = (msg.x - 338) /65;//点击的某一块图片
				status = 1;
				curZhiWu = index + 1;//选中的图片
				curX = msg.x;
				curY = msg.y;
			}
		}
		else if(msg.message==WM_MOUSEMOVE&&status==1)//鼠标拖动
		{
			curX = msg.x;
			curY = msg.y;
		}
		else if (msg.message == WM_LBUTTONUP)//鼠标松开
		{


			if (curX >=260 && curY >=0&& curX<=990&&curY<=559&&status==1)
			{
				if (msg.x > 256 && msg.y > 179 && msg.y < 489)
				{
					int row = (msg.y - 179) / 102;
					int col = (msg.x - 256) / 81;
					if(map[row][col].type==0)
					{
						map[row][col].type = curZhiWu;//植物种植标记
						map[row][col].frameIndex = 0;
					}
					else if(map[row][col].type !=0)
					{
						curZhiWu = 0;
						status = 0;;
					}
				}
			}
			else
			{
				curZhiWu = 0;
				status = 0;;
			}

		}
		else if(msg.message==WM_RBUTTONDOWN&&status==1)
		{
			curZhiWu = 0;
			status = 0;;
		}
	}
	//判断是否有消息
	
}
void createSunshine()
{
	static  int count = 0;
	static  int fre = 400;
	count++;
	if(count>=400)
	{
		//从阳光池种取一个可以使用的
		int ballMax = sizeof(balls) / sizeof(balls[0]);
		int i;
		for (i = 0; i < ballMax && balls[i].used; i++);
		if (i >= ballMax)
			return;
		balls[i].used = true;
		balls[i].frameIndex = 0;
		balls[i].x = 260 + rand() % (900 - 260);//260--899
		balls[i].y = 60;
		balls[i].destY = 200 + ((rand() % 4) * 90);
		count = 0;
	}
	
	
}
void updateGame()
{
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			if (map[i][j].type > 0)
			{
				map[i][j].frameIndex++;
				int ZhiWuType = map[i][j].type - 1;
				int index = map[i][j].frameIndex;
				if (imgeZhiWu[ZhiWuType][index] == NULL)
					map[i][j].frameIndex = 0;
					
			}
		}
	}


	createSunshine();//创建阳光
}


void startUI()
{
	IMAGE imgBg,imgMenu1,imgMenu2;
	loadimage(&imgBg, "res/menu.png");
	loadimage(&imgMenu1, "res/menu1.png");
	loadimage(&imgMenu2, "res/menu2.png");
	int flag = 0;//鼠标选择关卡状态
	while(1)
	{
		BeginBatchDraw();
		putimage(0, 0, &imgBg);
		putimagePNG(474, 75, flag ? &imgMenu2 : &imgMenu1);

		ExMessage msg;
		if(peekmessage(&msg))
		{
			if(msg.message==WM_LBUTTONDOWN&&msg.x>474&&msg.x<474+300&&msg.y>75&&msg.y<75+140)
			{
				flag = 1;//光标选中
				
			}
			else if(msg.message==WM_LBUTTONUP&&flag==1)
			{
				return;
			}
		}


		EndBatchDraw();
	}
}

int main()
{
	gameInit();
	
	initgraph(WIN_WIDTH, WIN_HIGHT);
	startUI();
	int timer = 0;
	int flag = true;
	while (1)
	{
		uesrClick();
		timer += getDelay();//工具函数用于获取时钟数
		if(timer>50)
		{
			flag = true;
			timer = 0;
		}
		if(flag)
		{
			flag = false;
			updateWindow();
			updateGame();

		}
		//不好影响拖动
	
	}

	system("pause");
	return 0;
}