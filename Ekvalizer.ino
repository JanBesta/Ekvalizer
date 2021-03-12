#include <FastLED.h>
#include <LiquidCrystal_I2C.h>
#include <AudioAnalyzer.h>
#include <Wire.h>

Analyzer Audio = Analyzer(12,13,0);//Strobe pin ->12  RST pin ->13 Analog Pin ->0
//Verze 1.3 pro spektrální analyzer

#define PINproLEDky 8
#define pocetLED 210
#define STROBE 4
#define RESET 5

//Promenne pro ekvalizer
int hodnotafrekvence[7], frekvence[7], frekvence1[7], frekvence2[7], frekvenceZBoku[7], frekvenceBarva[7];
unsigned long jas;
unsigned long padani;
unsigned long barevnyIndex;
unsigned long barva;
unsigned long barvaTecky, barevnyIndex1;
int volba=1;

int poslednipeak1=0, poslednipeak2=0, poslednipeak3=0, poslednipeak4=0, poslednipeak5=0, poslednipeak6=0, poslednipeak7=0;
unsigned long aktualniMillis, aktualniMillis1, predchoziMillis1=0, aktualniMillis2, predchoziMillis2=0, aktualniMillis3, predchoziMillis3=0, aktualniMillis4, predchoziMillis4=0, aktualniMillis5, predchoziMillis5=0, aktualniMillis6, predchoziMillis6=0, aktualniMillis7, predchoziMillis7=0;

long poslednipeak011=0, poslednipeak012=0, poslednipeak111=0, poslednipeak112=0, poslednipeak211=0, poslednipeak212=0, poslednipeak311=0, poslednipeak312=0, poslednipeak411=0, poslednipeak412=0, poslednipeak511=0, poslednipeak512=0, poslednipeak611=0, poslednipeak612=0;
unsigned long aktualniMillis011, predchoziMillis011=0, aktualniMillis012, predchoziMillis012=0, aktualniMillis111, predchoziMillis111=0, aktualniMillis112, predchoziMillis112=0, aktualniMillis211, predchoziMillis211=0, aktualniMillis212, predchoziMillis212=0; 
unsigned long aktualniMillis311, predchoziMillis311=0, aktualniMillis312, predchoziMillis312=0, aktualniMillis411, predchoziMillis411=0, aktualniMillis412, predchoziMillis412=0, aktualniMillis511, predchoziMillis511=0, aktualniMillis512, predchoziMillis512=0, aktualniMillis611, predchoziMillis611=0, aktualniMillis612, predchoziMillis612=0;

CRGB ledky[pocetLED];

// hlavicky funkci
void basy01();
void basy02();
void basy03();
void stredy01();
void stredy02();
void vysky01();
void vysky02();

void basy11();
void basy12();
void basy13();
void stredy11();
void stredy12();
void vysky11();
void vysky12();

void basy21();
void basy22();
void basy23();
void stredy21();
void stredy22();
void vysky21();
void vysky22();

void basybasic1();
void basybasic2();
void basybasic3();
void stredybasic1();
void stredybasic2();
void vyskybasic1();
void vyskybasic2();

void basybasic11();
void basybasic12();
void basybasic13();
void stredybasic11();
void stredybasic12();
void vyskybasic11();
void vyskybasic12();

void zBoku1();
void zBoku2();
void zBoku3();
void zBoku4();
void zBoku5();
void zBoku6();
void zBoku7();

void animace1();

void lcd1();

void tlacitko1();
void tlacitko2();


//LCD displej s I2C prevodnikem
LiquidCrystal_I2C lcd(0x27,16,2);

//animacni mod
uint8_t startIndex = 0;

void setup()
{
  Serial.begin(57600);
  Wire.setClock(100000);
  FastLED.addLeds<WS2812B,PINproLEDky, GRB>(ledky,pocetLED);
  Audio.Init();
  pinMode(STROBE,OUTPUT);
  pinMode(RESET,OUTPUT);
  
  attachInterrupt(digitalPinToInterrupt(2),tlacitko1,RISING);
  attachInterrupt(digitalPinToInterrupt(3),tlacitko2,RISING);
  
  pinMode(A0,INPUT);
  pinMode(A1,INPUT);
  pinMode(A2,INPUT);
  pinMode(A3,INPUT);
  pinMode(A4,INPUT);
  pinMode(A5,INPUT);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print(" Prave probiha:");
  lcd.setCursor(1,1);
  lcd.print("Test LED Pasku");
  
  for(int i = 0;i<30;i++)
  {
    ledky[i] = CHSV(0,255,100);
    delay(20);
    FastLED.show();
  }
  for(int i = 30;i<60;i++)
  {
    ledky[i] = CHSV(74,255,100);
    delay(20);
    FastLED.show();
  }
  for(int i = 60;i<90;i++)
  {
    ledky[i] = CHSV(111,255,100);
    delay(20);
    FastLED.show();
  }
  for(int i = 90;i<120;i++)
  {
    ledky[i] = CHSV(37,255,100);
    delay(20);
    FastLED.show();
  }
  for(int i = 120;i<150;i++)
  {
    ledky[i] = CHSV(222,255,100);
    delay(20);
    FastLED.show();
  }
  for(int i = 150;i<180;i++)
  {
    ledky[i] = CHSV(50,255,100);
    delay(20);
    FastLED.show();
  }
  for(int i = 180;i<210;i++)
  {
    ledky[i] = CHSV(148,255,100);
    delay(20);
    FastLED.show();
  }
  delay(100);
  for(int i = 0;i<210;i++)
  {
    ledky[i] = CHSV(0,0,0);
  }
  FastLED.show();
  
  lcd.clear();
  delay(1000);
}

void loop()
{
  Audio.ReadFreq(hodnotafrekvence);//vraci 7 hodnot 7i pasmovych propusti
                          //Frekvence(Hz):          63  160  400  1k  2.5k  6.25k  16k
                          //hodnotaFrekvence[]:      0    1    2    3    4    5    6  
  //omezeni hodnot aby byly jen kladne; mapovani, aby u neaktivniho pasku nesvitilo treba 5 ledek a aby hodnota odpovidala jedne ledce; a nasledne debugovani s vypisem hodnot
  for (int i = 0;i<7;i++)
  {
    hodnotafrekvence[i]=constrain(hodnotafrekvence[i],0,1023);
    frekvence[i]=map(hodnotafrekvence[i],145,1023,0,30);
    Serial.print(hodnotafrekvence[i]);//debuggovani
    Serial.print(" ");
  }
  Serial.println("");
  //znovu vypis ale uz mapovanych hodnot
  for(int i=0;i<=6;i++)
  {
    Serial.print(frekvence[i]);
    Serial.print(" ");
  }
  Serial.println(" ");

  for (int i = 0;i<7;i++)
  {
    hodnotafrekvence[i]=constrain(hodnotafrekvence[i],0,1023);
    frekvence1[i]=map(hodnotafrekvence[i],145,1023,14,29);
    Serial.print(hodnotafrekvence[i]);//debuggovani
    Serial.print(" ");
  }
  Serial.println(" ");

  for(int i=0;i<=6;i++)
  {
    Serial.print(frekvence1[i]);
    Serial.print(" ");
  }
  Serial.println(" ");
  
  for (int i = 0;i<7;i++)
  {
    hodnotafrekvence[i]=constrain(hodnotafrekvence[i],0,1023);
    frekvence2[i]=map(hodnotafrekvence[i],145,1023,14,0);
    Serial.print(hodnotafrekvence[i]);//debugovani
    Serial.print(" ");
  }
  Serial.println(" ");
  
  for(int i=0;i<=6;i++)
  {
    Serial.print(frekvence2[i]);
    Serial.print(" ");
  }
  Serial.println(" ");

for (int i = 0;i<7;i++)
  {
    hodnotafrekvence[i]=constrain(hodnotafrekvence[i],0,1023);
    frekvenceZBoku[i]=map(hodnotafrekvence[i],145,1023,0,7);
    Serial.print(hodnotafrekvence[i]);//debugovani
    Serial.print(" ");
  }
  Serial.println(" ");
  
  for(int i=0;i<=6;i++)
  {
    Serial.print(frekvenceZBoku[i]);
    Serial.print(" ");
  }
  Serial.println(" ");

  for (int i = 0;i<7;i++)
  {
    hodnotafrekvence[i]=constrain(hodnotafrekvence[i],0,1023);
    frekvenceBarva[i]=map(hodnotafrekvence[i],145,1023,0,230);
    Serial.print(hodnotafrekvence[i]);//debugovani
    Serial.print(" ");
  }
  Serial.println(" ");
  
  for(int i=0;i<=6;i++)
  {
    Serial.print(frekvenceBarva[i]);
    Serial.print(" ");
  }
  Serial.println(" ");


  if(map(analogRead(A1),10,1000,255,00)<jas || map(analogRead(A1),10,1000,255,0)>jas)
  {
    jas=map(analogRead(A1),10,1000,255,0);
  }
  
  delayMicroseconds(1);

  if(map(analogRead(A2),0,1000,255,0)<barva || map(analogRead(A2),0,1000,255,0>barva))
  {
  barva=map(analogRead(A2),50,1000,255,0);
    if(barva<0)
      barva=0;
    if(barva>255)
      barva=255;
  }
    
  delayMicroseconds(1);
  
  if(map(analogRead(A3),0,1000,10,0)<barevnyIndex || map(analogRead(A3),0,1000,10,0)>barevnyIndex)
  {
    barevnyIndex=map(analogRead(A3),20,1000,10,0);
    if(barevnyIndex<0)
      barevnyIndex=0;
    if(barevnyIndex>255)
      barevnyIndex=255;
  }

  delayMicroseconds(1);
  
  if(map(analogRead(A3),0,1000,16,0)<barevnyIndex1 || map(analogRead(A3),0,1000,16,0)>barevnyIndex1)
  {
    barevnyIndex1=map(analogRead(A3),20,1000,16,0);
    if(barevnyIndex1<0)
      barevnyIndex1=0;
    if(barevnyIndex1>255)
      barevnyIndex1=255;
  }
  
  delayMicroseconds(1);
  
  if(map(analogRead(A4),0,1000,250,0)<padani || map(analogRead(A4),0,1000,250,0)>padani)
  {
    padani=map(analogRead(A4),20,1000,250,0);
    if(padani<0)
      padani=0;
    if(padani>250)
      padani=250;
  }
  
  delayMicroseconds(1);
  
  if(map(analogRead(A5),0,1000,255,0)<barvaTecky || map(analogRead(A5),0,900,255,0)>barvaTecky)
  {
    barvaTecky=map(analogRead(A5),20,1000,255,0);
    if(barvaTecky<0)
      barvaTecky=0;
    if(barvaTecky>255)
      barvaTecky=255;
  }
  
  delayMicroseconds(1);

  
  Serial.print("Jas: ");
  Serial.println(jas);
  Serial.print("BarvenyIndex: ");
  Serial.println(barevnyIndex);
  Serial.print("Padani: ");
  Serial.println(padani);
  Serial.print("barva: ");
  Serial.println(barva);
  Serial.print("volba: ");
  Serial.println(volba);
  Serial.print("barvaTecky: ");
  Serial.println(barvaTecky);
  
  lcd1();
  
  switch(volba)
  {
  case 0:
  {

    animace1();
  break;
  }
  case 1:
  {
    basy01();
    basy02();
    basy03();
    stredy01();
    stredy02();
    vysky01();
    vysky02();
   break;
   }
   case 2:
   {
     basy11();
     basy12();
     basy13();
     stredy11();
     stredy12();
     vysky11();
     vysky12();
   break;
   }
   case 3:
   {
    basy21();
    basy22();
    basy23();
    stredy21();
    stredy22();
    vysky21();
    vysky22();
   break;
   case 4:
   {
    basybasic1();
    basybasic2();
    basybasic3();
    stredybasic1();
    stredybasic2();
    vyskybasic1();
    vyskybasic2();
   break;
   }
   case 5:
   {
    basybasic11();
    basybasic12();
    basybasic13();
    stredybasic11();
    stredybasic12();
    vyskybasic11();
    vyskybasic12();
   break;
   }
   case 6:
   {
    ledky[0]=CHSV(0,0,0);
    ledky[30]=CHSV(0,0,0);
    ledky[60]=CHSV(0,0,0);
    ledky[90]=CHSV(0,0,0);
    ledky[120]=CHSV(0,0,0);
    ledky[150]=CHSV(0,0,0);
    ledky[180]=CHSV(0,0,0);
    zBoku1();
    zBoku2();
    zBoku3();
    zBoku4();
    zBoku5();
    zBoku6();
    zBoku7();
   break;
   }
   
   default:
      lcd.clear();
      lcd.print("Spatne cislo");
   break;
  }
 }
}

