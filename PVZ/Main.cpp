#include<iostream>
#include "tools.h"//EASYX����ͼƬ���������ù���
#include<graphics.h>//ͼ�ο�EASYX
#include <time.h>
using namespace std;

//�궨�����ô��ڴ�С
#define WIN_WIDTH 990
#define WIN_HIGHT 600

IMAGE imgMap1;//��ͼ1
IMAGE imgBar;//����
enum  {WAN_DOU,XIANG_RI_KUI,DA_ZUI_HUA,TU_DOU_DI_LEI,ZHI_WU_COUNT  };//ö���� �Զ�����
IMAGE imagCards[ZHI_WU_COUNT]; 
IMAGE *imgeZhiWu[ZHI_WU_COUNT][20];//���ָ��ָ��NULL��������ͼƬ

int curX, curY;//��ǰѡ�е�ֲ����ƶ��е�λ��
int curZhiWu;//��ǵڼ���ֲ��

struct zhiwu//��ֲֲ��֡���Լ�����
{
	int type;//0û��ֲ�� 
	int frameIndex;//����֡�����
}map[3][9];
struct sunshineBall
{
	int x, y;//������Ʈ��˳��
	int frameIndex;
	int destY;//Ʈ���Ŀ��λ�õ�y����
	bool used;//������ʹ�����
} balls[10];
IMAGE imgSunshineBall[29];

bool fileExist(const char*name)//�ļ��жϴ���
{
	FILE* fp = fopen(name, "r");//�ж��ļ��Ƿ���ھ��ǿ����ܷ��
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

void gameInit()//��Ϸ��ʼ��
{
//������Ϸ����
	loadimage(&imgMap1, "res/Map/map.jpg");
	loadimage(&imgBar, "res/Screen/bar.png");
	memset(imgeZhiWu, 0, sizeof(imgeZhiWu));//��ָ��������ڴ�����Ϊ��
	char name[64];//�����ļ���
	for(int i=0;i<ZHI_WU_COUNT;i++)
	{
		sprintf_s(name, sizeof(name), "res/Cards/card_%d.png", i + 1);//�����ַ���
		loadimage(&imagCards[i], name);
		for(int j=0;j<20;j++)
		{
			sprintf_s(name, sizeof(name), "res/zhiwu/%d/%d.png",i, j+ 1);//�����ַ���
			//���ж��ļ�������
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
	srand(time(NULL));//����м� �����������
}
void updateWindow()//���´���ͼƬ
{
	BeginBatchDraw();//����ͼ��
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



	//��Ⱦ �϶����̵�ֲ��
	if (curZhiWu)
	{
		IMAGE* img = imgeZhiWu[curZhiWu - 1][0];
		putimagePNG(curX - img->getwidth() / 2, curY - img->getheight() / 2, img);//�϶�ֲ��λ��
	}
	EndBatchDraw();//��������
}
void uesrClick()
{
	ExMessage msg;
	static int status = 0;//ֲ���Ƿ�ѡ��
	if (peekmessage(&msg))
	{
		if(msg.message==WM_LBUTTONDOWN)//�����
		{
			if(msg.x>338&&msg.x<338+65*ZHI_WU_COUNT&&msg.y>10&&msg.y<100)
			{
				int index = (msg.x - 338) /65;//�����ĳһ��ͼƬ
				status = 1;
				curZhiWu = index + 1;//ѡ�е�ͼƬ
				curX = msg.x;
				curY = msg.y;
			}
		}
		else if(msg.message==WM_MOUSEMOVE&&status==1)//����϶�
		{
			curX = msg.x;
			curY = msg.y;
		}
		else if (msg.message == WM_LBUTTONUP)//����ɿ�
		{


			if (curX >=260 && curY >=0&& curX<=990&&curY<=559&&status==1)
			{
				if (msg.x > 256 && msg.y > 179 && msg.y < 489)
				{
					int row = (msg.y - 179) / 102;
					int col = (msg.x - 256) / 81;
					if(map[row][col].type==0)
					{
						map[row][col].type = curZhiWu;//ֲ����ֲ���
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
	//�ж��Ƿ�����Ϣ
	
}
void createSunshine()
{
	static  int count = 0;
	static  int fre = 400;
	count++;
	if(count>=400)
	{
		//���������ȡһ������ʹ�õ�
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


	createSunshine();//��������
}


void startUI()
{
	IMAGE imgBg,imgMenu1,imgMenu2;
	loadimage(&imgBg, "res/menu.png");
	loadimage(&imgMenu1, "res/menu1.png");
	loadimage(&imgMenu2, "res/menu2.png");
	int flag = 0;//���ѡ��ؿ�״̬
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
				flag = 1;//���ѡ��
				
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
		timer += getDelay();//���ߺ������ڻ�ȡʱ����
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
		//����Ӱ���϶�
	
	}

	system("pause");
	return 0;
}