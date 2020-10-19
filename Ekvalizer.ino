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

Analyzer Audio = Analyzer(6,7,0);//Strobe pin ->4  RST pin ->5 Analog Pin ->0
//Analyzer Audio = Analyzer();//Strobe->4 RST->5 Analog->0
//Version 1.3 for Spectrum analyzer
//Please download the lastest library file

#define PINproLEDky 8
#define pocetLED 180
#define STROBE 4
#define RESET 5



int hodnotafrekvence[7], frekvence[7];
int poslednipeak=0, CoXmillis;
uint8_t startIndex = 0;

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
      //nastaveni barvy,saturac a jasu
      ledky[i] = CHSV(140,255,150); // cervena

      // podminka, ktera mi nastavi peak na nejvyssi hodnotu pokud je vyssi nez predtim (+1 protoze by se mi prekryvaly modra a cerevena
      if(poslednipeak<i)
        poslednipeak=i+1;

      //toto by melo nastavit rychlost padani tecky
      CoXmillis=millis()%8;

      //podminka, ktera dela padani tecky
      if(CoXmillis==0)
      {
        poslednipeak--;
      }

      //rozsviceni na cervenou barvu peaku
      ledky[poslednipeak]=CHSV(0,255,150); //cervena

      //1. for - zhasina ledky mezi peakem a LEDkami, ktere jezdi podle hodnoty; 2. for - zhasina od peaku do konce pasku (+1 protoze by mi jinak zhasl i ten peak)
      for(int i=frekvence[0];i<poslednipeak;i++)
        ledky[i] = CHSV(0,0,0);
      for(int i=poslednipeak+1;i<=30;i++)
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
      for(int i=60;i>frekvence[1]+30;i--)
        ledky[i] = CHSV(0,0,0);
    }
FastLED.show();
}


void basy3()
{
  for(int i=60;i<=frekvence[2]+60;i++)
    {
      ledky[i] = CHSV(100,255,150);
      for(int i=90;i>frekvence[2]+60;i--)
        ledky[i] = CHSV(0,0,0);
    }
FastLED.show();
}


void stredy1()
{
  for(int i=90;i<=frekvence[3]+90;i++)
    {
      ledky[i] = CHSV(200,255,150);
      for(int i=120;i>frekvence[3]+90;i--)
        ledky[i] = CHSV(0,0,0);
    }
FastLED.show();
}


void stredy2()
{
  for(int i=120;i<=frekvence[4]+120;i++)
    {
      ledky[i] = CHSV(50,255,150);
      for(int i=150;i>frekvence[4]+120;i--)
        ledky[i] = CHSV(0,0,0);
    }
FastLED.show();
}


void vysky1()
{
  for(int i=150;i<=frekvence[5]+150;i++)
    {
      ledky[i] = CHSV(20,255,150);
      for(int i=180;i>frekvence[5]+150;i--)
        ledky[i] = CHSV(0,0,0);
    }
FastLED.show();
}
