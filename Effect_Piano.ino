#include <FastLED.h>

#define LED_PIN     7

#define NUM_LEDS    288

CRGB leds[NUM_LEDS];

using namespace std;

int f,num,vs2;

bool flag[89];

int circle;

bool re;

bool flagc;

//int gvwz[89];

int r,g,b;



void setup() {
  // put your setup code here, to run once:

Serial.begin(19200);

flagc = true;

//flagc = false;
//将本行注释掉以开启圈

pinMode (13,OUTPUT);

FastLED.addLeds<WS2812,LED_PIN,GRB>(leds , NUM_LEDS);

FastLED.setBrightness(100);//0~255

//Serial.println("ready\n");

memset(flag,false,sizeof (flag));

for (int i=1;i<NUM_LEDS-1;++i){
  leds[i] = CRGB(0,0,0);
}
leds[0] = CRGB(140,154,255);
leds[177] = CRGB(140,154,255);

FastLED.show();

//fill_rainbow(leds, 175, 0, 1);

r = 20;g = 24;b = 127;

}

void loop() {
  // put your main code here, to run repeatedly:
  num = -1;
  while (num == -1){
    num = Serial.read();
  }

  if (num == 88){
    FastLED.clear();
    memset(flag,false,sizeof (flag));
    leds[0] = CRGB(140,154,255);
    leds[177] = CRGB(140,154,255);
    circle = 0;
    FastLED.show();
    return;
  }
  else if (num == 90){
    flagc = false;
    for (int i=178;i<=287;++i){
      leds[i] = CRGB(0,0,0);
      circle = 0;
    }
    FastLED.show();
    return;
  }
  else if (num == 91){
    flagc = true;
    return;
  }
  else if (num == 89){
    r = -1;
    g = -1;
    b = -1;
    while (r == -1){
      r = Serial.read();
    }
    while (g == -1){
      g = Serial.read();
    }
    while (b == -1){
      b = Serial.read();
    }
    return;
  }
  
  vs2 = num*2;
  if (flag[num] == false){
    flag[num] = true;
    digitalWrite(13,HIGH);
    leds[vs2] = CRGB(r,g,b);
    leds[vs2-1] = CRGB(r,g,b);

    //由于供电不足，圆的部分无法与特效动作同时运行，故在执行特效动作时禁用圆
  
    if (flagc == true){
      
    
    if (circle >= 60){
      circle %= 60;
      if (re == false){
        re = true;
      }
      else re = false;
    }
    circle += 1;
    if (re == false){
      leds[circle + 226] = CRGB(39,5,64);
    }
    else {
      leds[circle + 226] = CRGB(0,0,0);
    }
    
    
  }
  
  FastLED.show();
  }
  else {
    flag[num] = false;
    digitalWrite (13,LOW);
    leds[vs2] = CRGB(0,0,0);
    leds[vs2-1] = CRGB(0,0,0);
    FastLED.show();
  }
/*
for (int i=0;i<88;++i){
    if (flag[i] == true){
      gvwz[i] += 1;  
    }
    if (gvwz[i] > 30){
      gvwz[i] = 0;
      flag[i] = false;
      leds[i*2] = CRGB(0,0,0);
      leds[i*2-1] = CRGB(0,0,0);
      FastLED.show();
      
    }
}
*/
  

  /*
  if (f <= 2){
    vs1 = (num-3)*2;
    if (f == 2){
      digitalWrite (13,LOW);
      leds[vs1] = CRGB(0,0,0);
     leds[vs1+1] = CRGB(0,0,0);
     FastLED.show();
   }
   else if (f == 1){
      digitalWrite (13,HIGH);
     leds[vs1] = CRGB(100,100,100);
     leds[vs1+1] = CRGB(100,100,100);
     FastLED.show();
   }
  }
  else {
    num = Serial.read();
    vs1 = (f-3)*2;
    leds[f] = CRGB(0,0,0);
    leds[f+1] = CRGB(0,0,0);
    FastLED.show();
  }
  */
  
  //delay (5);

  

}