void tlacitko1()
{
  volba++;
  if(volba>6)
    volba=6;
}

void tlacitko2()
{
  volba--;
  if(volba<0)
    volba=0;
}

void animace1()
{
  startIndex = startIndex + 8;
  int barevnyIndex=startIndex;
  for(int i = 0; i<pocetLED; i++) 
  {
  ledky[i] = ColorFromPalette(RainbowColors_p, barevnyIndex, 175);
  barevnyIndex+=6;
  }
  FastLED.show();
}


void zBoku1()
{
  switch(frekvenceZBoku[0])
  {
  case 1:
    {
    for(int i=1;i<=4;i++)
      ledky[i]=CHSV(frekvenceBarva[0],255,jas);
    for(int i=31;i<=34;i++)
      ledky[i]=CHSV(0,0,0);
    for(int i=61;i<=64;i++)
     ledky[i]=CHSV(0,0,0);
    for(int i=91;i<=94;i++)
      ledky[i]=CHSV(0,0,0);
    for(int i=121;i<=124;i++)
      ledky[i]=CHSV(0,0,0);
    for(int i=151;i<=154;i++)
      ledky[i]=CHSV(0,0,0);
    for(int i=181;i<=184;i++)
      ledky[i]=CHSV(0,0,0);
    break;
    }
  case 2:
    {
      for(int i=1;i<=4;i++)
        ledky[i]=CHSV(frekvenceBarva[0],255,jas);
      for(int i=31;i<=34;i++)
        ledky[i]=CHSV(frekvenceBarva[0],255,jas);
      for(int i=61;i<=64;i++)
       ledky[i]=CHSV(0,0,0);
      for(int i=91;i<=94;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=121;i<=124;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=151;i<=154;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=181;i<=184;i++)
        ledky[i]=CHSV(0,0,0);
      break;
    }
  case 3:
    {
      for(int i=1;i<=4;i++)
        ledky[i]=CHSV(frekvenceBarva[0],255,jas);
      for(int i=31;i<=34;i++)
        ledky[i]=CHSV(frekvenceBarva[0],255,jas);
      for(int i=61;i<=64;i++)
        ledky[i]=CHSV(frekvenceBarva[0],255,jas);
      for(int i=91;i<=94;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=121;i<=124;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=151;i<=154;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=181;i<=184;i++)
        ledky[i]=CHSV(0,0,0);
      break;
    }
  case 4:
    {
      for(int i=1;i<=4;i++)
        ledky[i]=CHSV(frekvenceBarva[0],255,jas);
      for(int i=31;i<=34;i++)
        ledky[i]=CHSV(frekvenceBarva[0],255,jas);
      for(int i=61;i<=64;i++)
        ledky[i]=CHSV(frekvenceBarva[0],255,jas);
      for(int i=91;i<=94;i++)
        ledky[i]=CHSV(frekvenceBarva[0],255,jas);
      for(int i=121;i<=124;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=151;i<=154;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=181;i<=184;i++)
        ledky[i]=CHSV(0,0,0);
      break;
    }
  case 5:
    {
      for(int i=1;i<=4;i++)
        ledky[i]=CHSV(frekvenceBarva[0],255,jas);
      for(int i=31;i<=34;i++)
        ledky[i]=CHSV(frekvenceBarva[0],255,jas);
      for(int i=61;i<=64;i++)
        ledky[i]=CHSV(frekvenceBarva[0],255,jas);
      for(int i=91;i<=94;i++)
        ledky[i]=CHSV(frekvenceBarva[0],255,jas);
      for(int i=121;i<=124;i++)
        ledky[i]=CHSV(frekvenceBarva[0],255,jas);
      for(int i=151;i<=154;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=181;i<=184;i++)
        ledky[i]=CHSV(0,0,0);
      break;
    }
  case 6:
    {
      for(int i=1;i<=4;i++)
        ledky[i]=CHSV(frekvenceBarva[0],255,jas);
      for(int i=31;i<=34;i++)
        ledky[i]=CHSV(frekvenceBarva[0],255,jas);
      for(int i=61;i<=64;i++)
        ledky[i]=CHSV(frekvenceBarva[0],255,jas);
      for(int i=91;i<=94;i++)
        ledky[i]=CHSV(frekvenceBarva[0],255,jas);
      for(int i=121;i<=124;i++)
        ledky[i]=CHSV(frekvenceBarva[0],255,jas);
      for(int i=151;i<=154;i++)
        ledky[i]=CHSV(frekvenceBarva[0],255,jas);
      for(int i=181;i<=184;i++)
        ledky[i]=CHSV(0,0,0);
      break;
    }
  case 7:
    {
      for(int i=1;i<=4;i++)
        ledky[i]=CHSV(frekvenceBarva[0],255,jas);
      for(int i=31;i<=34;i++)
        ledky[i]=CHSV(frekvenceBarva[0],255,jas);
      for(int i=61;i<=64;i++)
        ledky[i]=CHSV(frekvenceBarva[0],255,jas);
      for(int i=91;i<=94;i++)
        ledky[i]=CHSV(frekvenceBarva[0],255,jas);
      for(int i=121;i<=124;i++)
        ledky[i]=CHSV(frekvenceBarva[0],255,jas);
      for(int i=151;i<=154;i++)
        ledky[i]=CHSV(frekvenceBarva[0],255,jas);
      for(int i=181;i<=184;i++)
        ledky[i]=CHSV(frekvenceBarva[0],255,jas);
      break;
    }
 }
 FastLED.show();
}

