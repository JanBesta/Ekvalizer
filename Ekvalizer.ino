#include<math.h>

#include <bitswap.h>
#include <chipsets.h>
#include <color.h>
#include <colorpalettes.h>
#include <colorutils.h>
#include <controller.h>
#include <cpp_compat.h>
#include <dmx.h>
#include <FastLED.h>
#include <fastled_config.h>
#include <fastled_delay.h>
#include <fastled_progmem.h>
#include <fastpin.h>
#include <fastspi.h>
#include <fastspi_bitbang.h>
#include <fastspi_dma.h>
#include <fastspi_nop.h>
#include <fastspi_ref.h>
#include <fastspi_types.h>
#include <hsv2rgb.h>
#include <led_sysdefs.h>
#include <lib8tion.h>
#include <noise.h>
#include <pixelset.h>
#include <pixeltypes.h>
#include <platforms.h>
#include <power_mgt.h>
#include <AudioAnalyzer.h>

#include <LiquidCrystal.h> 

Analyzer Audio = Analyzer(12,13,0);//Strobe pin ->12  RST pin ->13 Analog Pin ->0
//Analyzer Audio = Analyzer();//Strobe->12 RST->13 Analog->0
//Version 1.3 for Spectrum analyzer
//Please download the lastest library file

#define PINproLEDky 8
#define pocetLED 180
#define STROBE 4
#define RESET 5
#define padani 50


int hodnotafrekvence[7], frekvence[7];
int poslednipeak1=0;
unsigned int aktualniMillis1, predchoziMillis1=0;
int poslednipeak2=0;
unsigned int aktualniMillis2, predchoziMillis2=0;
int poslednipeak3=0;
unsigned int aktualniMillis3, predchoziMillis3=0;
int poslednipeak4=0;
unsigned int aktualniMillis4, predchoziMillis4=0;
int poslednipeak5=0;
unsigned int aktualniMillis5, predchoziMillis5=0;
int poslednipeak6=0;
unsigned int aktualniMillis6, predchoziMillis6=0;


CRGB ledky[pocetLED];



// hlavicky funkci
void basy1();
void basy2();
void basy3();
void stredy1();
void stredy2();
void vysky1();

void setup()
{

  Serial.begin(57600);
  FastLED.addLeds<WS2812B,PINproLEDky, GRB>(ledky,pocetLED);
  Audio.Init();//Init module

  analogWrite(9,20);
  pinMode(STROBE,OUTPUT);
  pinMode(RESET,OUTPUT); 
}

void loop()
{
  Audio.ReadFreq(hodnotafrekvence);//Return 7 values of 7 bands pass filter
                          //Frequency(Hz):          63  160  400  1K  2.5K  6.25K  16K
                          //hodnotaFrekvence[]:      0    1    2    3    4    5    6  


  //omezeni hodnot aby byly jen kladne; mapovani, aby u neaktivniho pasku nesvitilo treba 5 ledek a aby hodnota odpovidala jedne ledce; a nasledne debugovani s vypisem hodnot
  for (int i = 0;i<7;i++){
    hodnotafrekvence[i]=constrain(hodnotafrekvence[i],0,1023);
    frekvence[i]=map(hodnotafrekvence[i],145,1023,0,30);
    Serial.print(hodnotafrekvence[i]);//used for debugging and Freq choosing
    Serial.print(" ");
  }
  //znovu vypis ale uz mapovanych hodnot
  for(int i=0;i<=6;i++)
  {
    Serial.print(frekvence[i]);
    Serial.print(" ");
  }
  Serial.println(" ");
  //FUNKCE S TECKOU (PEAKEM) JE: basy1
  basy1();
  basy2();
  basy3();
  stredy1();
  stredy2();
  vysky1();
}
//TATO FUNKCE JE S PEAKEM
void basy1()
{
  //naprogramovani ktere ledky maji svitit (vizualni zobrazeni je az na konci fce) podle ctene hodnoty frekvence)
  for(int i=0;i<=frekvence[0];i++)
    {
      //nastaveni barvy,saturace a jasu
      ledky[i] = CHSV(140,255,150); // azurove modra

      // podminka, ktera mi nastavi peak na nejvyssi hodnotu pokud je vyssi nez predtim (+1 protoze by se mi prekryvaly modra a cerevena
      if(poslednipeak1<i)
        poslednipeak1=i+1;

      aktualniMillis1=millis();
      //podminka, ktera dela padani tecky
      if(aktualniMillis1-predchoziMillis1>padani)
      {
        poslednipeak1--;
        predchoziMillis1=aktualniMillis1;
      }
      ledky[poslednipeak1]=CHSV(0,255,150); //cervena
      //1. for - zhasina ledky mezi peakem a LEDkami, ktere jezdi podle hodnoty; 2. for - zhasina od peaku do konce pasku (+1 protoze by mi jinak zhasl i ten peak)
      for(int i=frekvence[0];i<poslednipeak1;i++)
        ledky[i] = CHSV(0,0,0);
      for(int i=poslednipeak1+1;i<=30;i++)
        ledky[i] = CHSV(0,0,0);
    }
      
// TOTO TEPRVE ZOBRAZI VIZUALNE BARVY    
FastLED.show();
}


