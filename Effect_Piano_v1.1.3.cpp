#include "portmidi.h"
#include "porttime.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "assert.h"
#include <Windows.h>
#include <conio.h>


#define KEY_DOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1:0)



#define INPUT_BUFFER_SIZE 100
#define OUTPUT_BUFFER_SIZE 0
#define DRIVER_INFO NULL
#define TIME_PROC ((int32_t (*)(void *)) Pt_Time)
#define TIME_INFO NULL
#define TIME_START Pt_Start(1, 0, 0) /* timer started w/millisecond accuracy */

#define STRING_MAX 80 /* used for console input */

#define LODATA 200

struct NODEC {
	int r;
	int g;
	int b;
}color[100];


int32_t latency = 0;

/* crash the program to test whether midi ports are closed */
/**/

int op, cl;
int data[200];
char commu[1];
HANDLE hCom;
DCB lpTest;
DWORD btsIO;
int nkey;
int kbcd,kbecd;
char serialname[256];
int do_not_show_numbers;
int do_not_show_channels;

BOOL igoruse;
int numofusingchannel;
int using_channel[100];
BOOL flag_using;
int numofignoredchannel;
int ignore_channel[100];
BOOL flag_ignore;
int cmpignore;

int cdplus;

BOOL flagc;
BOOL flagz;
int brightness;

BOOL rainbowstatus;
BOOL rainbowcontain;



BOOL serialopen() {

	freopen("serial_name.txt", "r", stdin);
	scanf("%s", &serialname);
	freopen("CON", "r", stdin);

	//尝试打开串口
	hCom = CreateFile(serialname,//串口名称（请自行通过设备管理器查询串口名称） 
		GENERIC_READ | GENERIC_WRITE, //允许读和写
		0, //独占方式
		0,
		OPEN_EXISTING, //打开而不是创建
		0, //同步方式
		0);

	//检测串口是否已经连接
	if (hCom != INVALID_HANDLE_VALUE) {
		printf("Serial connected\n");

		GetCommState(hCom, &lpTest);  //获取当前的参数设置
		lpTest.BaudRate = CBR_19200;       //设置波特率
		lpTest.ByteSize = 8;              //数据位数为8
		lpTest.Parity = NOPARITY;         //无校验
		lpTest.StopBits = ONESTOPBIT;   //1位停止位
		SetCommState(hCom, &lpTest);  //设置通信参数
		return TRUE;
	}
	else {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
		printf("Serial connection failed .\nPlease check the serial name in serial_name.txt\n\nPress Enter to retry\nPress ESC to quit\n");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
		while (!KEY_DOWN(13) || kbcd > 10) {
			if (KEY_DOWN(27)) {
				return FALSE;
			}
			kbcd -= 1;
			Sleep(20);
		}
		kbcd += 100;
		serialopen();
	}
	//return FALSE;

}

void pta(int a) {
	cl++;
	if (cl >= LODATA) {
		cl -= LODATA;
	}
	data[cl] = a;
}
void push() {
	if (op != cl) {
		++op;
		if (op >= LODATA) {
			op -= LODATA;
		}
		commu[0] = data[op];
		WriteFile(hCom, commu, strlen(commu), &btsIO, NULL);
		if (do_not_show_numbers != 1) {
			printf("key=%d	", data[op]);
		}
		Sleep(latency);
		push();
	}
	else {
		Sleep(latency);
		return;
	}
}


void effect1(int n) {
	pta(90);
	push();
	Sleep(latency);
	for (int j = 1; j <= n; ++j) {
		pta(j);
		push();
		Sleep(latency);
	}
	for (int j = n + 1; j <= 87; ++j) {
		pta(j);
		pta(j - n);
		push();
		Sleep(latency);
	}
	for (int j = 88 - n; j <= 87; ++j) {
		pta(j);
		push();
		Sleep(latency);
	}
	pta(91);
	push();
	Sleep(latency);
	pta(88);
	push();
	Sleep(latency);
}



void doSomethingReallyStupid() {
	int * tmp = NULL;
	*tmp = 5;
}


/* exit the program without any explicit cleanup */
/**/
void doSomethingStupid() {
	assert(0);
}


/* read a number from console */
/**/
int get_number(char *prompt)
{
    char line[STRING_MAX];
    int n = 0, i;
    printf(prompt);
    while (n != 1) {
        n = scanf("%d", &i);
        fgets(line, STRING_MAX, stdin);

    }
    return i;
}