void zBoku2()
{
  switch(frekvenceZBoku[1])
  {
  case 1:
    {
    for(int i=5;i<=8;i++)
      ledky[i]=CHSV(frekvenceBarva[1],255,jas);
    for(int i=35;i<=38;i++)
      ledky[i]=CHSV(0,0,0);
    for(int i=65;i<=68;i++)
     ledky[i]=CHSV(0,0,0);
    for(int i=95;i<=98;i++)
      ledky[i]=CHSV(0,0,0);
    for(int i=125;i<=128;i++)
      ledky[i]=CHSV(0,0,0);
    for(int i=155;i<=158;i++)
      ledky[i]=CHSV(0,0,0);
    for(int i=185;i<=188;i++)
      ledky[i]=CHSV(0,0,0);
    break;
    }
  case 2:
    {
      for(int i=5;i<=8;i++)
        ledky[i]=CHSV(frekvenceBarva[1],255,jas);
      for(int i=35;i<=38;i++)
        ledky[i]=CHSV(frekvenceBarva[1],255,jas);
      for(int i=65;i<=68;i++)
       ledky[i]=CHSV(0,0,0);
      for(int i=95;i<=98;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=125;i<=128;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=155;i<=158;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=185;i<=188;i++)
        ledky[i]=CHSV(0,0,0);
      break;
    }
  case 3:
    {
      for(int i=5;i<=8;i++)
        ledky[i]=CHSV(frekvenceBarva[1],255,jas);
      for(int i=35;i<=38;i++)
        ledky[i]=CHSV(frekvenceBarva[1],255,jas);
      for(int i=65;i<=68;i++)
        ledky[i]=CHSV(frekvenceBarva[1],255,jas);
      for(int i=95;i<=98;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=125;i<=128;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=155;i<=158;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=185;i<=188;i++)
        ledky[i]=CHSV(0,0,0);
      break;
    }
  case 4:
    {
      for(int i=5;i<=8;i++)
        ledky[i]=CHSV(frekvenceBarva[1],255,jas);
      for(int i=35;i<=38;i++)
        ledky[i]=CHSV(frekvenceBarva[1],255,jas);
      for(int i=65;i<=68;i++)
       ledky[i]=CHSV(frekvenceBarva[1],255,jas);
      for(int i=95;i<=98;i++)
        ledky[i]=CHSV(frekvenceBarva[1],255,jas);
      for(int i=125;i<=128;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=155;i<=158;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=185;i<=188;i++)
        ledky[i]=CHSV(0,0,0);
      break;
    }
  case 5:
    {
      for(int i=5;i<=8;i++)
        ledky[i]=CHSV(frekvenceBarva[1],255,jas);
      for(int i=35;i<=38;i++)
        ledky[i]=CHSV(frekvenceBarva[1],255,jas);
      for(int i=65;i<=68;i++)
        ledky[i]=CHSV(frekvenceBarva[1],255,jas);
      for(int i=95;i<=98;i++)
        ledky[i]=CHSV(frekvenceBarva[1],255,jas);
      for(int i=125;i<=128;i++)
        ledky[i]=CHSV(frekvenceBarva[1],255,jas);
      for(int i=155;i<=158;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=185;i<=188;i++)
        ledky[i]=CHSV(0,0,0);
      break;
    }
  case 6:
    {
      for(int i=5;i<=8;i++)
        ledky[i]=CHSV(frekvenceBarva[1],255,jas);
       for(int i=35;i<=38;i++)
        ledky[i]=CHSV(frekvenceBarva[1],255,jas);
      for(int i=65;i<=68;i++)
        ledky[i]=CHSV(frekvenceBarva[1],255,jas);
      for(int i=95;i<=98;i++)
        ledky[i]=CHSV(frekvenceBarva[1],255,jas);
      for(int i=125;i<=128;i++)
        ledky[i]=CHSV(frekvenceBarva[1],255,jas);
      for(int i=155;i<=158;i++)
        ledky[i]=CHSV(frekvenceBarva[1],255,jas);
      for(int i=185;i<=188;i++)
        ledky[i]=CHSV(0,0,0);
      break;
    }
  case 7:
    {
      for(int i=5;i<=8;i++)
        ledky[i]=CHSV(frekvenceBarva[1],255,jas);
      for(int i=35;i<=38;i++)
        ledky[i]=CHSV(frekvenceBarva[1],255,jas);
      for(int i=65;i<=68;i++)
        ledky[i]=CHSV(frekvenceBarva[1],255,jas);
      for(int i=95;i<=98;i++)
        ledky[i]=CHSV(frekvenceBarva[1],255,jas);
      for(int i=125;i<=128;i++)
        ledky[i]=CHSV(frekvenceBarva[1],255,jas);
      for(int i=155;i<=158;i++)
        ledky[i]=CHSV(frekvenceBarva[1],255,jas);
      for(int i=185;i<=188;i++)
        ledky[i]=CHSV(frekvenceBarva[1],255,jas);
      break;
    }
 }
 FastLED.show();
}

void zBoku3()
{
  switch(frekvenceZBoku[2])
  {
  case 1:
    {
    for(int i=9;i<=12;i++)
      ledky[i]=CHSV(frekvenceBarva[2],255,jas);
    for(int i=39;i<=42;i++)
      ledky[i]=CHSV(0,0,0);
    for(int i=69;i<=72;i++)
     ledky[i]=CHSV(0,0,0);
    for(int i=99;i<=102;i++)
      ledky[i]=CHSV(0,0,0);
    for(int i=129;i<=132;i++)
      ledky[i]=CHSV(0,0,0);
    for(int i=159;i<=162;i++)
      ledky[i]=CHSV(0,0,0);
    for(int i=189;i<=192;i++)
      ledky[i]=CHSV(0,0,0);
    break;
    }
  case 2:
    {
      for(int i=9;i<=12;i++)
        ledky[i]=CHSV(frekvenceBarva[2],255,jas);
      for(int i=39;i<=42;i++)
        ledky[i]=CHSV(frekvenceBarva[2],255,jas);
      for(int i=69;i<=72;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=99;i<=102;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=129;i<=132;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=159;i<=162;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=189;i<=192;i++)
        ledky[i]=CHSV(0,0,0);
      break;
    }
  case 3:
    {
      for(int i=9;i<=12;i++)
        ledky[i]=CHSV(frekvenceBarva[2],255,jas);
      for(int i=39;i<=42;i++)
        ledky[i]=CHSV(frekvenceBarva[2],255,jas);
      for(int i=69;i<=72;i++)
        ledky[i]=CHSV(frekvenceBarva[2],255,jas);
      for(int i=99;i<=102;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=129;i<=132;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=159;i<=162;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=189;i<=192;i++)
        ledky[i]=CHSV(0,0,0);
      break;
    }
  case 4:
    {
      for(int i=9;i<=12;i++)
        ledky[i]=CHSV(frekvenceBarva[2],255,jas);
      for(int i=39;i<=42;i++)
        ledky[i]=CHSV(frekvenceBarva[2],255,jas);
      for(int i=69;i<=72;i++)
        ledky[i]=CHSV(frekvenceBarva[2],255,jas);
      for(int i=99;i<=102;i++)
        ledky[i]=CHSV(frekvenceBarva[2],255,jas);
      for(int i=129;i<=132;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=159;i<=162;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=189;i<=192;i++)
        ledky[i]=CHSV(0,0,0);
      break;
    }
  case 5:
    {
      for(int i=9;i<=12;i++)
        ledky[i]=CHSV(frekvenceBarva[2],255,jas);
      for(int i=39;i<=42;i++)
        ledky[i]=CHSV(frekvenceBarva[2],255,jas);
      for(int i=69;i<=72;i++)
        ledky[i]=CHSV(frekvenceBarva[2],255,jas);
      for(int i=99;i<=102;i++)
        ledky[i]=CHSV(frekvenceBarva[2],255,jas);
      for(int i=129;i<=132;i++)
        ledky[i]=CHSV(frekvenceBarva[2],255,jas);
      for(int i=159;i<=162;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=189;i<=192;i++)
        ledky[i]=CHSV(0,0,0);
     break;
    }
  case 6:
    {
      for(int i=9;i<=12;i++)
        ledky[i]=CHSV(frekvenceBarva[2],255,jas);
      for(int i=39;i<=42;i++)
        ledky[i]=CHSV(frekvenceBarva[2],255,jas);
      for(int i=69;i<=72;i++)
        ledky[i]=CHSV(frekvenceBarva[2],255,jas);
      for(int i=99;i<=102;i++)
        ledky[i]=CHSV(frekvenceBarva[2],255,jas);
      for(int i=129;i<=132;i++)
        ledky[i]=CHSV(frekvenceBarva[2],255,jas);
      for(int i=159;i<=162;i++)
        ledky[i]=CHSV(frekvenceBarva[2],255,jas);
      for(int i=189;i<=192;i++)
        ledky[i]=CHSV(0,0,0);
      break;
    }
  case 7:
    {
     for(int i=9;i<=12;i++)
        ledky[i]=CHSV(frekvenceBarva[2],255,jas);
      for(int i=39;i<=42;i++)
        ledky[i]=CHSV(frekvenceBarva[2],255,jas);
      for(int i=69;i<=72;i++)
        ledky[i]=CHSV(frekvenceBarva[2],255,jas);
      for(int i=99;i<=102;i++)
        ledky[i]=CHSV(frekvenceBarva[2],255,jas);
      for(int i=129;i<=132;i++)
        ledky[i]=CHSV(frekvenceBarva[2],255,jas);
      for(int i=159;i<=162;i++)
        ledky[i]=CHSV(frekvenceBarva[2],255,jas);
      for(int i=189;i<=192;i++)
        ledky[i]=CHSV(frekvenceBarva[2],255,jas);
      break;
    }
 }
 FastLED.show();
}