void basy2()
{
  for(int i=30;i<=frekvence[1]+30;i++)
    {
      ledky[i] = CHSV(170,255,150);

      if(poslednipeak2<i)
        poslednipeak2=i+1;

      aktualniMillis2=millis();

      if(aktualniMillis2-predchoziMillis2>padani)
      {
        poslednipeak2--;
        predchoziMillis2=aktualniMillis2;
      }
      ledky[poslednipeak2]=CHSV(0,255,150); //cervena

      for(int i=frekvence[1]+30;i<poslednipeak2;i++)
        ledky[i] = CHSV(0,0,0);
      for(int i=poslednipeak2+1;i<=60;i++)
        ledky[i] = CHSV(0,0,0);
    }
FastLED.show();
}


void basy3()
{
  for(int i=60;i<=frekvence[2]+60;i++)
    {
      ledky[i] = CHSV(100,255,150);

      if(poslednipeak3<i)
        poslednipeak3=i+1;

      aktualniMillis3=millis();

      if(aktualniMillis3-predchoziMillis3>padani)
      {
        poslednipeak3--;
        predchoziMillis3=aktualniMillis3;
      }
      ledky[poslednipeak3]=CHSV(0,255,150); //cervena

      for(int i=frekvence[2]+60;i<poslednipeak3;i++)
        ledky[i] = CHSV(0,0,0);
      for(int i=poslednipeak3+1;i<=90;i++)
        ledky[i] = CHSV(0,0,0);
    }
FastLED.show();
}


void stredy1()
{
  for(int i=90;i<=frekvence[3]+90;i++)
    {
      ledky[i] = CHSV(200,255,150);

      if(poslednipeak4<i)
        poslednipeak4=i+1;

      aktualniMillis4=millis();

      if(aktualniMillis4-predchoziMillis4>padani)
      {
        poslednipeak4--;
        predchoziMillis4=aktualniMillis4;
      }
      ledky[poslednipeak4]=CHSV(0,255,150); //cervena

      for(int i=frekvence[3]+90;i<poslednipeak4;i++)
        ledky[i] = CHSV(0,0,0);
      for(int i=poslednipeak4+1;i<=120;i++)
        ledky[i] = CHSV(0,0,0);
    }
FastLED.show();
}


void stredy2()
{
  for(int i=120;i<=frekvence[4]+120;i++)
    {
      ledky[i] = CHSV(50,255,150);

      if(poslednipeak5<i)
        poslednipeak5=i+1;

      aktualniMillis5=millis();

      if(aktualniMillis5-predchoziMillis5>padani)
      {
        poslednipeak5--;
        predchoziMillis5=aktualniMillis5;
      }
      ledky[poslednipeak5]=CHSV(0,255,150); //cervena

      for(int i=frekvence[5]+120;i<poslednipeak5;i++)
        ledky[i] = CHSV(0,0,0);
      for(int i=poslednipeak5+1;i<=150;i++)
        ledky[i] = CHSV(0,0,0);
    }
FastLED.show();
}


void vysky1()
{
  for(int i=150;i<=frekvence[5]+150;i++)
    {
      ledky[i]=CHSV(20,255,150);

        if(poslednipeak6<i)
        poslednipeak6=i+1;

      aktualniMillis6=millis();

      if(aktualniMillis6-predchoziMillis6>padani)
      {
        poslednipeak6--;
        predchoziMillis6=aktualniMillis6;
      }
      ledky[poslednipeak6]=CHSV(0,255,150); //cervena

      for(int i=frekvence[5]+150;i<poslednipeak6;i++)
        ledky[i] = CHSV(0,0,0);
      for(int i=poslednipeak6+1;i<=180;i++)
        ledky[i] = CHSV(0,0,0);
    }
FastLED.show();
}