/*
 * the somethingStupid parameter can be set to simulate a program crash.
 * We want PortMidi to close Midi ports automatically in the event of a
 * crash because Windows does not (and this may cause an OS crash)
 */
void main_test_input(unsigned int somethingStupid) {
    PmStream * midi;
    PmError status, length;
    PmEvent buffer[1];
    //int num = 100;
    int i = get_number("Type input number: ");
    /* It is recommended to start timer before Midi; otherwise, PortMidi may
       start the timer with its (default) parameters
     */
    TIME_START;

    /* open input device */
    Pm_OpenInput(&midi, 
                 i,
                 DRIVER_INFO, 
                 INPUT_BUFFER_SIZE, 
                 TIME_PROC, 
                 TIME_INFO);

    printf("Midi Input opened. Reading Midi messages...\n");
    Pm_SetFilter(midi, PM_FILT_ACTIVE | PM_FILT_CLOCK | PM_FILT_SYSEX);
    /* empty the buffer after setting filter, just in case anything
       got through */
    while (Pm_Poll(midi)) {
        Pm_Read(midi, buffer, 1);
    }
    /* now start paying attention to messages */
    //i = 0; /* count messages as they arrive */
    while (1) {

		for (int ch = 49; ch < 59; ++ch) {
			if (KEY_DOWN(ch) && kbcd <=10) {
				pta(89);
				pta(color[ch - 49].r);
				pta(color[ch - 49].g);
				pta(color[ch - 49].b);
				push();
				kbcd += cdplus;
				printf("\nChange_Color:%d %d %d\n", color[ch - 49].r, color[ch - 49].g, color[ch - 49].b);
			}
		}
		if (KEY_DOWN(27)) {
			break;
		}
		if (KEY_DOWN(81) && kbecd <= 10) {
			printf("\nEffect1\n");
			effect1(10);
			kbecd += cdplus;
		}
		if (KEY_DOWN(8) && kbcd <= 10) {
			pta(88);
			push();
			printf("\nCleared\n");
			kbcd += cdplus;
		}
		if (KEY_DOWN(67) && kbcd <= 10) {
			if (flagc == TRUE) {
				flagc = FALSE;
				pta(90);
				push();
				printf("\nCircle off\n");
				kbcd += cdplus;
			}
			else {
				flagc = TRUE;
				pta(91);
				push();
				printf("\nCircle on\n");
				kbcd += cdplus;
			}
		}
		if (KEY_DOWN(90) && kbcd <= 10) {
			if (flagz == TRUE) {
				flagz = FALSE;
				pta(92);
				push();
				printf("\nThe LEDs on both ends are off\n");
				kbcd += cdplus;
			}
			else {
				flagz = TRUE;
				pta(92);
				push();
				printf("\nThe LEDs on both ends are on\n");
				kbcd += cdplus;
			}
		}
		if (KEY_DOWN(88) && kbcd <= 10) {
			freopen("CON", "r", stdin);
			printf("\nType the brightness\n");
			scanf("x%d", &brightness);
			if (brightness >= 0 && brightness <= 255) {
				pta(93);
				pta(brightness);
				push();
				printf("\nBrightness : %d\n", brightness);
				kbcd += cdplus;
			}
			else {
				printf("Invalid brightness value . This number should be in the range [0,255]");
				kbcd += cdplus;
			}
		}
		if (KEY_DOWN(82)) {
			if (rainbowstatus == FALSE) {
				pta(93);
				pta(20);
				push();
				pta(90);
				
				push();
				pta(94);
				push();
				rainbowstatus = TRUE;
				
			}
			else {
				pta(94);
				push();
				Sleep(latency*2);
			}
		}
		else {
			if (rainbowstatus == TRUE) {
				pta(95);
				push();
				pta(93);
				pta(brightness);
				rainbowstatus = FALSE;
				if (flagc == TRUE) {
					pta(91);
					push();
				}
			}
		}
		if (KEY_DOWN(84) && kbcd <= 10) {
			if (rainbowcontain == FALSE) {
				pta(93);
				pta(20);
				push();
				pta(90);
				push();
				pta(94);
				push();
				flagc = FALSE;
				
				rainbowcontain = TRUE;
			}
			else {
				pta(95);
				push();
				pta(93);
				pta(brightness);
				rainbowcontain = FALSE;
				pta(88);
				push();
				
				
				if (flagc == TRUE) {
					pta(91);
					push();
					flagc = TRUE;
				}
			}
			kbcd += cdplus/2;
		}
		
		if (kbcd > 0) {
			kbcd -= 1;
		}
		if (kbecd > 0) {
			kbecd -= 1;
		}

		if (rainbowcontain == TRUE) {
			pta(94);
			push();
			Sleep(latency*5);
			continue;
		}
		

        status = Pm_Poll(midi);
        if (status == TRUE) {
            length = Pm_Read(midi,buffer, 1);
            if (length > 0) {
                /*printf("time %ld, %d %d %2lx\n",
                       
                       (long) buffer[0].timestamp,
                       (long) Pm_MessageStatus(buffer[0].message),
                       (long) Pm_MessageData1(buffer[0].message),
                       (long) Pm_MessageData2(buffer[0].message));
					   */
				cmpignore = Pm_MessageStatus(buffer[0].message);

				if (igoruse == FALSE) {
					flag_ignore = FALSE;
					for (int i = 0; i < numofignoredchannel; ++i) {
						if (cmpignore == ignore_channel[i]) {
							flag_ignore = TRUE;
							break;
						}
					}
					if (flag_ignore == FALSE) {
						if (do_not_show_channels != 1) {
							printf("channal=%d	", Pm_MessageStatus(buffer[0].message));
						}
						pta(Pm_MessageData1(buffer[0].message) - 20);
						push();
					}
				}
				else {
					flag_using = FALSE;
					for (int i = 0; i < numofusingchannel; ++i) {
						if (cmpignore == using_channel[i]) {
							flag_using = TRUE;
							break;
						}
					}
					if (flag_using == TRUE) {
						printf("channal = %d	", Pm_MessageStatus(buffer[0].message));
						pta(Pm_MessageData1(buffer[0].message) - 20);
						push();
					}
				}

				
				

            } else {
                assert(0);
            }
        }
        /* simulate crash if somethingStupid is 1 or 2 */
		/*
        if ((i > (num/2)) && (somethingStupid == 1)) {
            doSomethingStupid();
        } else if ((i > (num/2)) && (somethingStupid == 2)) {
            doSomethingReallyStupid();
        }
		*/
		Sleep(latency);
		
    }

    /* close device (this not explicitly needed in most implementations) */
    printf("\nready to close...");

    Pm_Close(midi);
    printf("done closing...");
}