void zBoku4()
{
  switch(frekvenceZBoku[3])
  {
  case 1:
    {
     for(int i=13;i<=16;i++)
       ledky[i]=CHSV(frekvenceBarva[3],255,jas);
     for(int i=43;i<=46;i++)
       ledky[i]=CHSV(0,0,0);
     for(int i=73;i<=76;i++)
       ledky[i]=CHSV(0,0,0);
     for(int i=103;i<=106;i++)
       ledky[i]=CHSV(0,0,0);
     for(int i=133;i<=136;i++)
       ledky[i]=CHSV(0,0,0);
     for(int i=163;i<=166;i++)
       ledky[i]=CHSV(0,0,0);
     for(int i=193;i<=196;i++)
       ledky[i]=CHSV(0,0,0);
    break;
    }
  case 2:
    {
     for(int i=13;i<=16;i++)
       ledky[i]=CHSV(frekvenceBarva[3],255,jas);
     for(int i=43;i<=46;i++)
       ledky[i]=CHSV(frekvenceBarva[3],255,jas);
     for(int i=73;i<=76;i++)
       ledky[i]=CHSV(0,0,0);
     for(int i=103;i<=106;i++)
       ledky[i]=CHSV(0,0,0);
     for(int i=133;i<=136;i++)
       ledky[i]=CHSV(0,0,0);
     for(int i=163;i<=166;i++)
       ledky[i]=CHSV(0,0,0);
     for(int i=193;i<=196;i++)
       ledky[i]=CHSV(0,0,0);
      break;
    }
  case 3:
    {
     for(int i=13;i<=16;i++)
       ledky[i]=CHSV(frekvenceBarva[3],255,jas);
     for(int i=43;i<=46;i++)
       ledky[i]=CHSV(frekvenceBarva[3],255,jas);
     for(int i=73;i<=76;i++)
       ledky[i]=CHSV(frekvenceBarva[3],255,jas);
     for(int i=103;i<=106;i++)
       ledky[i]=CHSV(0,0,0);
     for(int i=133;i<=136;i++)
       ledky[i]=CHSV(0,0,0);
     for(int i=163;i<=166;i++)
       ledky[i]=CHSV(0,0,0);
     for(int i=193;i<=196;i++)
       ledky[i]=CHSV(0,0,0);
      break;
    }
  case 4:
    {
     for(int i=13;i<=16;i++)
       ledky[i]=CHSV(frekvenceBarva[3],255,jas);
     for(int i=43;i<=46;i++)
       ledky[i]=CHSV(frekvenceBarva[3],255,jas);
     for(int i=73;i<=76;i++)
       ledky[i]=CHSV(frekvenceBarva[3],255,jas);
     for(int i=103;i<=106;i++)
       ledky[i]=CHSV(frekvenceBarva[3],255,jas);
     for(int i=133;i<=136;i++)
       ledky[i]=CHSV(0,0,0);
     for(int i=163;i<=166;i++)
       ledky[i]=CHSV(0,0,0);
     for(int i=193;i<=196;i++)
       ledky[i]=CHSV(0,0,0);
      break;
    }
  case 5:
    {
     for(int i=13;i<=16;i++)
       ledky[i]=CHSV(frekvenceBarva[3],255,jas);
     for(int i=43;i<=46;i++)
       ledky[i]=CHSV(frekvenceBarva[3],255,jas);
     for(int i=73;i<=76;i++)
       ledky[i]=CHSV(frekvenceBarva[3],255,jas);
     for(int i=103;i<=106;i++)
       ledky[i]=CHSV(frekvenceBarva[3],255,jas);
     for(int i=133;i<=136;i++)
       ledky[i]=CHSV(frekvenceBarva[3],255,jas);
     for(int i=163;i<=166;i++)
       ledky[i]=CHSV(0,0,0);
     for(int i=193;i<=196;i++)
       ledky[i]=CHSV(0,0,0);
     break;
    }
  case 6:
    {
     for(int i=13;i<=16;i++)
       ledky[i]=CHSV(frekvenceBarva[3],255,jas);
     for(int i=43;i<=46;i++)
       ledky[i]=CHSV(frekvenceBarva[3],255,jas);
     for(int i=73;i<=76;i++)
       ledky[i]=CHSV(frekvenceBarva[3],255,jas);
     for(int i=103;i<=106;i++)
       ledky[i]=CHSV(frekvenceBarva[3],255,jas);
     for(int i=133;i<=136;i++)
       ledky[i]=CHSV(frekvenceBarva[3],255,jas);
     for(int i=163;i<=166;i++)
       ledky[i]=CHSV(frekvenceBarva[3],255,jas);
     for(int i=193;i<=196;i++)
       ledky[i]=CHSV(0,0,0);
      break;
    }
  case 7:
    {
     for(int i=13;i<=16;i++)
       ledky[i]=CHSV(frekvenceBarva[3],255,jas);
     for(int i=43;i<=46;i++)
       ledky[i]=CHSV(frekvenceBarva[3],255,jas);
     for(int i=73;i<=76;i++)
       ledky[i]=CHSV(frekvenceBarva[3],255,jas);
     for(int i=103;i<=106;i++)
       ledky[i]=CHSV(frekvenceBarva[3],255,jas);
     for(int i=133;i<=136;i++)
       ledky[i]=CHSV(frekvenceBarva[3],255,jas);
     for(int i=163;i<=166;i++)
       ledky[i]=CHSV(frekvenceBarva[3],255,jas);
     for(int i=193;i<=196;i++)
       ledky[i]=CHSV(frekvenceBarva[3],255,jas);
     break;
    }
 }
 FastLED.show();
}

void zBoku5()
{
  switch(frekvenceZBoku[4])
  {
  case 1:
    {
      for(int i=17;i<=20;i++)
        ledky[i]=CHSV(frekvenceBarva[4],255,jas);
      for(int i=47;i<=50;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=77;i<=80;i++)
       ledky[i]=CHSV(0,0,0);
      for(int i=107;i<=110;i++)
       ledky[i]=CHSV(0,0,0);
      for(int i=137;i<=140;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=167;i<=170;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=197;i<=200;i++)
        ledky[i]=CHSV(0,0,0);
    break;
    }
  case 2:
    {
      for(int i=17;i<=20;i++)
        ledky[i]=CHSV(frekvenceBarva[4],255,jas);
      for(int i=47;i<=50;i++)
        ledky[i]=CHSV(frekvenceBarva[4],255,jas);
      for(int i=77;i<=80;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=107;i<=110;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=137;i<=140;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=167;i<=170;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=197;i<=200;i++)
        ledky[i]=CHSV(0,0,0);
      break;
    }
  case 3:
    {
      for(int i=17;i<=20;i++)
        ledky[i]=CHSV(frekvenceBarva[4],255,jas);
      for(int i=47;i<=50;i++)
        ledky[i]=CHSV(frekvenceBarva[4],255,jas);
      for(int i=77;i<=80;i++)
        ledky[i]=CHSV(frekvenceBarva[4],255,jas);
      for(int i=107;i<=110;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=137;i<=140;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=167;i<=170;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=197;i<=200;i++)
        ledky[i]=CHSV(0,0,0);
      break;
    }
  case 4:
    {
      for(int i=17;i<=20;i++)
        ledky[i]=CHSV(frekvenceBarva[4],255,jas);
      for(int i=47;i<=50;i++)
        ledky[i]=CHSV(frekvenceBarva[4],255,jas);
      for(int i=77;i<=80;i++)
        ledky[i]=CHSV(frekvenceBarva[4],255,jas);
      for(int i=107;i<=110;i++)
        ledky[i]=CHSV(frekvenceBarva[4],255,jas);
      for(int i=137;i<=140;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=167;i<=170;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=197;i<=200;i++)
        ledky[i]=CHSV(0,0,0);
      break;
    }
  case 5:
    {
      for(int i=17;i<=20;i++)
        ledky[i]=CHSV(frekvenceBarva[4],255,jas);
      for(int i=47;i<=50;i++)
        ledky[i]=CHSV(frekvenceBarva[4],255,jas);
      for(int i=77;i<=80;i++)
       ledky[i]=CHSV(frekvenceBarva[4],255,jas);
      for(int i=107;i<=110;i++)
       ledky[i]=CHSV(frekvenceBarva[4],255,jas);
      for(int i=137;i<=140;i++)
        ledky[i]=CHSV(frekvenceBarva[4],255,jas);
      for(int i=167;i<=170;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=197;i<=200;i++)
        ledky[i]=CHSV(0,0,0);
     break;
    }
  case 6:
    {
      for(int i=17;i<=20;i++)
        ledky[i]=CHSV(frekvenceBarva[4],255,jas);
      for(int i=47;i<=50;i++)
        ledky[i]=CHSV(frekvenceBarva[4],255,jas);
      for(int i=77;i<=80;i++)
       ledky[i]=CHSV(frekvenceBarva[4],255,jas);
      for(int i=107;i<=110;i++)
       ledky[i]=CHSV(frekvenceBarva[4],255,jas);
      for(int i=137;i<=140;i++)
        ledky[i]=CHSV(frekvenceBarva[4],255,jas);
      for(int i=167;i<=170;i++)
        ledky[i]=CHSV(frekvenceBarva[4],255,jas);
      for(int i=197;i<=200;i++)
        ledky[i]=CHSV(0,0,0);
      break;
    }
  case 7:
    {
      for(int i=17;i<=20;i++)
        ledky[i]=CHSV(frekvenceBarva[4],255,jas);
      for(int i=47;i<=50;i++)
        ledky[i]=CHSV(frekvenceBarva[4],255,jas);
      for(int i=77;i<=80;i++)
       ledky[i]=CHSV(frekvenceBarva[4],255,jas);
      for(int i=107;i<=110;i++)
       ledky[i]=CHSV(frekvenceBarva[4],255,jas);
      for(int i=137;i<=140;i++)
        ledky[i]=CHSV(frekvenceBarva[4],255,jas);
      for(int i=167;i<=170;i++)
        ledky[i]=CHSV(frekvenceBarva[4],255,jas);
      for(int i=197;i<=200;i++)
        ledky[i]=CHSV(frekvenceBarva[4],255,jas);
     break;
    }
 }
 FastLED.show();
}

