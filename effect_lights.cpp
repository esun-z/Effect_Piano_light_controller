#include <cstdio>
#include <windows.h>
#include <winuser.h>
#include <conio.h>
#include <cstring>

using namespace std;

const int iavi = 16;//这是一个差值，用来调整（向右平移）读取像素的横向位置 
const int LODATA = 200;//队列长度，取决于一次发送的数据的最大数量 
const int hokey = 650;//读取像素的高度，取决于窗口，需自行根据屏幕分辨率更改 

HWND desktop,hd1,hd2;
RECT rectod,rect1,rect2;
int wos,hos;
char strTitle[255];

float wokey;
int position[88];

int noc;

struct nodec{
	int r;
	int g;
	int b;
};
nodec color[100];

HDC hdc;
int red,green,blue;
COLORREF clr;

HANDLE hCom;
DCB lpTest;
DWORD btsIO;

int ch;
int op,cl;
int data[LODATA];
char commu[1];

bool flags[88];


void pta (int a){
	cl++;
	if (cl >= LODATA){
		cl -= LODATA;
	}
	data[cl] = a;
}
void push(){
	if (op != cl){
		++op;
		if (op >= LODATA){
			op -= LODATA;
		}
		commu[0] = data[op];
		WriteFile(hCom,commu,strlen(commu),&btsIO,NULL);
		printf ("%d	",data[op]);
		Sleep(3);
		push();
	}
	else {
		Sleep(3);
		return;
	}
}
void effect1(int n){
	pta (90);
	push();
	Sleep(2);
	for (int i=1;i<=n;++i){
		pta (i);
		push();
		Sleep(5);
	}
	for (int i=n+1;i<=87;++i){
		pta (i);
		pta (i-n);
		push ();
		Sleep (5);
	}
	for (int i=88-n;i<= 87;++i){
		pta (i);
		push ();
		Sleep (5);
	}
	pta (91);
	push();
	Sleep(2);
}
//注：控制台文字全部英文以防止由于编码不同带来的乱码问题 

int main (){
	
	desktop = GetDesktopWindow ();
	GetWindowRect(desktop,&rectod);
	printf ("Desktop: %d * %d\n",rectod.right - rectod.left,rectod.bottom - rectod.top);
	//获取桌面分辨率 
	
	hd1 = FindWindow(NULL,"Synthesia 10.1.3320");
	ShowWindow(hd1,SW_RESTORE);
	Sleep(100);
	SetForegroundWindow (hd1);
	GetWindowRect (hd1,&rect1);
	hos = rect1.bottom - rect1.top;
	wos = rect1.right - rect1.left;
	printf ("Syn: %d * %d\n",wos,hos);
	MoveWindow(hd1,0,0,wos,hos,false);
	//调整Syn窗口 
	
	wokey = (float)(wos-iavi)/89;
	for (int i=0;i<88;++i){
		position[i] = wokey * (i+1);
	} 
	printf ("Wide of key: %.2f\n",wokey);
	//计算Syn窗口中键盘按键位置 
	
	GetConsoleTitle(strTitle, 255);
	hd2 = FindWindow("ConsoleWindowClass",strTitle);
	GetWindowRect (hd2,&rect2);
	MoveWindow (hd2,wos,0,rectod.right - wos,rectod.bottom,false);
	Sleep (100);
	SetForegroundWindow (hd2);
	//调整控制台窗口 
	
	freopen ("color.txt","r",stdin);
	scanf ("%d",&noc);
	for (int i=0;i<noc;++i){
		scanf ("%d%d%d",&color[i].r,&color[i].g,&color[i].b);
	}
	freopen ("CON","r",stdin);
	//读取颜色预设（color.txt） 
	printf ("%d colors got\nPress any key to continue\n",noc);
	
	while (!_kbhit()){
		Sleep (20);
	}
	Sleep (100);
	//按下键盘任意键以开始窗口屏幕捕获和串口通讯 
	
	hdc = GetDC(hd1);
	
	hCom=CreateFile("COM3",//COM3串口（请自行通过设备管理器查询串口名称） 
	GENERIC_READ|GENERIC_WRITE, //允许读和写
	0, //独占方式
	0,
	OPEN_EXISTING, //打开而不是创建
	0, //同步方式
	0);
	//打开串口 
	
	if (hCom != INVALID_HANDLE_VALUE){
		printf ("Serial connected\n");
		
        GetCommState(hCom,&lpTest);  //获取当前的参数设置
        lpTest.BaudRate=CBR_19200;       //设置波特率
        lpTest.ByteSize=8;              //数据位数为8
        lpTest.Parity=NOPARITY;         //无校验
        lpTest.StopBits = ONESTOPBIT;   //1位停止位
        SetCommState(hCom,&lpTest);  //设置通信参数
	}
	else {
		printf ("Serial connection failed\n");
		return 0;
	} 
	//验证串口状态，若未连接串口则退出，否则进行设置 
	
	while (1){
		if (_kbhit()){
			ch = _getch();
			if (ch == 27){
				break;
			}
			else {
				if (ch > 48 and ch < 58){
					pta (89);
					pta (color[ch-49].r);
					pta (color[ch-49].g);
					pta (color[ch-49].b);
					push();
					printf ("\nChange_Color:%d %d %d\n",color[ch-49].r,color[ch-49].g,color[ch-49].b);
				}
				else if (ch == 48){
					effect1(10);
					printf ("\nEffect1\n");
				}
				else if (ch == 45){
					//effect2();
					printf ("\nEffect2\n");
				}
				else if (ch == 61){
					//effect3();
					printf ("\nEffect3\n");
				}
			}
			
		}
		for (int i=0;i<88;++i){
			clr = GetPixel (hdc,position[i],hokey);
			red = GetRValue (clr);
			green = GetGValue (clr);
			blue = GetBValue (clr);
			if (green - blue - red > 0){
				if (flags[i] == false){
					pta (i+1);
					flags[i] = true;
				}
			}
			else if (flags[i] == true){
				pta (i+1);
				flags[i] = false;
			}
		}
		push();
		Sleep (3);
	}
	//程序主体部分 
	
	
	return 0;
} 