/* main_test_stream exercises windows winmm API's stream mode */
/*    The winmm stream mode is used for latency>0, and sends
   timestamped messages. The timestamps are relative (delta) 
   times, whereas PortMidi times are absolute. Since peculiar
   things happen when messages are not always sent in advance,
   this function allows us to exercise the system and test it.
 */

void show_usage()
{
    printf("Usage: test [-h] [-l latency-in-ms]\n");
    exit(0);
}

int main(int argc, char *argv[])
{
	//const int LODATA = 200;
	
	printf("Loading...\n\n");
	int default_in;
    int default_out;
    int i = 0, n = 0;
    char line[STRING_MAX];
    int test_input = 0, test_output = 0, test_both = 0, somethingStupid = 0;
    int stream_test = 0;
    int latency_valid = FALSE;
	int nocolor;
	int always_show_in_front;
	flagc = TRUE;
	flagz = TRUE;

	char contitle[255];
	HWND desktop,selfhwnd;
	RECT desktoprect;
	desktop = GetDesktopWindow();
	GetWindowRect(desktop, &desktoprect);
	GetConsoleTitle(contitle, 255);
	selfhwnd = FindWindow("ConsoleWindowClass", contitle);
	MoveWindow(selfhwnd, (desktoprect.right - desktoprect.left) / 3*2, 0, (desktoprect.right - desktoprect.left) / 3,desktoprect.bottom - desktoprect.top,FALSE);
	
	freopen("always_show_in_front.txt", "r", stdin);
	scanf("%d", &always_show_in_front);
	freopen("CON", "r", stdin);
	if (always_show_in_front == 1) {
		SetWindowPos(selfhwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
		printf("\nThis window has been fixed in the foreground\n\n");
	}
	
	
	
    /*
    if (sizeof(void *) == 8) 
        printf("Apparently this is a 64-bit machine.\n");
    else if (sizeof(void *) == 4) 
        printf ("Apparently this is a 32-bit machine.\n");
		*/
    /*
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0) {
            show_usage();
        } else if (strcmp(argv[i], "-l") == 0 && (i + 1 < argc)) {
            i = i + 1;
            latency = atoi(argv[i]);
            printf("Latency will be %ld\n", (long) latency);
            latency_valid = TRUE;
        } else {
            show_usage();
        }
    }
	*/
	

	freopen("latency.txt", "r", stdin);
    while (!latency_valid) {
        int lat; // declared int to match "%d"
        printf("Latency in ms: ");
		
        if (scanf("%d", &lat) == 1) {
            latency = (int32_t) lat; // coerce from "%d" to known size
	    latency_valid = TRUE;
        }
		else if (lat == 0 || lat >= 1000) {
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
			printf("Need file : latency.txt\nThe number should in the range (0,1000)\n");
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
			return 0;
		}

		printf("%d .\nChange it in latency.txt if you want\n\n", lat);
    }
	freopen("CON", "r", stdin);
	cdplus = 300 / latency;
	
	
	freopen("do_not_show_numbers_or_channels.txt", "r", stdin);
	scanf("%d%d", &do_not_show_numbers,&do_not_show_channels);
	freopen("CON", "r", stdin);
	if (do_not_show_numbers == 1) {
		printf("Numbers logs will not be shown .\n");
	}
	if (do_not_show_channels == 1) {
		printf("Channels logs will not be shown.\n");
	}
	printf("Change it in do_not_show_numbers_or_channels.txt if you want\n\n");

	
	freopen("serial_name.txt", "r", stdin);
	scanf("%s", &serialname);
	freopen("CON", "r", stdin);
	
	freopen("color.txt", "r", stdin);
	scanf("%d", &nocolor);
	
	if (nocolor == 0) {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
		printf("No color found\nNeed file : color.txt\n");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
		return 0;
	}
	else {
		
		for (int i = 0; i < nocolor; ++i) {
			scanf("%d%d%d", &color[i].r, &color[i].g, &color[i].b);
		}
		printf("%d color loaded .\nAdd colors in color.txt if you want\n\n", nocolor);
	}
	
	freopen("CON", "r", stdin);

	freopen("using_channel.txt", "r", stdin);
	scanf("%d", &numofusingchannel);
	if (numofusingchannel != 0) {
		igoruse = TRUE;
		printf("Mode :using channel\nIf you want to switch to ignore channel mode , change the first number in using_channel.txt to 0\n");
		printf("%d channel will be read\n", numofusingchannel);
		for (int i = 0; i < numofusingchannel; ++i) {
			scanf("%d", &using_channel[i]);
			printf("%d	", using_channel[i]);
		}
		printf("\nChange them in using_channel.txt if you want\n\n");
	}
	freopen("CON", "r", stdin);

	if (igoruse == FALSE) {
		freopen("ignored_channel.txt", "r", stdin);
		scanf("%d", &numofignoredchannel);
		for (int i = 0; i < numofignoredchannel; ++i) {
			scanf("%d", &ignore_channel[i]);
		}
		printf("Mode : ignore cahnnel\nIgnored Channel :\n");
		for (int i = 0; i < numofignoredchannel; ++i) {
			printf("%d	", ignore_channel[i]);
		}
		printf("\n %d channel(s) will be ignored.\nChange it in ignored_channel.txt if you want\n\n", numofignoredchannel);
	}
	
   
    
	test_input = 1;

	if (serialopen() == FALSE) {
		return 0;
	}



    /* list device information */
    default_in = Pm_GetDefaultInputDeviceID();
    default_out = Pm_GetDefaultOutputDeviceID();
    for (i = 0; i < Pm_CountDevices(); i++) {
        char *deflt;
        const PmDeviceInfo *info = Pm_GetDeviceInfo(i);
        if (((test_input  | test_both) & info->input) |
            ((test_output | test_both | stream_test) & info->output)) {
            printf("%d: %s, %s", i, info->interf, info->name);
            if (info->input) {
                deflt = (i == default_in ? "default " : "");
                printf(" (%sinput)", deflt);
            }
            if (info->output) {
                deflt = (i == default_out ? "default " : "");
                printf(" (%soutput)", deflt);
            }
            printf("\n");
        }
    }

	

    
    /* run test */
	/*
    if (stream_test) {
        main_test_stream();
    } else if (test_input) {
        main_test_input(somethingStupid);
    } else if (test_output) {
        main_test_output();
    } else if (test_both) {
        main_test_both();
    }
	*/
	main_test_input(somethingStupid);
    
    printf("\ntype ENTER again to quit...");
    fgets(line, STRING_MAX, stdin);
    return 0;
}