void zBoku6()
{
  switch(frekvenceZBoku[5])
  {
  case 1:
    {
      for(int i=21;i<=24;i++)
        ledky[i]=CHSV(frekvenceBarva[5],255,jas);
      for(int i=51;i<=54;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=81;i<=84;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=111;i<=114;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=141;i<=144;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=171;i<=174;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=201;i<=204;i++)
        ledky[i]=CHSV(0,0,0);
    break;
    }
  case 2:
    {
      for(int i=21;i<=24;i++)
        ledky[i]=CHSV(frekvenceBarva[5],255,jas);
      for(int i=51;i<=54;i++)
        ledky[i]=CHSV(frekvenceBarva[5],255,jas);
      for(int i=81;i<=84;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=111;i<=114;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=141;i<=144;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=171;i<=174;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=201;i<=204;i++)
        ledky[i]=CHSV(0,0,0);
      break;
    }
  case 3:
    {
      for(int i=21;i<=24;i++)
        ledky[i]=CHSV(frekvenceBarva[5],255,jas);
      for(int i=51;i<=54;i++)
        ledky[i]=CHSV(frekvenceBarva[5],255,jas);
      for(int i=81;i<=84;i++)
        ledky[i]=CHSV(frekvenceBarva[5],255,jas);
      for(int i=111;i<=114;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=141;i<=144;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=171;i<=174;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=201;i<=204;i++)
        ledky[i]=CHSV(0,0,0);
      break;
    }
  case 4:
    {
      for(int i=21;i<=24;i++)
        ledky[i]=CHSV(frekvenceBarva[5],255,jas);
      for(int i=51;i<=54;i++)
        ledky[i]=CHSV(frekvenceBarva[5],255,jas);
      for(int i=81;i<=84;i++)
        ledky[i]=CHSV(frekvenceBarva[5],255,jas);
      for(int i=111;i<=114;i++)
        ledky[i]=CHSV(frekvenceBarva[5],255,jas);
      for(int i=141;i<=144;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=171;i<=174;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=201;i<=204;i++)
        ledky[i]=CHSV(0,0,0);
      break;
    }
  case 5:
    {
      for(int i=21;i<=24;i++)
        ledky[i]=CHSV(frekvenceBarva[5],255,jas);
      for(int i=51;i<=54;i++)
        ledky[i]=CHSV(frekvenceBarva[5],255,jas);
      for(int i=81;i<=84;i++)
        ledky[i]=CHSV(frekvenceBarva[5],255,jas);
      for(int i=111;i<=114;i++)
        ledky[i]=CHSV(frekvenceBarva[5],255,jas);
      for(int i=141;i<=144;i++)
        ledky[i]=CHSV(frekvenceBarva[5],255,jas);
      for(int i=171;i<=174;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=201;i<=204;i++)
        ledky[i]=CHSV(0,0,0);
     break;
    }
  case 6:
    {
      for(int i=21;i<=24;i++)
        ledky[i]=CHSV(frekvenceBarva[5],255,jas);
      for(int i=51;i<=54;i++)
        ledky[i]=CHSV(frekvenceBarva[5],255,jas);
      for(int i=81;i<=84;i++)
        ledky[i]=CHSV(frekvenceBarva[5],255,jas);
      for(int i=111;i<=114;i++)
        ledky[i]=CHSV(frekvenceBarva[5],255,jas);
      for(int i=141;i<=144;i++)
        ledky[i]=CHSV(frekvenceBarva[5],255,jas);
      for(int i=171;i<=174;i++)
        ledky[i]=CHSV(frekvenceBarva[5],255,jas);
      for(int i=201;i<=204;i++)
        ledky[i]=CHSV(0,0,0);
      break;
    }
  case 7:
    {
      for(int i=21;i<=24;i++)
        ledky[i]=CHSV(frekvenceBarva[5],255,jas);
      for(int i=51;i<=54;i++)
        ledky[i]=CHSV(frekvenceBarva[5],255,jas);
      for(int i=81;i<=84;i++)
        ledky[i]=CHSV(frekvenceBarva[5],255,jas);
      for(int i=111;i<=114;i++)
        ledky[i]=CHSV(frekvenceBarva[5],255,jas);
      for(int i=141;i<=144;i++)
        ledky[i]=CHSV(frekvenceBarva[5],255,jas);
      for(int i=171;i<=174;i++)
        ledky[i]=CHSV(frekvenceBarva[5],255,jas);
      for(int i=201;i<=204;i++)
        ledky[i]=CHSV(frekvenceBarva[5],255,jas);
     break;
    }
 }
 FastLED.show();
}

void zBoku7()
{
  switch(frekvenceZBoku[6])
  {
  case 1:
    {
      for(int i=25;i<=28;i++)
        ledky[i]=CHSV(frekvenceBarva[6],255,jas);
      for(int i=55;i<=58;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=85;i<=88;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=115;i<=118;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=145;i<=148;i++)
         ledky[i]=CHSV(0,0,0);
      for(int i=175;i<=178;i++)
         ledky[i]=CHSV(0,0,0);
       for(int i=205;i<=208;i++)
         ledky[i]=CHSV(0,0,0);
    break;
    }
  case 2:
    {
      for(int i=25;i<=28;i++)
        ledky[i]=CHSV(frekvenceBarva[6],255,jas);
      for(int i=55;i<=58;i++)
        ledky[i]=CHSV(frekvenceBarva[6],255,jas);
      for(int i=85;i<=88;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=115;i<=118;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=145;i<=148;i++)
         ledky[i]=CHSV(0,0,0);
      for(int i=175;i<=178;i++)
         ledky[i]=CHSV(0,0,0);
       for(int i=205;i<=208;i++)
         ledky[i]=CHSV(0,0,0);
      break;
    }
  case 3:
    {
      for(int i=25;i<=28;i++)
        ledky[i]=CHSV(frekvenceBarva[6],255,jas);
      for(int i=55;i<=58;i++)
        ledky[i]=CHSV(frekvenceBarva[6],255,jas);
      for(int i=85;i<=88;i++)
        ledky[i]=CHSV(frekvenceBarva[6],255,jas);
      for(int i=115;i<=118;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=145;i<=148;i++)
         ledky[i]=CHSV(0,0,0);
      for(int i=175;i<=178;i++)
         ledky[i]=CHSV(0,0,0);
       for(int i=205;i<=208;i++)
         ledky[i]=CHSV(0,0,0);
      break;
    }
  case 4:
    {
      for(int i=25;i<=28;i++)
        ledky[i]=CHSV(frekvenceBarva[6],255,jas);
      for(int i=55;i<=58;i++)
        ledky[i]=CHSV(frekvenceBarva[6],255,jas);
      for(int i=85;i<=88;i++)
        ledky[i]=CHSV(frekvenceBarva[6],255,jas);
      for(int i=115;i<=118;i++)
        ledky[i]=CHSV(frekvenceBarva[6],255,jas);
      for(int i=145;i<=148;i++)
         ledky[i]=CHSV(0,0,0);
      for(int i=175;i<=178;i++)
         ledky[i]=CHSV(0,0,0);
       for(int i=205;i<=208;i++)
         ledky[i]=CHSV(0,0,0);
      break;
    }
  case 5:
    {
      for(int i=25;i<=28;i++)
        ledky[i]=CHSV(frekvenceBarva[6],255,jas);
      for(int i=55;i<=58;i++)
        ledky[i]=CHSV(frekvenceBarva[6],255,jas);
      for(int i=85;i<=88;i++)
        ledky[i]=CHSV(frekvenceBarva[6],255,jas);
      for(int i=115;i<=118;i++)
        ledky[i]=CHSV(frekvenceBarva[6],255,jas);
      for(int i=145;i<=148;i++)
         ledky[i]=CHSV(frekvenceBarva[6],255,jas);
      for(int i=175;i<=178;i++)
         ledky[i]=CHSV(0,0,0);
       for(int i=205;i<=208;i++)
         ledky[i]=CHSV(0,0,0);
     break;
    }
  case 6:
    {
      for(int i=25;i<=28;i++)
        ledky[i]=CHSV(frekvenceBarva[6],255,jas);
      for(int i=55;i<=58;i++)
        ledky[i]=CHSV(frekvenceBarva[6],255,jas);
      for(int i=85;i<=88;i++)
        ledky[i]=CHSV(frekvenceBarva[6],255,jas);
      for(int i=115;i<=118;i++)
        ledky[i]=CHSV(frekvenceBarva[6],255,jas);
      for(int i=145;i<=148;i++)
         ledky[i]=CHSV(frekvenceBarva[6],255,jas);
      for(int i=175;i<=178;i++)
         ledky[i]=CHSV(frekvenceBarva[6],255,jas);
       for(int i=205;i<=208;i++)
         ledky[i]=CHSV(0,0,0);
      break;
    }

  case 7:
    {
      for(int i=25;i<=28;i++)
        ledky[i]=CHSV(frekvenceBarva[6],255,jas);
      for(int i=55;i<=58;i++)
        ledky[i]=CHSV(frekvenceBarva[6],255,jas);
      for(int i=85;i<=88;i++)
        ledky[i]=CHSV(frekvenceBarva[6],255,jas);
      for(int i=115;i<=118;i++)
        ledky[i]=CHSV(frekvenceBarva[6],255,jas);
      for(int i=145;i<=148;i++)
         ledky[i]=CHSV(frekvenceBarva[6],255,jas);
      for(int i=175;i<=178;i++)
         ledky[i]=CHSV(frekvenceBarva[6],255,jas);
       for(int i=205;i<=208;i++)
         ledky[i]=CHSV(frekvenceBarva[6],255,jas);
     break;
    }
 }
 FastLED.show();
}

