#include <iostream>
#include <conio.h>
#include <string>
#include <map>
#include <cmath>
#include <windows.h>
#include <time.h> 
#define KEY_DOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1:0)
using namespace std;
int fallspeed;
int jumpspeed = -3;
int gravity = 1;
int y = 400, x = 500;
int restartposy, restartposx, face, health = 1000, lasthealth = 1000, breath = 100, hungry = 1000, dienum;
bool attack, defense, hurt, mode;
struct TNT
{
	int y;
	int x;
	int time;
	bool issave;
};
struct BLOCK
{
	int color;
	string ch;
	string type;
};
struct MOB
{
	int fallspeed;
	int health;
	bool hurt;
	int y;
	int x;
	int attack;
	string shap;
	bool isenemy;
	int color;
	string name;
};
struct ARROW
{
	string shap;
	double y;
	double x;
	double fallspeed;
	double plusx;
};
TNT tnt[20];
string die;
ARROW arrow[100];
MOB mobs[50] = {
	{0,1000,0,0,0,100,"危",true,7,"危"},
	{0,10,0,0,0,10,"  ",true,7,"  "},
	{0,1000,0,0,0,100,"MM",false,7,"MM"},
	{0,100000,0,90,70,-100,"AC",true,7,"Accept"},
	{0,10000,0,90,70,500,"BO",true,7,"BOSS"},
	{0,100000,0,90,70,-1000,"AK",true,7,"AK"},
};
MOB mob[100] = {
	{0,1000,0,92,4,100,"WA",true,7,"Wrong Anwser"},
	{0,1000,0,92,4,100,"TL",true,7,"Time Limit Error"},
	{0,2000,0,92,4,300,"CE",true,7,"Compile Error"},
	{0,1000,0,45,9,100,"WA",true,7,"Wrong Anwser"},
	{0,100000,0,90,70,-100,"AC",true,7,"Accept"},
	{0,100000,0,90,70,-1000,"AK",true,7,"AK"},
	{0,10000,0,90,70,500,"UK",true,7,"Unknown Error"},
	{0,1000,0,92,3,0,"MM",false,7,"MM"},
	{0,1000,0,92,3,0,"MM",false,7,"MM"},
	{0,1000,0,90,15,0,"MM",false,7,"MM"},
	{0,1000,0,90,80,0,"MM",false,7,"MM"},
};
BLOCK block[32] = {
	{0,"  ","air"},//空气
	{6,"██","block"},//土块
	{8,"██","block"},//石头
	{2,"██","block"},//草方块
	{15,"██","block"},//雪块 
	{4,"██","block"},//岩浆块
	{14,"▓▓","fallblock"},//沙块 
	{8,"II","fallblock"},//铁砧
	{9,"██","water"},//水
	{9,"▇▇","water"},//水
	{9,"▆▆","water"},//水
	{9,"▅▅","water"},//水
	{9,"▄▄","water"},//水
	{9,"▃▃","water"},//水
	{9,"▂▂","water"},//水
	{9,"▁▁","water"},//水
	{12,"██","lava"},//岩浆
	{12,"▇▇","lava"},//岩浆
	{12,"▆▆","lava"},//岩浆
	{12,"▅▅","lava"},//岩浆
	{12,"▄▄","lava"},//岩浆
	{12,"▃▃","lava"},//岩浆
	{12,"▂▂","lava"},//岩浆
	{12,"▁▁","lava"},//岩浆  
	{12,"危","background"},//危
	{6,"██","background"},//木头
	{10,"▓▓","background"},//树叶 
	{15,"▓▓","background"},//带雪树叶
	{15,"▅▅","bomb"},//TNT爆炸 
	{12,"Ⅲ","TNT"},//TNT
	{7,"Ⅲ","TNT"},//TNT2
	{6,"∷","ladder"},//梯子
};
int board[1005][1005];
int setboard[1005][1005];
int bag[100];
int clear_buffer()
{
	while(kbhit())
	{
		if(getch() != EOF); 
		for(int i = 1; i <= 256; i++)
		{
			if(GetAsyncKeyState(i));
		}
	}
	return 0;
}
void color(int a)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),a);
/*	1	深蓝色
	2	深绿色
	3	深青色 
	4	深红色
	5	深粉色
	i	黄色
	7	深白色
	8	灰色
	9	浅蓝色
	10	浅绿色 
	11	浅青色 
	12	浅红色 
	13	浅粉色 
	14	浅黄色 
	15	浅白色 
	背景
	1~15		黑色 
	16~31		深蓝色 
	32~47		深绿色
	48~63		深青色
	64~79		深红色
	'S'~95		深粉色
	96~111		深黄色
	112~127 	深白色
	128~143 	灰色
	144~159 	浅蓝色
	160~1'A' 	浅绿色
	176~191 	浅青色
	192~207 	浅红色
	208~223 	浅粉色
	224~239 	浅黄色
	240~255 	浅白色
*/
}
int init()//听说有人要我增加地图生成的注释，所以我就写了。 
{
	for(int j = 0; j < 100; j++)
	{
		bag[j] = 0;//这个...初始化背包 
	}
	for(int i = 0; i < 1000; i++)
	{
		for(int j = 0; j < 1000; j++)
		{
			board[i][j] = 0;//初始化地图 （我们的y是倒着来的） 
		}
	} 
	double lasty = rand() % 101 + 400;//lasty代表上一个我们选择的点的高度。 
	for(int i = 5; i < 1000; i += 5)//i每次加5，每隔5个点连一条线 
	{
		double y = rand() % 21 - 10 + lasty;//y代表这个点我们选择的高度，为了不出现太陡峭的山，我们只允许这个高度在刚才的点的高低10格内。 
		y = min(450.0, y);//这是最小高度，防止整个地图都在水里。 
		double high = lasty;//这个high是用来统计当前高度的，用double可以更加精确。
		int dirt = rand() % 5 + 2;//dirt代表这一列上泥土高度。 
		for(int j = i - 5; j < i; j++)
		{
			high += (y - lasty) / 5;//high每次增加差距的1/5。 
			for(int k = 999; k >= (int)high; k--)
			{
				if(k == (int)high)//如果是最高点 
				{
					setboard[k][j] = 3;//就用草地 
					if(high <= 350)//如果high比较高 
					{
						setboard[k][j] = 4;//就用雪地 
					}
				}
				else if(k - dirt <= (int)high)//泥土 
				{
					setboard[k][j] = 1;
				}
				else 
				{
					setboard[k][j] = 2;//石头 
				}
			}
		}
		lasty = y;//赋值 
	}
	//再来一边，填满最后几格 
	int dirt = rand() % 5 + 2;
	double high = lasty;
	for(int j = 995; j < 999; j++)
	{
		for(int k = 999; k >= (int)high; k--)
		{
			if(k == (int)high)
			{
				setboard[k][j] = 3;
				if(high <= 350)
				{
					setboard[k][j] = 4;
				}
			}
			else if(k - dirt <= (int)high)
			{
				setboard[k][j] = 1;
			}
			else 
			{
				setboard[k][j] = 2;
			}
		}
	}
	//填满水，这里默认把海平面高度设为410。 
	for(int i = 0; i < 1000; i++)
	{
		for(int j = 600; j >= 410; j--)
		{
			if(setboard[j][i] == 0)
			{
				setboard[j][i] = 8;
			}
		}
	}
	//沙子 
	for(int i = 0; i < 1000; i++)
	{
		bool a = 0;
		for(int j = 999; j >= 0; j--)
		{
			if(a && setboard[j][i] != 0  && setboard[j][i] != 8)
			{
				setboard[j][i] = 6;
				continue;
			}
			if(setboard[j][i] == 8)
			{
				continue;
			}
			if(setboard[j][i + 3] == 8)
			{
				a = true;
				setboard[j][i] = 6;
			}
			if(setboard[j][i + 2] == 8)
			{
				a = true;
				setboard[j][i] = 6;
			}
			if(setboard[j][i + 1] == 8)
			{
				a = true;
				setboard[j][i] = 6;
			}
			if(setboard[j][i - 1] == 8)
			{
				a = true;
				setboard[j][i] = 6;
			}
			if(setboard[j][i - 2] == 8)
			{
				a = true;
				setboard[j][i] = 6;
			}
			if(setboard[j][i - 3] == 8)
			{
				a = true;
				setboard[j][i] = 6;
			}
			if(setboard[j - 2][i] == 8)
			{
				a = true;
				setboard[j][i] = 6;
			}
			if(setboard[j - 1][i] == 8)
			{
				a = true;
				setboard[j][i] = 6;
			}
			if(setboard[j - 3][i] == 8)
			{
				a = true;
				setboard[j][i] = 6;
			}
			if(setboard[j - 4][i] == 8)
			{
				a = true;
				setboard[j][i] = 6;
			}
		}
	}
	//树 
	for(int i = 0; i < 1000; i++)
	{
		for(int j = 0; j < 1000; j++)
		{
			if(setboard[j][i] == 0 && block[setboard[j + 1][i]].type == "block")
			{
				if(rand() % 10 == 1)
				{
					setboard[j][i] = 25;
					setboard[j - 1][i] = 25;
					setboard[j - 2][i] = 25;
					setboard[j - 3][i] = 25;
					setboard[j - 3][i + 1] = 26;
					setboard[j - 3][i - 1] = 26;
					setboard[j - 4][i + 2] = 26; 
					setboard[j - 4][i + 1] = 26;
					setboard[j - 4][i] = 26;
					setboard[j - 4][i - 1] = 26;
					setboard[j - 4][i - 2] = 26;
					setboard[j - 5][i + 2] = 26;
					setboard[j - 5][i + 1] = 26;
					setboard[j - 5][i] = 26;
					setboard[j - 5][i - 1] = 26;
					setboard[j - 5][i - 2] = 26;
					setboard[j - 6][i + 1] = 26;
					setboard[j - 6][i] = 26;
					setboard[j - 6][i - 1] = 26;
					if(j <= 350)
					{
						setboard[j - 3][i + 1] = 27;
						setboard[j - 3][i - 1] = 27;
						setboard[j - 4][i + 2] = 27; 
						setboard[j - 4][i + 1] = 27;
						setboard[j - 4][i] = 27;
						setboard[j - 4][i - 1] = 27;
						setboard[j - 4][i - 2] = 27;
						setboard[j - 5][i + 2] = 27;
						setboard[j - 5][i + 1] = 27;
						setboard[j - 5][i] = 27;
						setboard[j - 5][i - 1] = 27;
						setboard[j - 5][i - 2] = 27;
						setboard[j - 6][i + 1] = 27;
						setboard[j - 6][i] = 27;
						setboard[j - 6][i - 1] = 27;
					}
				}
			}
		}
	} 
	//地洞 
	for(int j = 999; j >= 700; j--)
	{
		if(rand() % 20 == 1)
		{
			setboard[j - 3][2] = 0;
			setboard[j - 2][2] = 0; setboard[j - 2][3] = 0;
			setboard[j - 1][2] = 0; setboard[j - 1][3] = 0; setboard[j - 1][4] = 0;
			setboard[j][2] = 0; setboard[j][3] = 0; setboard[j][4] = 0; setboard[j][5] = 0;
			setboard[j + 1][2] = 0; setboard[j + 1][3] = 0; setboard[j + 1][4] = 0;
			setboard[j + 2][2] = 0; setboard[j + 2][3] = 0;
			setboard[j + 3][2] = 0;
		}
	}
	for(int i = 3; i < 997; i++)
	{
		for(int j = 996; j >= 500; j--)
		{
			if(block[setboard[j + 3][i]].type == "block" && block[setboard[j - 3][i]].type == "block" && setboard[j][i] == 0 && setboard[j + 1][i] == 0 && setboard[j + 2][i] == 0 && setboard[j - 1][i] == 0  && setboard[j - 2][i] == 0)
			{
				j += rand() % 5 - 2;
				j = max(3, j);
				j = min(996, j);
				if(rand() % 500 <= 499)
				{
					setboard[j - 3][i] = 0;
					setboard[j - 2][i - 1] = 0; setboard[j - 2][i] = 0; setboard[j - 2][i + 1] = 0;
					setboard[j - 1][i - 2] = 0; setboard[j - 1][i - 1] = 0; setboard[j - 1][i] = 0; setboard[j - 1][i + 1] = 0; setboard[j - 1][i + 2] = 0;
					setboard[j][i - 3] = 0; setboard[j][i - 2] = 0; setboard[j][i - 1] = 0; setboard[j][i] = 0; setboard[j][i + 1] = 0; setboard[j][i + 2] = 0; setboard[j][i + 3] = 0;
					setboard[j + 1][i - 2] = 0; setboard[j + 1][i - 1] = 0; setboard[j + 1][i] = 0; setboard[j + 1][i + 1] = 0; setboard[j + 1][i + 2] = 0;
					setboard[j + 2][i - 1] = 0; setboard[j + 2][i] = 0; setboard[j + 2][i + 1] = 0;
					setboard[j + 3][i] = 0;
					if(rand() % 20 == 1)
					{
						for(int k = 0; k < 100; k++)
						{
							if(mob[k].shap == "")
							{
								mob[k] = {0,1000,0,0,0,100,"SS",true,7,"僵尸"};
								mob[k].x = i;
								mob[k].y = j;
								break;
							}
						}
					}
				}
			}
			else if(block[setboard[j + 3][i]].type == "block" && block[setboard[j + 2][i]].type == "block" && block[setboard[j + 1][i]].type == "block" && block[setboard[j][i]].type == "block" && block[setboard[j - 1][i]].type == "block" && block[setboard[j - 2][i]].type == "block" && block[setboard[j - 3][i]].type == "block")
			{
				if(rand() % 500 == 1)
				{
					setboard[j - 3][i] = 0;
					setboard[j - 2][i - 1] = 0; setboard[j - 2][i] = 0; setboard[j - 2][i + 1] = 0;
					setboard[j - 1][i - 2] = 0; setboard[j - 1][i - 1] = 0; setboard[j - 1][i] = 0; setboard[j - 1][i + 1] = 0; setboard[j - 1][i + 2] = 0;
					setboard[j][i - 3] = 0; setboard[j][i - 2] = 0; setboard[j][i - 1] = 0; setboard[j][i] = 0; setboard[j][i + 1] = 0; setboard[j][i + 2] = 0; setboard[j][i + 3] = 0;
					setboard[j + 1][i - 2] = 0; setboard[j + 1][i - 1] = 0; setboard[j + 1][i] = 0; setboard[j + 1][i + 1] = 0; setboard[j + 1][i + 2] = 0;
					setboard[j + 2][i - 1] = 0; setboard[j + 2][i] = 0; setboard[j + 2][i + 1] = 0;
					setboard[j + 3][i] = 0;
				}
			}
		}
	}
	//出生点 
	bool a = false;
	for(int i = 400; i < 1000; i++)
	{
		for(int j = 0; j < 1000; j++)
		{
			if(setboard[j][i] == 0 && block[setboard[j + 1][i]].type == "block" && (setboard[j + 1][i] == 3 || setboard[j + 1][i] == 4))
			{
				restartposy = j;
				restartposx = i;
				y = j;
				x = i;
				a = true;
				break;
			}
		}
		if(a)
		{
			break;
		}
	}
	return 0;
}
int Arrowmove()
{
	for(int i = 0; i < 100; i++)
	{
		if(arrow[i].shap == "")
		{
			continue;
		}
		arrow[i].x += (int)arrow[i].plusx;
		arrow[i].plusx -= 0.1;
		for(int j = (int)arrow[j].y + 1; j <= (int)arrow[j].y + (int)arrow[j].fallspeed; j++)
		{
			if(block[board[j][(int)arrow[j].x]].type == "block" || block[board[j][(int)arrow[j].x]].type == "fallblock")
			{
				arrow[i].fallspeed = 0;
				return 0;
			}
		}
		arrow[i].y += arrow[i].fallspeed;
		arrow[i].fallspeed += 0.1;
		if(arrow[i].y > 999)
		{
			arrow[i].shap = "";
		}
		if(block[board[(int)arrow[i].y + 1][(int)arrow[i].x]].type == "block" || block[board[(int)arrow[i].y + 1][(int)arrow[i].x]].type == "fallblock")
		{
			arrow[i].shap = "";
		}
	}
	return 0;
}
int mobmove()
{
	for(int j = 0; j < 100; j++)
	{
		if(mob[j].shap == "")
		{
			continue;
		}
		mob[j].hurt = false;
		mob[j].color = 7;
		if(mob[j].health <= 0 || mob[j].y > 999)
		{
			mob[j].shap = "";
			mob[j].color = 7;
			continue;
		}
		if(block[board[mob[j].y][mob[j].x]].type == "lava")
		{
			mob[j].health -= 200;
			mob[j].hurt = true;
		}
		else if((board[mob[j].y + 1][mob[j].x] == 5) || ((mob[j].y == y || mob[j].y == y - 1) && (mob[j].x == x + 1 || mob[j].x == x || mob[j].x == x - 1) && KEY_DOWN(' ')))
		{
			mob[j].health -= 100;
			mob[j].hurt = true;
		}
		if(mob[j].y == y && mob[j].x == x && mob[j].isenemy)
		{
			health -= mob[j].attack;
			hurt = true;
			die = "被 " + mob[j].name + " 杀死了";
		}
		for(int i = mob[j].y - 1; i >= mob[j].y + mob[j].fallspeed; i--)
		{
			if(block[board[i - 1][mob[j].x]].type == "block")
			{
				mob[j].fallspeed = 0 - mob[j].y + i + 1;
				return 0;
			}
		}
		for(int i = mob[j].y + 1; i <= mob[j].y + mob[j].fallspeed; i++)
		{
			if(block[board[i][mob[j].x]].type == "block" || block[board[i][mob[j].x]].type == "fallblock")
			{
				if(mob[j].fallspeed >= 5 && block[board[i - 1][mob[j].x]].type == "water")
				{
					mob[j].health -= 50 * (mob[j].fallspeed - 4);
					mob[j].hurt = true;
				}
				mob[j].fallspeed = 0;
				return 0;
			}
		}
		mob[j].y += mob[j].fallspeed;
		mob[j].fallspeed += gravity;
		if(mob[j].isenemy)
		{
			if(mob[j].y > y && (block[board[mob[j].y + 1][mob[j].x]].type == "block" || block[board[mob[j].y + 1][mob[j].x]].type == "fallblock"))
			{
				if(mob[j].y > y + 50)
				{
					mob[j].fallspeed = -7;
				}
				mob[j].fallspeed = -3;
			}
			if(mob[j].x < x)
			{
				if(block[board[mob[j].y][mob[j].x + 1]].type != "block" || block[board[mob[j].y][mob[j].x + 1]].type != "fallblock")
				{
					mob[j].x++;
				}
				else if(block[board[mob[j].y + 1][mob[j].x]].type == "block" || block[board[mob[j].y + 1][mob[j].x]].type != "fallblock")
				{
					mob[j].fallspeed = -3;
				}
			}
			else if(mob[j].x > x)
			{
				if(block[board[mob[j].y][mob[j].x - 1]].type != "block" || block[board[mob[j].y][mob[j].x - 1]].type != "fallblock")
				{
					mob[j].x--;
				}
				else if(block[board[mob[j].y + 1][mob[j].x]].type == "block" || block[board[mob[j].y + 1][mob[j].x]].type != "fallblock")
				{
					mob[j].fallspeed = -3;
				}
			}
		}
		else
		{
			if(rand() % 3 == 0)
			{
				if(block[board[mob[j].y][mob[j].x + 1]].type != "block" || block[board[mob[j].y][mob[j].x + 1]].type != "fallblock")
				{
					mob[j].x++;
				}
				else if(block[board[mob[j].y + 1][mob[j].x]].type == "block" || block[board[mob[j].y + 1][mob[j].x]].type != "fallblock")
				{
					mob[j].fallspeed = -3;
				}
			}
			else if(rand() % 3 == 1)
			{
				if(block[board[mob[j].y][mob[j].x - 1]].type != "block" || block[board[mob[j].y][mob[j].x - 1]].type != "fallblock")
				{
					mob[j].x--;
				}
				else if(block[board[mob[j].y + 1][mob[j].x]].type == "block" || block[board[mob[j].y + 1][mob[j].x]].type != "fallblock")
				{
					mob[j].fallspeed = -3;
				}
			}
		}
		if(mob[j].hurt)
		{
			mob[j].color = 12;
		}
	}
	return 0;
}
int print()
{
	if(!mode)
	{
		color(8);
		cout << "生命值:";
		color(12);
		cout << health << endl; 
		for(int i = 0; i < health; i += 100)
		{
			cout << "◆";
		}
		color(7);
		for(int i = health; i < lasthealth; i += 100)
		{
			cout << "◇";
		}
		cout << endl;
		color(8);
		cout << "氧气:";
		color(11);
		cout << breath << endl;
		for(int i = 0; i < breath; i += 10)
		{
			cout << "●";
		}
		for(int i = breath; i < 91; i += 10)
		{
			cout << "○";
		}
		cout << endl;
		color(8);
		cout << "饱食度:";
		color(14);
		cout << hungry << endl;
		for(int i = 0; i < hungry; i += 100)
		{
			cout << "§";
		}
		cout <<endl <<"按c以加满饱食度" <<endl;
	} 
	color(8);
	cout << "死亡次数:";
	color(4);
	cout << dienum << endl;
	for(int i = y - 6; i <= y + 6; i++)
	{
		for(int j = x - 6; j <= x + 6; j++)
		{
			bool ismob = false;
			for(int s = 0; s < 100; s++)
			{
				if(mob[s].shap == "")
				{
					continue;
				}
				else if(mob[s].x == j && mob[s].y == i)
				{
					color(mob[s].color);
					cout << mob[s].shap;
					ismob = true;
					break;
				}
			}
			if(ismob)
			{
				continue;
			}
			for(int s = 0; s < 100; s++)
			{
				if(arrow[s].shap == "")
				{
					continue;
				}
				else if((int)arrow[s].x == j && (int)arrow[s].y == i)
				{
					color(7);
					cout << arrow[s].shap;
					ismob = true;
					break;
				}
			}
			if(ismob)
			{
				continue;
			}
			else if(i == y && j == x)
			{
				if (KEY_DOWN('S'))
				{
					color(14);
					if(hurt)
					{
						color(12);
					}
					cout << "()";
				}
				else
				{
					color(9);
					if(hurt)
					{
						color(12);
					}
					cout << "∏";
				}
			}
			else if(i == y - 1 && j == x)
			{
				if (!KEY_DOWN('S'))
				{
					color(14);
					if(hurt)
					{
						color(12);
					}
					cout << "()";
				}
				else
				{
					color(block[board[i][j]].color);
					cout << block[board[i][j]].ch;
				}
			}
			else
			{
				if(i < 0 || i >= 1000 || j < 0 || j >= 1000)
				{
					cout << "  ";
					continue;
				}
				color(block[board[i][j]].color);
				cout << block[board[i][j]].ch;
			}
		}
		cout << endl;
	}
	color(7);
	cout << "Y坐标:";
	color(6);
	cout << 1000 - y << endl;
	color(7);
	cout << "X坐标:";
	color(6);
	cout << x << endl;
	return 0;
}
int move()
{
	if(board[y][x] == 31)
	{
		y += fallspeed;
		return 0;
	}
	if(block[board[y][x]].type == "water")
	{
		if(fallspeed > 1)
		{
			fallspeed = 1;
		}
	}
	else
	{
		for(int i = y - 1; i >= y + fallspeed; i--)
		{
			if(block[board[i - 1][x]].type == "block" || block[board[i - 1][x]].type == "fallblock")
			{
				fallspeed = 0 - y + i + 1;
				return 0;
			}
		}
	}
	for(int i = y + 1; i <= y + fallspeed; i++)
	{
		if(block[board[i][x]].type == "block" || block[board[i][x]].type == "fallblock")
		{
			if(fallspeed >= 5 && block[board[i - 1][x]].type != "water")
			{
				health -= 50 * (fallspeed - 4);
				if(fallspeed >= 7)
				{
					die = "落地过猛";
				}
				else
				{
					die = "从高处摔了下来";
				}
				hurt = true;
			}
			fallspeed = 0;
			return 0;
		}
	}
	y += fallspeed;
	fallspeed += gravity;
	return 0;
}
int bomb()
{
	for(int i = 0; i < 20; i++)
	{
		if(tnt[i].time == 0)
		{
			int atk = 0;
			if(abs(x - tnt[i].x) + abs(y - tnt[i].y) == 4)
			{
				atk = 50;
				fallspeed -= 1;
				x += x - tnt[i].x;
			}
			if(abs(x - tnt[i].x) + abs(y - tnt[i].y) == 3)
			{
				atk = 100;
				fallspeed -= 2;
				x += x - tnt[i].x;
			}
			if(abs(x - tnt[i].x) + abs(y - tnt[i].y) == 2)
			{
				atk = 300;
				fallspeed -= 4;
				x += 2 * (x - tnt[i].x);
			}
			if(abs(x - tnt[i].x) + abs(y - tnt[i].y) == 1)
			{
				atk = 500;
				fallspeed -= 7;
				x += 5 * (x - tnt[i].x);
			}
			if(abs(x - tnt[i].x) + abs(y - tnt[i].y) == 0)
			{
				atk = 800;
				fallspeed -= 10;
			}
			tnt[i].time--;
			if(!tnt[i].issave)
			{
				health -= atk;
			}
			die = "被TNT炸死了";
			if(atk > 0 && !tnt[i].issave)
			{
				hurt = true;
			}
			if(!tnt[i].issave)
			{
				for(int yy = tnt[i].y - 3; yy <= tnt[i].y + 3; yy++)
				{
					for(int xx = tnt[i].x - 3; xx <= tnt[i].x + 3; xx++)
					{
						if((int)(sqrt(abs(yy - tnt[i].y) * abs(yy - tnt[i].y) + abs(xx - tnt[i].x) * abs(xx - tnt[i].x)) + 0.9999) == 3)
						{
							setboard[yy][xx] = 28;
						}
						if((int)(sqrt(abs(yy - tnt[i].y) * abs(yy - tnt[i].y) + abs(xx - tnt[i].x) * abs(xx - tnt[i].x)) + 0.9999) < 3)
						{
							setboard[yy][xx] = 0;
						}
					}
				}
			}
		}
		else if(tnt[i].time == -1)
		{
			setboard[tnt[i].y][tnt[i].x] = 0;
			if(!tnt[i].issave)
			{
				for(int yy = tnt[i].y - 3; yy <= tnt[i].y + 3; yy++)
				{
					for(int xx = tnt[i].x - 3; xx <= tnt[i].x + 3; xx++)
					{
						if((int)(sqrt(abs(yy - tnt[i].y) * abs(yy - tnt[i].y) + abs(xx - tnt[i].x) * abs(xx - tnt[i].x)) + 0.9999) <= 3)
						{
							setboard[yy][xx] = 0;
						}
					}
				}
			}
			tnt[i].time--;
		}
		else if(tnt[i].time > 0)
		{
			tnt[i].time--;
			if(tnt[i].time % 2 == 0)
			{
				setboard[tnt[i].y][tnt[i].x] = 29;
			}
			else
			{
				setboard[tnt[i].y][tnt[i].x] = 30;
			}
		}
	}
	return 0;
}
int check()
{
	for(int i = 0; i < 1000; i++)
	{
		for(int j = 0; j < 1000; j++)
		{
			if(block[board[i][j]].type == "water" && board[i][j] != 15)
			{
				if(board[i + 1][j] == 0)
				{
					setboard[i + 1][j] = board[i][j];
					setboard[i + 1][j] = 8;
				}
				else if(block[board[i + 1][j]].type == "lava")
				{
					setboard[i + 1][j] = 2;
				}
				else if(block[board[i + 1][j]].type == "block" || block[board[i + 1][j]].type == "fallblock")
				{
					if(board[i][j + 1] == 0)
					{
						setboard[i][j + 1] = board[i][j] + 1;
					}
					else if(block[board[i][j + 1]].type == "lava")
					{
						setboard[i][j + 1] = 2;
					}
					if(board[i][j - 1] == 0)
					{
						setboard[i][j - 1] = board[i][j] + 1;
					}
					else if(block[board[i][j - 1]].type == "lava")
					{
						setboard[i][j - 1] = 2;
					}
				}
			}
			if(block[board[i][j]].type == "lava" && board[i][j] != 23)
			{
				if(board[i + 1][j] == 0)
				{
					setboard[i + 1][j] = board[i][j];
					setboard[i + 1][j] = 16;
				}
				else if(block[board[i + 1][j]].type == "water")
				{
					setboard[i + 1][j] = 2;
				}
				else if(block[board[i + 1][j]].type == "block" || block[board[i + 1][j]].type == "fallblock")
				{
					if(board[i][j + 1] == 0)
					{
						setboard[i][j + 1] = board[i][j] + 1;
					}
					else if(block[board[i][j + 1]].type == "water")
					{
						setboard[i][j + 1] = 2;
					}
					if(board[i][j - 1] == 0)
					{
						setboard[i][j - 1] = board[i][j] + 1;
					}
					else if(block[board[i][j - 1]].type == "water")
					{
						setboard[i][j - 1] = 2;
					}
				}
			}
			if(block[board[i][j]].type == "fallblock")
			{
				if(board[i + 2][j] == 0 && board[i + 1][j] == 0 && i + 2 < 100)
				{
					setboard[i][j] = 0;
					setboard[i + 2][j] = board[i][j];
					if(board[i][j] == 7 && j == x && i + 2 == y)
					{
						health -= 600;
						hurt = true;
						die = "被压扁了";
					}
				}
				else if(board[i + 1][j] == 0 && i + 1 < 100)
				{
					setboard[i][j] = 0;
					setboard[i + 1][j] = board[i][j];
					if(board[i][j] == 7 && j == x && i + 1 == y)
					{
						health -= 600;
						hurt = true;
						die = "被压扁了";
					}
					else if(board[i][j] == 7 && j == x && i + 2 == y)
					{
						health -= 600;
						hurt = true;
						die = "被压扁了";
					}
				}
			}
		}
	}
	for(int i = 0; i < 1000; i++)
	{
		for(int j = 0; j < 1000; j++)
		{
			board[i][j] = setboard[i][j];
		}
	}
}
int main()
{
	srand((int)time(0));
	for(int i = 0; i < 20; i++)
	{
		tnt[i].time = -2;
	}
	init();
	while(1)
	{
		system ("cls");
		if(!mode)
		{
			move();
		} 
		if(KEY_DOWN('F'))
		{
			setboard[y + 1][x] = 2;
		}
		check();
		bomb();
		Arrowmove();
		mobmove();
		if(mode)
		{
			hurt = false;
		} 
		print();
		Sleep(30);
		hungry--;
		hungry = max(hungry, 0);
		if(hungry == 0)
		{
			die = "饿死了";
			hurt = true;
			health -= 10;
		}
		if(mode)
		{
			health = 1000;
		}
		if(health <= 0)
		{
			Sleep(500); 
			system("cls");
			color(12);
			cout << "           You Died !           " << endl;
			color(7);
			cout << "          Alex " << die << endl;
			dienum++;
			Sleep(2000);
			x = restartposx;
			y = restartposy;
			health = 1000;
			hungry = 1000;
			breath = 100;
			fallspeed = 0;
		}
		health += (hungry + 201) / 300; 
		if(health > 1000)
		{
			health = 1000; 
		} 
		if(attack)
		{
			attack = 0;
		}
		if(defense)
		{
			defense = 0;
		}
		if(hurt)
		{
			hurt = false;
			lasthealth = health;
		}
		if(block[board[y][x]].type == "water")
		{
			fallspeed = 1;
			if(KEY_DOWN('W'))
			{
				fallspeed = -1;
			}
		}
		if(block[board[y - 1][x]].type == "water")
		{
			die = "被水淹死了"; 
			breath--; 
		}
		else
		{
			breath++; 
			if(breath > 100)
			{
				breath = 100; 
			} 
		} 
		if(board[y][x] == 31)
		{
			fallspeed = 0;
			if(KEY_DOWN('W'))
			{
				fallspeed = -1;
			}
			if(KEY_DOWN('S'))
			{
				fallspeed = 1;
			}
		}
		if(block[board[y][x]].type == "lava")
		{
			fallspeed = 1;
			if(KEY_DOWN('W'))
			{
				fallspeed = -1;
			}
			die = "试图在岩浆里游泳"; 
			hurt = true;
			health -= 100; 
		}
		if (KEY_DOWN('W') && !KEY_DOWN('S'))
		{
			if(mode)
			{
				y--;
			}
			else
			{
				if(block[board[y][x]].type != "water" && board[y][x] != 31)
				{
					if(block[board[y + 1][x]].type == "block" || block[board[y + 1][x]].type == "fallblock")
					{
						fallspeed = jumpspeed;
					}
				}
			}
		}
		if (KEY_DOWN('A'))
		{
			if((block[board[y][x - 1]].type != "block" && block[board[y][x - 1]].type != "fallblock") || mode)
			{
				if(KEY_DOWN('S') || (block[board[y - 1][x - 1]].type != "block" && block[board[y - 1][x - 1]].type != "fallblock") || mode)
				{
					x -= 1;
				}
			}
			face = -1;
		}
		if (KEY_DOWN('D'))
		{
			if((block[board[y][x + 1]].type != "block" && block[board[y][x + 1]].type != "fallblock") || mode)
			{
				if(KEY_DOWN('S') || (block[board[y - 1][x + 1]].type != "block"  && block[board[y - 1][x + 1]].type != "fallblock") || mode)
				{
					x += 1;
				}
			}
			face = 1;
		}
		if(KEY_DOWN('S'))
		{
			if(mode)
			{
				y++;
			}
			else
			{
				fallspeed += 1;
			}
		}
		if(KEY_DOWN(' '))
		{
			attack = true;
		}
		if(KEY_DOWN('E'))
		{
			system("cls");
			color(7);
			cout << "┌──┬──┬──┬──┬──┬──┬──┬──┬──┬──┐" << endl;
			for(int i = 0; i < 9; i++)
			{
				color(7);
				cout << "│";
				for(int j = 0; j < 10; j++)
				{
					color(block[bag[i * 10 + j]].color);
					cout << block[bag[i * 10 + j]].ch;
					color(7);
					cout << "│";
				}
				cout << endl;
				color(7);
				cout << "├──┼──┼──┼──┼──┼──┼──┼──┼──┼──┤" << endl;
			}
			color(7);
			cout << "│";
			for(int j = 0; j < 10; j++)
			{
				color(block[bag[90 + j]].color);
				cout << block[bag[90 + j]].ch;
				color(7);
				cout << "│";
			}
			color(7);
			cout << endl;
			cout << "└──┴──┴──┴──┴──┴──┴──┴──┴──┴──┘" << endl;
			Sleep(3000);
		}
		if(KEY_DOWN('C'))
		{
			hungry += 100;
			hungry = min(hungry, 1000);
		}
		if(KEY_DOWN('Q'))
		{
			for(int i = 0; i < 20; i++)
			{
				if(tnt[i].time == -2)
				{
					tnt[i] = {y, x, 10, 0};
					break;
				}
			}
		}
		if(KEY_DOWN('i'))
		{
			for(int i = 0; i < 20; i++)
			{
				if(tnt[i].time == -2)
				{
					tnt[i] = {y, x, 10, 1};
					break;
				}
			}
		}
		if(KEY_DOWN('8'))
		{
			for(int i = 0; i < 20; i++)
			{
				if(tnt[i].time == -2)
				{
					tnt[i] = {y, x, 10, 2};
					break;
				}
			}
		}
		if(KEY_DOWN('9'))
		{
			for(int i = 0; i < 20; i++)
			{
				if(tnt[i].time == -2)
				{
					tnt[i] = {y, x, 10, 3};
					break;
				}
			}
		}
		if(KEY_DOWN('0'))
		{
			for(int i = 0; i < 20; i++)
			{
				if(tnt[i].time == -2)
				{
					tnt[i] = {y, x, 10, 4};
					break;
				}
			}
		}
		if(KEY_DOWN('I'))
		{
			for(int i = 0; i < 20; i++)
			{
				if(tnt[i].time == -2)
				{
					tnt[i] = {y, x, 10, 5};
					break;
				}
			}
		}
		if(KEY_DOWN('Z'))
		{
			setboard[y + 1][x] = 0;
		}
		if(KEY_DOWN('X'))
		{
			setboard[y][x] = 24;
		}
		if(KEY_DOWN('3'))
		{
			setboard[y + 1][x] = 6;
		}
		if(KEY_DOWN('4'))
		{
			setboard[y + 1][x] = 7;
		}
		if(KEY_DOWN('1'))
		{
			setboard[y + 1][x] = 8;
		}
		if(KEY_DOWN('2'))
		{
			setboard[y + 1][x] = 16;
		}
		if(KEY_DOWN('5'))
		{
			if(face == 1)
			{
				for(int i = 0; i < 100; i++)
				{
					if(arrow[i].shap == "")
					{
						arrow[i] = {"→",y,x,-0.7,2};
						break; 
					}
				}
			}
			else
			{
				for(int i = 0; i < 100; i++)
				{
					if(arrow[i].shap == "")
					{
						arrow[i] = {"←",y,x,-0.7,-2};
						break;
					}
				}
			}
		}
		if(KEY_DOWN('T'))
		{
			clear_buffer();
			system("cls");
			cout << "请输入指令(只能输一次):" << endl;
			string a;
			cout << "kill " <<"杀死自己"<<endl;
			cout << "health_full " <<"生命值加满"<<endl;
			cout << "hungry_full " <<"饱食度加满"<<endl;
			cout << "air_full " <<"氧气值加满"<<endl; 
			cout << "creative "<<"创造模式"<<endl;
			cout << "move " <<"瞬移(可能死亡!)"<<endl;
			cout << "summon "<<"有几率生成BOSS"<<endl;
			cin >> a;
			if(a == "kill")
			{
				die = "失败了。";
				hurt = true;
				health = 0; 
			}
			if(a == "health_full")
			{
				health = 1000; 
			}
			if(a == "hungry_full")
			{
				hungry = 1000;
			}
			if(a == "air_full")
			{
				breath = 100;
			}
			if(a == "creative")
			{
				mode = !mode;
			}
			if(a == "move")
			{
				cin >> y >> x;
				y = 1000 - y;
			}
			if(a == "summon")
			{
				int a;
				cin >> a;
				for(int i = 0; i < 100; i++)
				{
					if(mob[i].shap == "")
					{
						mob[i] = mobs[a];
						mob[i].x = x;
						mob[i].y = y;
						break;
					}
				}
			}
		}
		if(KEY_DOWN('O'))
		{
			for(int i = 0; i < 1000; i++)
			{
				for(int j = 0; j < 1000; j++)
				{
					setboard[i][j] = 0;
				}
			}
		}
		if(KEY_DOWN('P'))
		{
			for(int i = 0; i < 1000; i++)
			{
				for(int j = 0; j < 1000; j++)
				{
					setboard[i][j] = 2;
				}
			}
		}
		if(y > 1000)
		{
			die = "掉出了这个世界";
			hurt = true;
			health -= 200; 
		}
		if(breath <= 0)
		{
			breath = 0;
			hurt = true;
			health -= 10; 
		}
		if(!KEY_DOWN('S') && block[board[y - 1][x]].type == "block")
		{
			die = "在墙里窒息死亡";
			hurt = true;
			health -= 50;
		}
		if(board[y + 1][x] == 5)
		{
			hurt = true;
			die = "发现了地板是熔岩做的";
			health -= 30; 
		}
		clear_buffer();
	}
	return 0;
}