void basy01()
{
  //naprogramovani ktere ledky maji svitit (vizualni zobrazeni je az na konci fce) podle ctene hodnoty frekvence)
  for(int i=0;i<=frekvence[0];i++)
    {
      //nastaveni barvy,saturace a jasu
      ledky[i] = CHSV(barva+(barevnyIndex*i),255,jas);

      // podminka, ktera mi nastavi peak na nejvyssi hodnotu pokud je vyssi nez predtim (+1 protoze by se mi prekryvaly modra a cerevena)
      if(poslednipeak1<i)
        poslednipeak1=i+1;

      aktualniMillis1=millis();
      //podminka, ktera dela padani tecky
      if(aktualniMillis1-predchoziMillis1>padani)
      {
        poslednipeak1--;
        predchoziMillis1=aktualniMillis1;
      }
      
      ledky[poslednipeak1]=CHSV(barvaTecky,255,jas); //nastaveni barvy tecky a stjeneho jasu jak jsou ostatni ledky

      
      //1. for - zhasina ledky mezi peakem a LEDkami, ktere jezdi podle hodnoty; 2. for - zhasina od peaku do konce pasku (+1 protoze by mi jinak zhasl i ten peak)
      for(int i=frekvence[0];i<poslednipeak1;i++)
        ledky[i] = CHSV(0,0,0);
      for(int i=poslednipeak1+1;i<=30;i++)
        ledky[i] = CHSV(0,0,0);
    }

// TOTO TEPRVE ZOBRAZI VIZUALNE BARVY    
FastLED.show();
}

void basy02()
{
  for(int i=30;i<=frekvence[1]+30;i++)
    {
      ledky[i] = CHSV(barva+barevnyIndex*(i-30),255,jas);

      if(poslednipeak2<i)
        poslednipeak2=i+1;
      
      aktualniMillis2=millis();

      if(aktualniMillis2-predchoziMillis2>padani)
      {
        poslednipeak2--;
        predchoziMillis2=aktualniMillis2;
      }
      ledky[poslednipeak2]=CHSV(barvaTecky,255,jas); 

      for(int i=frekvence[1]+30;i<poslednipeak2;i++)
        ledky[i] = CHSV(0,0,0);
      for(int i=poslednipeak2+1;i<=60;i++)
        ledky[i] = CHSV(0,0,0);
    }
FastLED.show();
}


void basy03()
{
  for(int i=60;i<=frekvence[2]+60;i++)
    {
      ledky[i] = CHSV(barva+barevnyIndex*(i-60),255,jas);

      if(poslednipeak3<i)
        poslednipeak3=i+1;

      aktualniMillis3=millis();

      if(aktualniMillis3-predchoziMillis3>padani)
      {
        poslednipeak3--;
        predchoziMillis3=aktualniMillis3;
      }
      
      ledky[poslednipeak3]=CHSV(barvaTecky,255,jas); 

      for(int i=frekvence[2]+60;i<poslednipeak3;i++)
        ledky[i] = CHSV(0,0,0);
      for(int i=poslednipeak3+1;i<=90;i++)
        ledky[i] = CHSV(0,0,0);
    }
FastLED.show();
}


void stredy01()
{
  for(int i=90;i<=frekvence[3]+90;i++)
    {
      ledky[i] = CHSV(barva+barevnyIndex*(i-90),255,jas);

      if(poslednipeak4<i)
        poslednipeak4=i+1;

      aktualniMillis4=millis();

      if(aktualniMillis4-predchoziMillis4>padani)
      {
        poslednipeak4--;
        predchoziMillis4=aktualniMillis4;
      }
      
      ledky[poslednipeak4]=CHSV(barvaTecky,255,jas); 

      for(int i=frekvence[3]+90;i<poslednipeak4;i++)
        ledky[i] = CHSV(0,0,0);
      for(int i=poslednipeak4+1;i<=120;i++)
        ledky[i] = CHSV(0,0,0);
    }
FastLED.show();
}


void stredy02()
{
  for(int i=120;i<=frekvence[4]+120;i++)
    {
      ledky[i] = CHSV(barva+barevnyIndex*(i-120),255,jas);

      if(poslednipeak5<i)
        poslednipeak5=i+1;

      aktualniMillis5=millis();

      if(aktualniMillis5-predchoziMillis5>padani)
      {
        poslednipeak5--;
        predchoziMillis5=aktualniMillis5;
      }
      
      ledky[poslednipeak5]=CHSV(barvaTecky,255,jas);

      for(int i=frekvence[4]+120;i<poslednipeak5;i++)
        ledky[i] = CHSV(0,0,0);
      for(int i=poslednipeak5+1;i<=150;i++)
        ledky[i] = CHSV(0,0,0);
    }
FastLED.show();
}

void vysky01()
{
  for(int i=150;i<=frekvence[5]+150;i++)
    {
      ledky[i] = CHSV(barva+barevnyIndex*(i-150),255,jas);

      if(poslednipeak6<i)
        poslednipeak6=i+1;

      aktualniMillis6=millis();

      if(aktualniMillis6-predchoziMillis6>padani)
      {
        poslednipeak6--;
        predchoziMillis6=aktualniMillis6;
      }
      
      ledky[poslednipeak6]=CHSV(barvaTecky,255,jas);

      for(int i=frekvence[5]+150;i<poslednipeak6;i++)
        ledky[i] = CHSV(0,0,0);
      for(int i=poslednipeak6+1;i<=180;i++)
        ledky[i] = CHSV(0,0,0);
    }
FastLED.show();
}

void vysky02()
{
  for(int i=180;i<=frekvence[6]+180;i++)
    {
      ledky[i] = CHSV(barva+barevnyIndex*(i-180),255,jas);

      if(poslednipeak7<i)
        poslednipeak7=i+1;

      aktualniMillis7=millis();

      if(aktualniMillis7-predchoziMillis7>padani)
      {
        poslednipeak7--;
        predchoziMillis7=aktualniMillis7;
      }
      
      ledky[poslednipeak7]=CHSV(barvaTecky,255,jas);

      for(int i=frekvence[6]+180;i<poslednipeak7;i++)
        ledky[i] = CHSV(0,0,0);
      for(int i=poslednipeak7+1;i<=209;i++)
        ledky[i] = CHSV(0,0,0);
    }
FastLED.show();
}


void basy11()
{
for(int i=14, j=14;i<=frekvence1[0] && j>=frekvence2[0];i++,j--)
  {
    ledky[i] = CHSV(barva+(barevnyIndex1*i),255,jas);

    if(poslednipeak011<i)
        poslednipeak011=i+1;
      
      aktualniMillis011=millis();

      if(aktualniMillis011-predchoziMillis011>padani)
      {
        poslednipeak011--;
        predchoziMillis011=aktualniMillis011;
      }
      ledky[poslednipeak011]=CHSV(barvaTecky,255,jas); 

      for(int i=frekvence1[0];i<poslednipeak011;i++)
        ledky[i] = CHSV(0,0,0);
      for(int i=poslednipeak011+1;i<=30;i++)
        ledky[i] = CHSV(0,0,0);
        
    ledky[j] = CHSV(barva+(barevnyIndex1*i),255,jas);

    if(poslednipeak012>j)
        poslednipeak012=j+1;
      
      aktualniMillis012=millis();

      if(aktualniMillis012-predchoziMillis012>padani)
      {
        poslednipeak012++;
        predchoziMillis012=aktualniMillis012;
      }
      ledky[poslednipeak012]=CHSV(barvaTecky,255,jas);

      for(int j=frekvence2[0];j>poslednipeak012;j--)
        ledky[j] = CHSV(0,0,0);
      for(int j=poslednipeak012-1;j>=0;j--)
        ledky[j] = CHSV(0,0,0);
  }
FastLED.show();
}


void basy12()
{
  for(int i=44, j=44;i<=frekvence1[1]+30 && j>=frekvence2[1]+30;i++, j--)
  {
    ledky[i] = CHSV(barva+barevnyIndex1*(i-30),255,jas);

    if(poslednipeak111<i)
        poslednipeak111=i+1;
      
      aktualniMillis111=millis();

      if(aktualniMillis111-predchoziMillis111>padani)
      {
        poslednipeak111--;
        predchoziMillis111=aktualniMillis111;
      }
      ledky[poslednipeak111]=CHSV(barvaTecky,255,jas); 

      for(int i=frekvence1[1]+30;i<poslednipeak111;i++)
        ledky[i] = CHSV(0,0,0);
      for(int i=poslednipeak111+1;i<=60;i++)
        ledky[i] = CHSV(0,0,0);

    ledky[j] = CHSV(barva+barevnyIndex1*(i-30),255,jas);

    if(poslednipeak112>j)
        poslednipeak112=j+1;
      
      aktualniMillis112=millis();

      if(aktualniMillis112-predchoziMillis112>padani)
      {
        poslednipeak112++;
        predchoziMillis112=aktualniMillis112;
      }
      ledky[poslednipeak112]=CHSV(barvaTecky,255,jas);

      for(int j=frekvence2[1]+30;j>poslednipeak112;j--)
        ledky[j] = CHSV(0,0,0);
      for(int j=poslednipeak112-1;j>=30;j--)
        ledky[j] = CHSV(0,0,0);
  }
FastLED.show();
}

void basy13()
{
  for(int i=74, j=74;i<=frekvence1[2]+60 && j>=frekvence2[2]+60;i++, j--)
  {
    ledky[i] = CHSV(barva+barevnyIndex1*(i-60),255,jas);

    if(poslednipeak211<i)
        poslednipeak211=i+1;
      
      aktualniMillis211=millis();

      if(aktualniMillis211-predchoziMillis211>padani)
      {
        poslednipeak211--;
        predchoziMillis211=aktualniMillis211;
      }
      ledky[poslednipeak211]=CHSV(barvaTecky,255,jas); 

      for(int i=frekvence1[2]+60;i<poslednipeak211;i++)
        ledky[i] = CHSV(0,0,0);
      for(int i=poslednipeak211+1;i<=90;i++)
        ledky[i] = CHSV(0,0,0);
        
    ledky[j] = CHSV(barva+barevnyIndex1*(i-60),255,jas);

    if(poslednipeak212>j)
        poslednipeak212=j+1;
      
      aktualniMillis212=millis();

      if(aktualniMillis212-predchoziMillis212>padani)
      {
        poslednipeak212++;
        predchoziMillis212=aktualniMillis212;
      }
      ledky[poslednipeak212]=CHSV(barvaTecky,255,jas);

      for(int j=frekvence2[2]+60;j>poslednipeak212;j--)
        ledky[j] = CHSV(0,0,0);
      for(int j=poslednipeak212-1;j>=60;j--)
        ledky[j] = CHSV(0,0,0);
  }
FastLED.show();
}

void stredy11()
{
  for(int i=104, j=104;i<=frekvence1[3]+90 && j>=frekvence2[3];i++, j--)
  {
    ledky[i] = CHSV(barva+barevnyIndex1*(i-90),255,jas);

    if(poslednipeak311<i)
        poslednipeak311=i+1;
      
      aktualniMillis311=millis();

      if(aktualniMillis311-predchoziMillis311>padani)
      {
        poslednipeak311--;
        predchoziMillis311=aktualniMillis311;
      }
      ledky[poslednipeak311]=CHSV(barvaTecky,255,jas); 

      for(int i=frekvence1[3]+90;i<poslednipeak311;i++)
        ledky[i] = CHSV(0,0,0);
      for(int i=poslednipeak311+1;i<=120;i++)
        ledky[i] = CHSV(0,0,0);

    ledky[j] = CHSV(barva+barevnyIndex1*(i-90),255,jas);

    if(poslednipeak312>j)
        poslednipeak312=j+1;
      
      aktualniMillis312=millis();

      if(aktualniMillis312-predchoziMillis312>padani)
      {
        poslednipeak312++;
        predchoziMillis312=aktualniMillis312;
      }
      ledky[poslednipeak312]=CHSV(barvaTecky,255,jas);

      for(int j=frekvence2[3]+90;j>poslednipeak312;j--)
        ledky[j] = CHSV(0,0,0);
      for(int j=poslednipeak312-1;j>=90;j--)
        ledky[j] = CHSV(0,0,0);
  }
FastLED.show();
}

void stredy12()
{
  for(int i=134, j=134;i<=frekvence1[4]+120 && j>=frekvence2[4]+120;i++, j--)
  {
    ledky[i] = CHSV(barva+barevnyIndex1*(i-120),255,jas);

    if(poslednipeak411<i)
        poslednipeak411=i+1;
      
      aktualniMillis411=millis();

      if(aktualniMillis411-predchoziMillis411>padani)
      {
        poslednipeak411--;
        predchoziMillis411=aktualniMillis411;
      }
      ledky[poslednipeak411]=CHSV(barvaTecky,255,jas); 

      for(int i=frekvence1[4]+120;i<poslednipeak411;i++)
        ledky[i] = CHSV(0,0,0);
      for(int i=poslednipeak411+1;i<=150;i++)
        ledky[i] = CHSV(0,0,0);
    
    ledky[j] = CHSV(barva+barevnyIndex1*(i-120),255,jas);

    if(poslednipeak412>j)
        poslednipeak412=j+1;
      
      aktualniMillis412=millis();

      if(aktualniMillis412-predchoziMillis412>padani)
      {
        poslednipeak412++;
        predchoziMillis412=aktualniMillis412;
      }
      ledky[poslednipeak412]=CHSV(barvaTecky,255,jas);

      for(int j=frekvence2[4]+120;j>poslednipeak412;j--)
        ledky[j] = CHSV(0,0,0);
      for(int j=poslednipeak412-1;j>=120;j--)
        ledky[j] = CHSV(0,0,0);
  }
FastLED.show();
}

void vysky11()
{
  for(int i=164, j=164;i<=frekvence1[5]+150 && j>=frekvence2[5]+150;i++, j--)
  {
    ledky[i] = CHSV(barva+barevnyIndex1*(i-150),255,jas);

    if(poslednipeak511<i)
        poslednipeak511=i+1;
      
      aktualniMillis511=millis();

      if(aktualniMillis511-predchoziMillis511>padani)
      {
        poslednipeak511--;
        predchoziMillis511=aktualniMillis511;
      }
      ledky[poslednipeak511]=CHSV(barvaTecky,255,jas); 

      for(int i=frekvence1[5]+150;i<poslednipeak511;i++)
        ledky[i] = CHSV(0,0,0);
      for(int i=poslednipeak511+1;i<=180;i++)
        ledky[i] = CHSV(0,0,0);
    
    ledky[j] = CHSV(barva+barevnyIndex1*(i-150),255,jas);

    if(poslednipeak512>j)
        poslednipeak512=j+1;
      
      aktualniMillis512=millis();

      if(aktualniMillis512-predchoziMillis512>padani)
      {
        poslednipeak512++;
        predchoziMillis512=aktualniMillis512;
      }
      ledky[poslednipeak512]=CHSV(barvaTecky,255,jas);

      for(int j=frekvence2[5]+150;j>poslednipeak512;j--)
        ledky[j] = CHSV(0,0,0);
      for(int j=poslednipeak512-1;j>=150;j--)
        ledky[j] = CHSV(0,0,0);
  }
FastLED.show();
}

void vysky12()
{
  for(int i=194, j=194;i<=frekvence1[6]+180 && j>=frekvence2[6]+180;i++, j--)
  {
    ledky[i] = CHSV(barva+barevnyIndex1*(i-180),255,jas);

    if(poslednipeak611<i)
        poslednipeak611=i+1;
      
      aktualniMillis611=millis();

      if(aktualniMillis611-predchoziMillis611>padani)
      {
        poslednipeak611--;
        predchoziMillis611=aktualniMillis611;
      }
      ledky[poslednipeak611]=CHSV(barvaTecky,255,jas); 

      for(int i=frekvence1[6]+180;i<poslednipeak611;i++)
        ledky[i] = CHSV(0,0,0);
      for(int i=poslednipeak611+1;i<=209;i++)
        ledky[i] = CHSV(0,0,0);
    
    ledky[j] = CHSV(barva+barevnyIndex1*(i-180),255,jas);

    if(poslednipeak612>j)
        poslednipeak612=j+1;
      
      aktualniMillis612=millis();

      if(aktualniMillis612-predchoziMillis612>padani)
      {
        poslednipeak612++;
        predchoziMillis612=aktualniMillis612;
      }
      ledky[poslednipeak612]=CHSV(barvaTecky,255,jas);

      for(int j=frekvence2[6]+180;j>poslednipeak612;j--)
        ledky[j] = CHSV(0,0,0);
      for(int j=poslednipeak612-1;j>=180;j--)
        ledky[j] = CHSV(0,0,0);
  }
FastLED.show();
}


void basy21()
{
  for(int i=0;i<=frekvence[0];i++)
    {
      ledky[i] = CHSV(0+barva+(barevnyIndex*i),255,jas);

      if(poslednipeak1<i)
        poslednipeak1=i+1;

      aktualniMillis1=millis();

      if(aktualniMillis1-predchoziMillis1>padani)
      {
        poslednipeak1--;
        predchoziMillis1=aktualniMillis1;
      }
      ledky[poslednipeak1]=CHSV(50+barvaTecky,255,jas);

      for(int i=frekvence[0];i<poslednipeak1;i++)
        ledky[i] = CHSV(0,0,0);
      for(int i=poslednipeak1+1;i<=30;i++)
        ledky[i] = CHSV(0,0,0);
    }
FastLED.show();
}


void basy22()
{
  for(int i=30;i<=frekvence[1]+30;i++)
    {
      ledky[i] = CHSV(60+barva+barevnyIndex*(i-30),255,jas);

      if(poslednipeak2<i)
        poslednipeak2=i+1;
      
      aktualniMillis2=millis();

      if(aktualniMillis2-predchoziMillis2>padani)
      {
        poslednipeak2--;
        predchoziMillis2=aktualniMillis2;
      }
      ledky[poslednipeak2]=CHSV(100+barvaTecky,255,jas);

      for(int i=frekvence[1]+30;i<poslednipeak2;i++)
        ledky[i] = CHSV(0,0,0);
      for(int i=poslednipeak2+1;i<=60;i++)
        ledky[i] = CHSV(0,0,0);
    }
FastLED.show();
}


void basy23()
{
  for(int i=60;i<=frekvence[2]+60;i++)
    {
      ledky[i] = CHSV(100+barva+barevnyIndex*(i-60),255,jas);

      if(poslednipeak3<i)
        poslednipeak3=i+1;

      aktualniMillis3=millis();

      if(aktualniMillis3-predchoziMillis3>padani)
      {
        poslednipeak3--;
        predchoziMillis3=aktualniMillis3;
      }
      ledky[poslednipeak3]=CHSV(200+barvaTecky,255,jas);

      for(int i=frekvence[2]+60;i<poslednipeak3;i++)
        ledky[i] = CHSV(0,0,0);
      for(int i=poslednipeak3+1;i<=90;i++)
        ledky[i] = CHSV(0,0,0);
    }
FastLED.show();
}


void stredy21()
{
  for(int i=90;i<=frekvence[3]+90;i++)
    {
      ledky[i] = CHSV(130+barva+barevnyIndex*(i-90),255,jas);

      if(poslednipeak4<i)
        poslednipeak4=i+1;

      aktualniMillis4=millis();

      if(aktualniMillis4-predchoziMillis4>padani)
      {
        poslednipeak4--;
        predchoziMillis4=aktualniMillis4;
      }
      ledky[poslednipeak4]=CHSV(30+barvaTecky,255,jas);

      for(int i=frekvence[3]+90;i<poslednipeak4;i++)
        ledky[i] = CHSV(0,0,0);
      for(int i=poslednipeak4+1;i<=120;i++)
        ledky[i] = CHSV(0,0,0);
    }
FastLED.show();
}


void stredy22()
{
  for(int i=120;i<=frekvence[4]+120;i++)
    {
      ledky[i] = CHSV(200+barva+barevnyIndex*(i-120),255,jas);

      if(poslednipeak5<i)
        poslednipeak5=i+1;

      aktualniMillis5=millis();

      if(aktualniMillis5-predchoziMillis5>padani)
      {
        poslednipeak5--;
        predchoziMillis5=aktualniMillis5;
      }
      ledky[poslednipeak5]=CHSV(0+barvaTecky,255,jas);

      for(int i=frekvence[4]+120;i<poslednipeak5;i++)
        ledky[i] = CHSV(0,0,0);
      for(int i=poslednipeak5+1;i<=150;i++)
        ledky[i] = CHSV(0,0,0);
    }
FastLED.show();
}


void vysky21()
{
  for(int i=150;i<=frekvence[5]+150;i++)
    {
      ledky[i]=CHSV(230+barva+barevnyIndex*(i-150),255,jas);

        if(poslednipeak6<i)
        poslednipeak6=i+1;

      aktualniMillis6=millis();

      if(aktualniMillis6-predchoziMillis6>padani)
      {
        poslednipeak6--;
        predchoziMillis6=aktualniMillis6;
      }
      ledky[poslednipeak6]=CHSV(120+barvaTecky,255,jas);

      for(int i=frekvence[5]+150;i<poslednipeak6;i++)
        ledky[i] = CHSV(0,0,0);
      for(int i=poslednipeak6+1;i<=180;i++)
        ledky[i] = CHSV(0,0,0);
    }
FastLED.show();
}

void vysky22()
{
  for(int i=180;i<=frekvence[6]+180;i++)
    {
      ledky[i]=CHSV(230+barva+barevnyIndex*(i-180),255,jas);

        if(poslednipeak7<i)
        poslednipeak7=i+1;

      aktualniMillis7=millis();

      if(aktualniMillis7-predchoziMillis7>padani)
      {
        poslednipeak7--;
        predchoziMillis7=aktualniMillis7;
      }
      ledky[poslednipeak7]=CHSV(80+barvaTecky,255,jas); 

      for(int i=frekvence[6]+180;i<poslednipeak7;i++)
        ledky[i] = CHSV(0,0,0);
      for(int i=poslednipeak7+1;i<=209;i++)
        ledky[i] = CHSV(0,0,0);
    }
FastLED.show();
}

void basybasic1()
{
  for(int i=0;i<=frekvence[0];i++)
    {
      ledky[i] = CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=frekvence[0];i<30;i++)
        ledky[i] = CHSV(0,0,0);
    }
FastLED.show();
}

void basybasic2()
{
  for(int i=30;i<=frekvence[1]+30;i++)
    {
      ledky[i] = CHSV(barva+barevnyIndex*(i-30),255,jas);
      for(int i=frekvence[1]+30;i<60;i++)
        ledky[i] = CHSV(0,0,0);
    }
FastLED.show();
}

void basybasic3()
{
  for(int i=60;i<=frekvence[2]+60;i++)
    {
      ledky[i] = CHSV(barva+barevnyIndex*(i-60),255,jas);
      for(int i=frekvence[2]+60;i<90;i++)
        ledky[i] = CHSV(0,0,0);
    }
FastLED.show();
}

void stredybasic1()
{
  for(int i=90;i<=frekvence[3]+90;i++)
    {
      ledky[i] = CHSV(barva+barevnyIndex*(i-90),255,jas);
      for(int i=frekvence[3]+90;i<120;i++)
        ledky[i] = CHSV(0,0,0);
    }
FastLED.show();
}

void stredybasic2()
{
  for(int i=120;i<=frekvence[4]+120;i++)
    {
      ledky[i] = CHSV(barva+barevnyIndex*(i-120),255,jas);
      for(int i=frekvence[4]+120;i<150;i++)
        ledky[i] = CHSV(0,0,0);
    }
FastLED.show();
}

void vyskybasic1()
{
  for(int i=150;i<=frekvence[5]+150;i++)
    {
      ledky[i] = CHSV(barva+barevnyIndex*(i-150),255,jas);
      for(int i=frekvence[5]+150;i<180;i++)
        ledky[i] = CHSV(0,0,0);
    }
FastLED.show();
}

void vyskybasic2()
{
  for(int i=180;i<=frekvence[6]+180;i++)
    {
      ledky[i] = CHSV(barva+barevnyIndex*(i-180),255,jas);
      for(int i=frekvence[6]+180;i<210;i++)
        ledky[i] = CHSV(0,0,0);
    }
FastLED.show();
}

void basybasic11()
{
  for(int i=0;i<=frekvence[0];i++)
    {
      ledky[i] = CHSV(0+barva+(barevnyIndex*i),255,jas);
      for(int i=frekvence[0];i<30;i++)
        ledky[i] = CHSV(0,0,0);
    }
FastLED.show();
}

void basybasic12()
{
  for(int i=30;i<=frekvence[1]+30;i++)
    {
      ledky[i] = CHSV(0+barva+barevnyIndex*(i-30),255,jas);
      for(int i=frekvence[1]+30;i<60;i++)
        ledky[i] = CHSV(0,0,0);
    }
FastLED.show();
}

void basybasic13()
{
  for(int i=60;i<=frekvence[2]+60;i++)
    {
      ledky[i] = CHSV(125+barva+barevnyIndex*(i-60),255,jas);
      for(int i=frekvence[2]+60;i<90;i++)
        ledky[i] = CHSV(0,0,0);
    }
FastLED.show();
}

void stredybasic11()
{
  for(int i=90;i<=frekvence[3]+90;i++)
    {
      ledky[i] = CHSV(80+barva+barevnyIndex*(i-90),255,jas);
      for(int i=frekvence[3]+90;i<120;i++)
        ledky[i] = CHSV(0,0,0);
    }
FastLED.show();
}

void stredybasic12()
{
  for(int i=120;i<=frekvence[4]+120;i++)
    {
      ledky[i] = CHSV(215+barva+barevnyIndex*(i-120),255,jas);
      for(int i=frekvence[4]+120;i<150;i++)
        ledky[i] = CHSV(0,0,0);
    }
FastLED.show();
}

void vyskybasic11()
{
  for(int i=150;i<=frekvence[5]+150;i++)
    {
      ledky[i] = CHSV(142+barva+barevnyIndex*(i-150),255,jas);
      for(int i=frekvence[5]+150;i<180;i++)
        ledky[i] = CHSV(0,0,0);
    }
FastLED.show();
}

void vyskybasic12()
{
  for(int i=180;i<=frekvence[6]+180;i++)
    {
      ledky[i] = CHSV(15+barva+barevnyIndex*(i-180),255,jas);
      for(int i=frekvence[6]+180;i<210;i++)
        ledky[i] = CHSV(0,0,0);
    }
FastLED.show();
}


void lcd1()
{
  switch(volba)
  {
  case 0:
  {
    lcd.setCursor(0,0);
    lcd.print("+  VOLBA REZIMU ");
    lcd.setCursor(0,1);
    lcd.print("-  ANIMACE");
    lcd.print("  (0");
    lcd.print(")");
    break;
  }
  case 1:
  {
    lcd.setCursor(0,0);
    lcd.print("+  VOLBA REZIMU ");
    lcd.setCursor(0,1);
    lcd.print("-       ");
    lcd.print(volba);
    lcd.print("      ");
    break;
  }
  case 2:
  {
    lcd.setCursor(0,0);
    lcd.print("+  VOLBA REZIMU ");
    lcd.setCursor(0,1);
    lcd.print("-  ");
    lcd.print(volba);
    lcd.print("      ");
    break;
  }
  case 3:
  {
    lcd.setCursor(0,0);
    lcd.print("+  VOLBA REZIMU ");
    lcd.setCursor(0,1);
    lcd.print("-       ");
    lcd.print(volba);
    lcd.print("      ");
    break;
  }
  case 4:
  {
    lcd.setCursor(0,0);
    lcd.print("+  VOLBA REZIMU ");
    lcd.setCursor(0,1);
    lcd.print("-   BASIC");
    lcd.print(" (");
    lcd.print(volba);
    lcd.print(")   ");
    break;
    
  }
  case 5:
  {
    lcd.setCursor(0,0);
    lcd.print("+  VOLBA REZIMU ");
    lcd.setCursor(0,1);
    lcd.print("-       ");
    lcd.print(volba);
    lcd.print("      ");
    break;
  }
  case 6:
  {
    lcd.setCursor(0,0);
    lcd.print("+  VOLBA REZIMU ");
    lcd.setCursor(0,1);
    lcd.print("-       ");
    lcd.print(volba);
    lcd.print("      ");
    break;
  }
 }
}
