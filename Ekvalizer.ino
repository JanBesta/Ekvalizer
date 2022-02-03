 #include <FastLED.h>
#include <LiquidCrystal_I2C.h> // stazena knihovna umoznujici mi pracovat s LCD
#include <AudioAnalyzer.h> //stazena knihovna umoznujici mi pracovat s modulem viz kapitola 8.1 Deska s cipem MSGEQ7
#include <Wire.h> //pro nastaveni komunikace LCD pres piny SDA a SCL

Analyzer Audio = Analyzer(12,13,0);//STROBE pin ->12  RESET pin ->13 Analog Pin ->0
//Verze 1.3 pro spektralni analyzer

#define PINproLEDky 8
#define pocetLED 210
#define PINproLEDkypodsviceni 9
#define pocetLEDpodsviceni 66

#define BRIGHTNESS  200
#define SPARKING 120
#define COOLING  55
#define UPDATES_PER_SECOND 100
#define FRAMES_PER_SECOND  60

#define TOLERANCE 7
#define TOLERANCE_INDEX 1

// Gradient palette "bhw2_turq_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/bhw/bhw2/tn/bhw2_turq.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 28 bytes of program space.

DEFINE_GRADIENT_PALETTE( bhw2_turq_gp ) {
    0,   1, 33, 95,
   38,   1,107, 37,
   76,  42,255, 45,
  127, 255,255, 45,
  178,  42,255, 45,
  216,   1,107, 37,
  255,   1, 33, 95};

CRGBPalette16 moje=bhw2_turq_gp;
CRGBPalette16 gPal;

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

//Promenne pro ekvalizer
int hodnotafrekvence[7], frekvence[7], frekvence1[7], frekvence2[7], frekvenceZBoku[7];
unsigned long jas;
unsigned long padani;
unsigned long barevnyIndex, barevnyIndex1;
unsigned long barva;
unsigned long barvaTecky;
int volba=1, volbaPodsviceni=0, rezimPodsviceni=0;
bool gReverseDirection = false;
int JAS=255;

unsigned int BasJas=0, StredyJas=0, VyskyJas=0;
unsigned int posledniVolba=1, poslednirezimPodsviceni=0, posledniJas=0, posledniBarva=0, posledniBarevnyIndex=0, posledniBarevnyIndex1=0, posledniPadani=0, posledniBarvaTecky=0;
unsigned int rozdilVolba, rozdilRezimPodsviceni, rozdilJas, rozdilBarva, rozdilBarevnyIndex, rozdilBarevnyIndex1, rozdilPadani, rozdilBarvaTecky;

int poslednipeak1=0, poslednipeak2=0, poslednipeak3=0, poslednipeak4=0, poslednipeak5=0, poslednipeak6=0, poslednipeak7=0;
unsigned long aktualniMillis, predchoziMillis=0, aktualniMillis1, predchoziMillis1=0, aktualniMillis2, predchoziMillis2=0, aktualniMillis3, predchoziMillis3=0, aktualniMillis4, predchoziMillis4=0, aktualniMillis5, predchoziMillis5=0, aktualniMillis6, predchoziMillis6=0, aktualniMillis7, predchoziMillis7=0;

long poslednipeak011=0, poslednipeak012=0, poslednipeak111=0, poslednipeak112=0, poslednipeak211=0, poslednipeak212=0, poslednipeak311=0, poslednipeak312=0, poslednipeak411=0, poslednipeak412=0, poslednipeak511=0, poslednipeak512=0, poslednipeak611=0, poslednipeak612=0;
unsigned long aktualniMillis011, predchoziMillis011=0, aktualniMillis012, predchoziMillis012=0, aktualniMillis111, predchoziMillis111=0, aktualniMillis112, predchoziMillis112=0, aktualniMillis211, predchoziMillis211=0, aktualniMillis212, predchoziMillis212=0; 
unsigned long aktualniMillis311, predchoziMillis311=0, aktualniMillis312, predchoziMillis312=0, aktualniMillis411, predchoziMillis411=0, aktualniMillis412, predchoziMillis412=0, aktualniMillis511, predchoziMillis511=0, aktualniMillis512, predchoziMillis512=0, aktualniMillis611, predchoziMillis611=0, aktualniMillis612, predchoziMillis612=0;

CRGB ledky[pocetLED];
CRGB ledkyPodsviceni[pocetLEDpodsviceni];

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

void animace();

void Fire2012WithPalette();
void animacePodsviceni();
void toceniLedekbila();
void toceniLedekbarva();
void prolinani();
void rainbow() ;
void rainbowWithGlitter();
void addGlitter( fract8 chanceOfGlitter);
void confetti();
void sinelon();
void bpm();
void juggle();
void second_light();
void cylon();
void fadeall();

void tlacitko1();
void tlacitko2();

void lcdfunkce();

//LCD displej s I2C prevodnikem
LiquidCrystal_I2C lcd(0x27,16,2); //typ,sloupce,radky

//animacni mod
uint8_t startIndexRezim=0;
uint8_t startIndex=0;

void setup()
{
  Serial.begin(1000000); //nastaveni seriove komunikace. Diky tomuto kodu jsem jeste schopen urychlit dynamicnost ekvalizeru
  FastLED.addLeds<WS2812B,PINproLEDky, GRB>(ledky,pocetLED); //Nastavi LED pasky - prida do databaze typ pasku, pocet LED, pin z ktereho pujdou data pro pasky a typ barevne kolaze
  FastLED.addLeds<WS2812B,PINproLEDkypodsviceni, GRB>(ledkyPodsviceni,pocetLEDpodsviceni);
  Audio.Init(); //priradi modul s MSGEQ7 k arduino knihovne, kterou jsem taktez stahl z internetu viz kapitola 8.1 Deska s cipem MSGEQ7
  pinMode(8,OUTPUT);
  pinMode(9,OUTPUT);
  pinMode(12,OUTPUT);
  pinMode(13,OUTPUT);
  
  attachInterrupt(digitalPinToInterrupt(2),tlacitko1,RISING);
  attachInterrupt(digitalPinToInterrupt(3),tlacitko2,RISING);
  
  pinMode(A0,INPUT);
  pinMode(A1,INPUT);
  pinMode(A2,INPUT);
  pinMode(A3,INPUT);
  pinMode(A4,INPUT);
  pinMode(A5,INPUT);
  
  gPal = HeatColors_p;
  
  lcd.init(); //priradi I2C prevodnik LCD
  lcd.backlight(); //zapne podscviceni LCD
  lcd.setCursor(0,0);
  lcd.print(" Prave probiha: ");
  lcd.setCursor(1,1);
  lcd.print("Test LED Pasku");
  
  for(int i = 0;i<pocetLED;i++)
  {
    ledky[i] = CHSV(120+(7*i),255,100);
    for(int j=0;j<pocetLEDpodsviceni;j++) 
    {
      ledkyPodsviceni[j]=CHSV(0+(4*j),255,150);
    }
    FastLED.show();
  }
  
  for(int i=0;i<pocetLED;i++)
  {
    for(int j=0;j<pocetLEDpodsviceni;j++) 
    {
      ledky[i]=CRGB(0,0,0);
      ledkyPodsviceni[j]=CRGB(0,0,0);
    }
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


  if(map(analogRead(A1),0,1023,255,0)<jas || map(analogRead(A1),0,1023,255,0)>jas)
  {
    jas=map(analogRead(A1),0,1023,255,0);
    if(jas<=0)
      jas=0;
    if(jas>255)
      jas=255;
  }

  if(map(analogRead(A2),0,1023,255,0)<barva || map(analogRead(A2),0,1023,255,0>barva))
  { 
    barva=map(analogRead(A2),00,1023,255,0);
    if(barva<0)
      barva=0;
    if(barva>255)
      barva=255;
  }
  
  if(map(analogRead(A3),0,1023,15,0)<barevnyIndex || map(analogRead(A3),0,1023,15,0)>barevnyIndex)
  {
    barevnyIndex=map(analogRead(A3),0,1023,12,0);
    if(barevnyIndex<0)
      barevnyIndex=0;
    if(barevnyIndex>12)
      barevnyIndex=12;
  }
  
  if(map(analogRead(A3),0,1023,16,0)<barevnyIndex1 || map(analogRead(A3),0,1023,16,0)>barevnyIndex1)
    {
      barevnyIndex1=map(analogRead(A3),0,1023,16,0);
      if(barevnyIndex1<0)
        barevnyIndex1=0;
      if(barevnyIndex1>16)
        barevnyIndex1=16;
    }
    
  if(map(analogRead(A4),0,1023,300,25)<padani || map(analogRead(A4),0,1023,300,25)>padani)
    {
      padani=map(analogRead(A4),0,1023,300,25);
      if(padani<25)
        padani=25;
      if(padani>300)
        padani=300;
    }
    
  if(map(analogRead(A5),0,1023,255,0)<barvaTecky || map(analogRead(A5),0,1023,255,0)>barvaTecky)
    {
      barvaTecky=map(analogRead(A5),0,1023,255,0);
      if(barvaTecky<0)
        barvaTecky=0;
      if(barvaTecky>255)
        barvaTecky=255;
    }
  rozdilVolba=abs(volba-posledniVolba);
  rozdilRezimPodsviceni=abs(rezimPodsviceni-poslednirezimPodsviceni);
  rozdilJas=abs(jas-posledniJas);
  rozdilBarva=abs(barva-posledniBarva);
  rozdilBarevnyIndex=abs(barevnyIndex-posledniBarevnyIndex);
  rozdilBarevnyIndex1=abs(barevnyIndex1-posledniBarevnyIndex1);
  rozdilPadani=abs(padani-posledniPadani);
  rozdilBarvaTecky=abs(barvaTecky-posledniBarvaTecky);
  
    if(rozdilJas>TOLERANCE)
    {
        lcd.clear();
        lcd.setCursor(3,0);
        lcd.print("Menite jas");
        lcd.setCursor(4,1);
        lcd.print("JAS:");
        lcd.print(jas);
        posledniJas=jas;
    }
    if(rozdilBarva>TOLERANCE)
    {
        lcd.clear();
        lcd.setCursor(2,0);
        lcd.print("Menite barvu");
        lcd.setCursor(3,1);
        lcd.print("BARVA:");
        lcd.print(barva);
        posledniBarva=barva;
    }
    if(rozdilBarevnyIndex>TOLERANCE_INDEX)
    {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Menite barvIndex");
        lcd.setCursor(0,1);
        lcd.print("BAREVNY INDEX:");
        lcd.print(barevnyIndex);
        posledniBarevnyIndex=barevnyIndex;
    }
    if(volba==2)
    {
      if(rozdilBarevnyIndex1>TOLERANCE_INDEX)
      {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Menite barvIndex");
          lcd.setCursor(0,1);
          lcd.print("BAREVNY INDEX:");
          lcd.print(barevnyIndex1);
          posledniBarevnyIndex1=barevnyIndex1;
      }
    }
    if(rozdilPadani>TOLERANCE)
    {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Menite rych bodu");
        lcd.setCursor(1,1);
        lcd.print("RYCHL BODU:");
        lcd.print(padani);
        posledniPadani=padani;
    }
    if(rozdilBarvaTecky>TOLERANCE)
    {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Menite barv bodu");
        lcd.setCursor(1,1);
        lcd.print("BARVA BODU:");
        lcd.print(barvaTecky);
        posledniBarvaTecky=barvaTecky;
    }
  aktualniMillis=millis();
  if(aktualniMillis-predchoziMillis>4000)
  {
    lcdfunkce();
    predchoziMillis=aktualniMillis;
  }
  else if(rozdilVolba>TOLERANCE_INDEX || rozdilRezimPodsviceni>TOLERANCE_INDEX)
    {
      lcdfunkce();
    }
  
  EVERY_N_MILLISECONDS( 20 ) { gHue++; }
  EVERY_N_SECONDS( 20 ) { volbaPodsviceni++; }
  
  if(volbaPodsviceni>3)
    volbaPodsviceni=0;
  
  switch(volba)
  {
    case 0:
    {
      //Fire2012WithPalette();
      animace();
      switch(rezimPodsviceni)
      {
        case 0:
        {
          animacePodsviceni();
        }
        break;
        case 1:
        {
          switch(volbaPodsviceni)
          {
            case 1:
            {
              confetti();
            }
            break;
            case 2:
            {
              rainbowWithGlitter();
            }
            break;
          }
        }
        break;
      }
    }
    FastLED.show();
    break;
    case 1:
    {
      basy01();
      basy02();
      basy03();
      stredy01();
      stredy02();
      vysky01();
      vysky02();
      switch(rezimPodsviceni)
      {
        case 0:
        {
          animacePodsviceni();
        }
        break;
        case 1:
        {
          switch(volbaPodsviceni)
          {
            case 1:
            {
              confetti();
            }
            break;
            case 2:
            {
              rainbowWithGlitter();
            }
            break;
            case 3:
            {
              sinelon();
            }
            break;
          }
        }
        break;
      }
    }
    FastLED.show();
    break;
    case 2:
    {
      basy11();
      basy12();
      basy13();
      stredy11();
      stredy12();
      vysky11();
      vysky12();
      switch(rezimPodsviceni)
      {
        case 0:
        {
          animacePodsviceni();
        }
        break;
        case 1:
        {
          switch(volbaPodsviceni)
          {
            case 1:
            {
              confetti();
            }
            break;
            case 2:
            {
              rainbowWithGlitter();
            }
            break;
            case 3:
            {
              sinelon();
            }
            break;
          }
        }
        break;
      }
    }
    FastLED.show();
    break;
    case 3:
    {
      basy21();
      basy22();
      basy23();
      stredy21();
      stredy22();
      vysky21();
      vysky22();
      switch(rezimPodsviceni)
      {
        case 0:
        {
          animacePodsviceni();
        }
        break;
        case 1:
        {
          switch(volbaPodsviceni)
          {
            case 1:
            {
              confetti();
            }
            break;
            case 2:
            {
              rainbowWithGlitter();
            }
            break;
            case 3:
            {
              sinelon();
            }
            break;
          }
        }
        break;
      }
    }
    FastLED.show();
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
      switch(rezimPodsviceni)
      {
        case 0:
        {
          animacePodsviceni();
        }
        break;
        case 1:
        {
          switch(volbaPodsviceni)
          {
            case 1:
            {
              confetti();
            }
            break;
            case 2:
            {
              rainbowWithGlitter();
            }
            break;
            case 3:
            {
              sinelon();
            }
            break;
          }
        }
        break;
      }
    }
    FastLED.show();
    break;
    case 5:
    {
      basybasic11();
      basybasic12();
      basybasic13();
      stredybasic11();
      stredybasic12();
      vyskybasic11();
      vyskybasic12();
      switch(rezimPodsviceni)
      {
        case 0:
        {
          animacePodsviceni();
        }
        break;
        case 1:
        {
          switch(volbaPodsviceni)
          {
            case 1:
            {
              confetti();
            }
            break;
            case 2:
            {
              rainbowWithGlitter();
            }
            break;
            case 3:
            {
              sinelon();
            }
            break;
          }
        }
        break;
      }
    }
    FastLED.show();
    break;
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
      switch(rezimPodsviceni)
      {
        case 0:
        {
          animacePodsviceni();
        }
        break;
        case 1:
        {
          switch(volbaPodsviceni)
          {
            case 1:
            {
              confetti();
            }
            break;
            case 2:
            {
              rainbowWithGlitter();
            }
            break;
            case 3:
            {
              sinelon();
            }
            break;
          }
        }
        break;
      }
    }
    FastLED.show();
    break;
  }
}

void tlacitko1()
{
  volba++;
  if(volba>6)
    volba=0;
}

void tlacitko2()
{
  rezimPodsviceni=!rezimPodsviceni;
}

void lcdfunkce()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("REZIM VIZUAL:");
  lcd.setCursor(14,0);
  lcd.print(volba);
  lcd.setCursor(0,1);
  lcd.print("REZIM PODSVIC:");
  lcd.setCursor(15,1);
  lcd.print(rezimPodsviceni);
}


void animace()
{
  startIndexRezim = startIndexRezim + 4;
  int barevnyIndexRezim=startIndexRezim;
  for(int i = 0; i<pocetLED; i++) 
  {
  ledky[i] = ColorFromPalette(RainbowColors_p, barevnyIndexRezim, jas);
  barevnyIndexRezim+=6;
  }
  FastLED.show();
}

void Fire2012WithPalette()
{
// Array of temperature readings at each simulation cell
  static byte heat[210];

  // Step 1.  Cool down every cell a little
    for( int i = 0; i < 30; i++)
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / 30) + 2));
    for( int i = 30; i < 60; i++)
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / 60) + 2));
    for( int i = 60; i < 90; i++)
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / 90) + 2));
    for( int i = 90; i < 120; i++)
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / 120) + 2));
    for( int i = 120; i < 150; i++)
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / 150) + 2));
    for( int i = 150; i < 180; i++)
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / 180) + 2));
    for( int i = 180; i < 210; i++)
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / 210) + 2));
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= 30 - 1; k >= 2; k--)
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    for( int k= 60 - 1; k >= 4; k--)
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    for( int k= 90 - 1; k >= 6; k--)
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    for( int k= 120 - 1; k >= 8; k--)
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    for( int k= 150 - 1; k >= 10; k--)
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    for( int k= 180 - 1; k >= 12; k--)
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < SPARKING ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < 30; j++) {
      // Scale the heat value from 0-255 down to 0-240
      // for best results with color palettes.
      byte colorindex = scale8( heat[j], 240);
      CRGB color = ColorFromPalette( gPal, colorindex);
      int pixelnumber;
      if( gReverseDirection ) {
        pixelnumber = (30-1) - j;
      } else {
        pixelnumber = j;
      }
      ledky[pixelnumber] = color;
    }
    for( int j = 30; j < 60; j++) {
      // Scale the heat value from 0-255 down to 0-240
      // for best results with color palettes.
      byte colorindex = scale8( heat[j], 240);
      CRGB color = ColorFromPalette( gPal, colorindex);
      int pixelnumber;
      if( gReverseDirection ) {
        pixelnumber = (60-1) - j;
      } else {
        pixelnumber = j;
      }
      ledky[pixelnumber] = color;
    }
    for( int j = 60; j < 90; j++) {
      // Scale the heat value from 0-255 down to 0-240
      // for best results with color palettes.
      byte colorindex = scale8( heat[j], 240);
      CRGB color = ColorFromPalette( gPal, colorindex);
      int pixelnumber;
      if( gReverseDirection ) {
        pixelnumber = (90-1) - j;
      } else {
        pixelnumber = j;
      }
      ledky[pixelnumber] = color;
    }
    for( int j = 90; j < 120; j++) {
      // Scale the heat value from 0-255 down to 0-240
      // for best results with color palettes.
      byte colorindex = scale8( heat[j], 240);
      CRGB color = ColorFromPalette( gPal, colorindex);
      int pixelnumber;
      if( gReverseDirection ) {
        pixelnumber = (120-1) - j;
      } else {
        pixelnumber = j;
      }
      ledky[pixelnumber] = color;
    }
    for( int j = 120; j < 150; j++) {
      // Scale the heat value from 0-255 down to 0-240
      // for best results with color palettes.
      byte colorindex = scale8( heat[j], 240);
      CRGB color = ColorFromPalette( gPal, colorindex);
      int pixelnumber;
      if( gReverseDirection ) {
        pixelnumber = (150-1) - j;
      } else {
        pixelnumber = j;
      }
      ledky[pixelnumber] = color;
    }
    for( int j = 150; j < 180; j++) {
      // Scale the heat value from 0-255 down to 0-240
      // for best results with color palettes.
      byte colorindex = scale8( heat[j], 240);
      CRGB color = ColorFromPalette( gPal, colorindex);
      int pixelnumber;
      if( gReverseDirection ) {
        pixelnumber = (180-1) - j;
      } else {
        pixelnumber = j;
      }
      ledky[pixelnumber] = color;
    }
    for( int j = 180; j < 210; j++) {
      // Scale the heat value from 0-255 down to 0-240
      // for best results with color palettes.
      byte colorindex = scale8( heat[j], 240);
      CRGB color = ColorFromPalette( gPal, colorindex);
      int pixelnumber;
      if( gReverseDirection ) {
        pixelnumber = (210-1) - j;
      } else {
        pixelnumber = j;
      }
      ledky[pixelnumber] = color;
    }
}


//Podsviceni


void animacePodsviceni()
{
  if(frekvence[0]>=21)
  {
    BasJas=250;
  }
  else if(frekvence[1]>=21)
  {
    BasJas=250;
  }
  else
  {
    BasJas=75;
  }
  
  if(frekvence[2]>=25)
  {
    StredyJas=250;
  }
  else if(frekvence[3]>=22)
  {
    StredyJas=250;
  }
  else
  {
    StredyJas=75;
  }
  
  if(frekvence[4]>=10)
  {
    VyskyJas=250;
  }
  else if(frekvence[5]>=6)
  {
    VyskyJas=250;
  }
  else
  {
    VyskyJas=75;
  }
  startIndex=startIndex+5;
  int barevnyIndex=startIndex;
  for(int i=0;i<20;i++) 
  {
    ledkyPodsviceni[i] = ColorFromPalette(RainbowColors_p, barevnyIndex, VyskyJas);
    barevnyIndex+=4;
  }
  for(int i=20;i<33;i++) 
  {
    ledkyPodsviceni[i] = ColorFromPalette(RainbowColors_p, barevnyIndex, StredyJas);
    barevnyIndex+=4;
  }
  for(int i=33;i<53;i++) 
  {
    ledkyPodsviceni[i] = ColorFromPalette(RainbowColors_p, barevnyIndex, BasJas);
    barevnyIndex+=4;
  }
  for(int i=53;i<66;i++)
  {
    ledkyPodsviceni[i] = ColorFromPalette(RainbowColors_p, barevnyIndex, StredyJas);
    barevnyIndex+=4;
  }
}

void toceniLedekbila()
{
    for (int q=0; q < 3; q++)
    {
      for (uint16_t i=0; i < pocetLEDpodsviceni; i=i+3)
      {
        ledkyPodsviceni[i+q]=CHSV(255,0,255);    //turn every third pixel on
      }
      FastLED.show();
      delay(50);
      
      for (uint16_t i=0; i < pocetLEDpodsviceni; i=i+3) {
        ledkyPodsviceni[i+q]=CRGB(0,0,0);        //turn every third pixel off
      }
    }
}

void toceniLedekbarva()
{
    for (int q=0; q < 4; q++)
    {
      for (uint16_t i=0; i < pocetLEDpodsviceni; i=i+4)
      {
        ledkyPodsviceni[i+q]=CHSV(0+(10*i),255,255); //turn every third pixel on

      }
      FastLED.show();
      delay(50);
      
      for (uint16_t i=0; i < pocetLEDpodsviceni; i=i+4) {
        ledkyPodsviceni[i+q]=CRGB(0,0,0);        //turn every third pixel off
      }
    }
}

void prolinani()
{
  startIndexRezim++;
  int barevnyIndexRezim=startIndexRezim;
  for(int i=0;i<pocetLEDpodsviceni;i++) 
  {
    ledkyPodsviceni[i] = ColorFromPalette(RainbowColors_p, barevnyIndexRezim, JAS);
    JAS=JAS-10;
    barevnyIndexRezim+=10;
  }
  FastLED.show();
  delay(5);
}

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( ledkyPodsviceni, pocetLEDpodsviceni, 0, 5);
}

void rainbowWithGlitter()
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(85);
  FastLED.show();
}
 
void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    ledkyPodsviceni[ random16(pocetLEDpodsviceni) ] += CRGB::White;
  }
}
 
void confetti()
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( ledkyPodsviceni, pocetLEDpodsviceni, 10);
  int pos = random16(pocetLEDpodsviceni);
  ledkyPodsviceni[pos] += CHSV( gHue + random8(64), 200, 255);
  delay(10);
  FastLED.show();
}
 
void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( ledkyPodsviceni, pocetLEDpodsviceni, 4);
  int pos = beatsin16( 12, 0, pocetLEDpodsviceni-1 );
  ledkyPodsviceni[pos] += CHSV(gHue, 255, 192);
  FastLED.show();
}
 
void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < pocetLEDpodsviceni; i++) { //9948
    ledkyPodsviceni[i] = ColorFromPalette(moje, 5+(i*2), beat-5+(i*10));
  }
  FastLED.show();
}
 
void juggle()
{
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( ledkyPodsviceni, pocetLEDpodsviceni, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    ledkyPodsviceni[beatsin16( i+7, 0, pocetLEDpodsviceni-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
  FastLED.show();
}
 

void second_light()
{
  static uint8_t hue = 0;
   for(int whiteLed = 0; whiteLed < pocetLEDpodsviceni; whiteLed = whiteLed + 1) {
      // Turn our current led on to white, then show the ledkyPodsviceni
      ledkyPodsviceni[whiteLed] =  CHSV(hue++, 255, 255);
 
      // Show the ledkyPodsviceni (only one of which is set to white, from above)
      FastLED.show();
 
      // Wait a little bit
      delay(20);
 
      // Turn our current led back to black for the next loop around
      ledkyPodsviceni[whiteLed] = CHSV(hue++, 255, 255);
   }
}
void cylon()
{
    static uint8_t hue = 0;
 // Serial.print("x");
  // First slide the led in one direction
  for(int i = 0; i < pocetLEDpodsviceni; i++) {
    // Set the i'th led to red 
    ledkyPodsviceni[i] = CHSV(hue++, 255, 255);
    // Show the ledkyPodsviceni
    FastLED.show(); 
    // now that we've shown the ledkyPodsviceni, reset the i'th led to black
    // ledkyPodsviceni[i] = CRGB::Black;
    fadeall();
    // Wait a little bit before we loop around and do it again
    delay(5);
  }
  //Serial.print("x");
 
  // Now go in the other direction.  
  for(int i = (pocetLEDpodsviceni)-1; i >= 0; i--) {
    // Set the i'th led to red 
    ledkyPodsviceni[i] = CHSV(hue++, 255, 255);
    // Show the ledkyPodsviceni
    FastLED.show();
    // now that we've shown the ledkyPodsviceni, reset the i'th led to black
    // ledkyPodsviceni[i] = CRGB::Black;
    fadeall();
    // Wait a little bit before we loop around and do it again
    delay(5);
  }
}
void fadeall() { for(int i = 0; i < pocetLEDpodsviceni; i++) { ledkyPodsviceni[i].nscale8(250); } }




//Všechny níže zmíněné proměnné jsou očíslovány, protože nebylo možné používat jen jednu proměnnou
void basy01()
{
  //naprogramování ktere ledky mají svítit podle čtené hodnoty frekvence (vizualni zobrazeni je až na konci funkce))
  for(int i=0;i<=frekvence[0];i++)
    {
      //nastavení barvy, saturace a jasu
      ledky[i] = CHSV(barva+(barevnyIndex*i),255,jas);

      //podmínka, která mi nastaví bod na nejvyšší hodnotu pokud je vyšší nez předtím (+1 protože by se mi překryval bod s nejvyššíi čtenou hodnotou)
      if(poslednipeak1<i)
        poslednipeak1=i+1;

      aktualniMillis1=millis();
      //podmínka, která dělá padání tečky (tato podmínka se používá pokud chceme aby arduino dělalo "2 věci najednou")
      if(aktualniMillis1-predchoziMillis1>padani)
      {
        if(poslednipeak1>i)
          poslednipeak1--;
        else
          poslednipeak1=0;
        predchoziMillis1=aktualniMillis1;
      }
      
      ledky[poslednipeak1]=CHSV(barvaTecky,255,jas); //nastavení barvy bodu a stejného jasu jako jsou ostatní LEDky

      //1. for - zhasina ledky mezi bodem a LEDkami, ktere jezdi podle hodnoty a 2. for zhasina od bodu do konce pasku (+1 protoze by mi jinak zhasl i ten daný bod)
      for(int i=frekvence[0];i<poslednipeak1;i++)
        ledky[i] = CHSV(0,0,0);
      for(int i=poslednipeak1+1;i<=29;i++)
        ledky[i] = CHSV(0,0,0);
    }

//Tato funkce teprve vizuálně zobrazí všechna LED
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
        if(poslednipeak2>i)
          poslednipeak2--;
        else
          poslednipeak2=30;
        predchoziMillis2=aktualniMillis2;
      }
      ledky[poslednipeak2]=CHSV(barvaTecky,255,jas); 

      for(int i=frekvence[1]+30;i<poslednipeak2;i++)
        ledky[i] = CHSV(0,0,0);
      for(int i=poslednipeak2+1;i<=59;i++)
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
        if(poslednipeak3>i)
          poslednipeak3--;
        else
          poslednipeak3=60;
        predchoziMillis3=aktualniMillis3;
      }
      
      ledky[poslednipeak3]=CHSV(barvaTecky,255,jas); 

      for(int i=frekvence[2]+60;i<poslednipeak3;i++)
        ledky[i] = CHSV(0,0,0);
      for(int i=poslednipeak3+1;i<=89;i++)
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
        if(poslednipeak4>i)
          poslednipeak4--;
        else
          poslednipeak4=90;
        predchoziMillis4=aktualniMillis4;
      }
      
      ledky[poslednipeak4]=CHSV(barvaTecky,255,jas); 

      for(int i=frekvence[3]+90;i<poslednipeak4;i++)
        ledky[i] = CHSV(0,0,0);
      for(int i=poslednipeak4+1;i<=119;i++)
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
        if(poslednipeak5>i)
          poslednipeak5--;
        else
          poslednipeak5=120;
        predchoziMillis5=aktualniMillis5;
      }
      
      ledky[poslednipeak5]=CHSV(barvaTecky,255,jas);

      for(int i=frekvence[4]+120;i<poslednipeak5;i++)
        ledky[i] = CHSV(0,0,0);
      for(int i=poslednipeak5+1;i<=149;i++)
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
        if(poslednipeak6>i)
          poslednipeak6--;
        else
          poslednipeak6=150;
        predchoziMillis6=aktualniMillis6;
      }
      
      ledky[poslednipeak6]=CHSV(barvaTecky,255,jas);

      for(int i=frekvence[5]+150;i<poslednipeak6;i++)
        ledky[i] = CHSV(0,0,0);
      for(int i=poslednipeak6+1;i<=179;i++)
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
        if(poslednipeak7>i)
          poslednipeak7--;
        else
          poslednipeak7=180;
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
        if(poslednipeak1>i)
          poslednipeak1--;
        else
          poslednipeak1=0;
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
        if(poslednipeak2>i)
          poslednipeak2--;
        else
          poslednipeak2=30;
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
        if(poslednipeak3>i)
          poslednipeak3--;
        else
          poslednipeak3=60;
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
        if(poslednipeak4>i)
          poslednipeak4--;
        else
          poslednipeak4=90;
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
        if(poslednipeak5>i)
          poslednipeak5--;
        else
          poslednipeak5=120;
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
        if(poslednipeak6>i)
          poslednipeak6--;
        else
          poslednipeak6=150;
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
        if(poslednipeak7>i)
          poslednipeak7--;
        else
          poslednipeak7=180;
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

void zBoku1()
{
  switch(frekvenceZBoku[0])
  {
  case 1:
    {
    for(int i=1;i<=4;i++)
      ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
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
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=31;i<=34;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
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
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=31;i<=34;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=61;i<=64;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
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
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=31;i<=34;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=61;i<=64;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=91;i<=94;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
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
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=31;i<=34;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=61;i<=64;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=91;i<=94;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=121;i<=124;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=151;i<=154;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=181;i<=184;i++)
        ledky[i]=CHSV(0,0,0);
      break;
    }
  case 6:
    {
      for(int i=1;i<=4;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=31;i<=34;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=61;i<=64;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=91;i<=94;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=121;i<=124;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=151;i<=154;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=181;i<=184;i++)
        ledky[i]=CHSV(0,0,0);
      break;
    }
  case 7:
    {
      for(int i=1;i<=4;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=31;i<=34;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=61;i<=64;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=91;i<=94;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=121;i<=124;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=151;i<=154;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=181;i<=184;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
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
      ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
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
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=35;i<=38;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
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
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=35;i<=38;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=65;i<=68;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
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
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=35;i<=38;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=65;i<=68;i++)
       ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=95;i<=98;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
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
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=35;i<=38;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=65;i<=68;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=95;i<=98;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=125;i<=128;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=155;i<=158;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=185;i<=188;i++)
        ledky[i]=CHSV(0,0,0);
      break;
    }
  case 6:
    {
      for(int i=5;i<=8;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
       for(int i=35;i<=38;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=65;i<=68;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=95;i<=98;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=125;i<=128;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=155;i<=158;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=185;i<=188;i++)
        ledky[i]=CHSV(0,0,0);
      break;
    }
  case 7:
    {
      for(int i=5;i<=8;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=35;i<=38;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=65;i<=68;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=95;i<=98;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=125;i<=128;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=155;i<=158;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=185;i<=188;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
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
      ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
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
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=39;i<=42;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
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
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=39;i<=42;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=69;i<=72;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
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
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=39;i<=42;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=69;i<=72;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=99;i<=102;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
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
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=39;i<=42;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=69;i<=72;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=99;i<=102;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=129;i<=132;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=159;i<=162;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=189;i<=192;i++)
        ledky[i]=CHSV(0,0,0);
     break;
    }
  case 6:
    {
      for(int i=9;i<=12;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=39;i<=42;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=69;i<=72;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=99;i<=102;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=129;i<=132;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=159;i<=162;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=189;i<=192;i++)
        ledky[i]=CHSV(0,0,0);
      break;
    }
  case 7:
    {
     for(int i=9;i<=12;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=39;i<=42;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=69;i<=72;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=99;i<=102;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=129;i<=132;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=159;i<=162;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=189;i<=192;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
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
       ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
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
       ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
     for(int i=43;i<=46;i++)
       ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
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
       ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
     for(int i=43;i<=46;i++)
       ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
     for(int i=73;i<=76;i++)
       ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
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
       ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
     for(int i=43;i<=46;i++)
       ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
     for(int i=73;i<=76;i++)
       ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
     for(int i=103;i<=106;i++)
       ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
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
       ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
     for(int i=43;i<=46;i++)
       ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
     for(int i=73;i<=76;i++)
       ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
     for(int i=103;i<=106;i++)
       ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
     for(int i=133;i<=136;i++)
       ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
     for(int i=163;i<=166;i++)
       ledky[i]=CHSV(0,0,0);
     for(int i=193;i<=196;i++)
       ledky[i]=CHSV(0,0,0);
     break;
    }
  case 6:
    {
     for(int i=13;i<=16;i++)
       ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
     for(int i=43;i<=46;i++)
       ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
     for(int i=73;i<=76;i++)
       ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
     for(int i=103;i<=106;i++)
       ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
     for(int i=133;i<=136;i++)
       ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
     for(int i=163;i<=166;i++)
       ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
     for(int i=193;i<=196;i++)
       ledky[i]=CHSV(0,0,0);
      break;
    }
  case 7:
    {
     for(int i=13;i<=16;i++)
       ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
     for(int i=43;i<=46;i++)
       ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
     for(int i=73;i<=76;i++)
       ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
     for(int i=103;i<=106;i++)
       ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
     for(int i=133;i<=136;i++)
       ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
     for(int i=163;i<=166;i++)
       ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
     for(int i=193;i<=196;i++)
       ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
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
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
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
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=47;i<=50;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
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
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=47;i<=50;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=77;i<=80;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
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
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=47;i<=50;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=77;i<=80;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=107;i<=110;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
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
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=47;i<=50;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=77;i<=80;i++)
       ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=107;i<=110;i++)
       ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=137;i<=140;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=167;i<=170;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=197;i<=200;i++)
        ledky[i]=CHSV(0,0,0);
     break;
    }
  case 6:
    {
      for(int i=17;i<=20;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=47;i<=50;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=77;i<=80;i++)
       ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=107;i<=110;i++)
       ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=137;i<=140;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=167;i<=170;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=197;i<=200;i++)
        ledky[i]=CHSV(0,0,0);
      break;
    }
  case 7:
    {
      for(int i=17;i<=20;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=47;i<=50;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=77;i<=80;i++)
       ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=107;i<=110;i++)
       ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=137;i<=140;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=167;i<=170;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=197;i<=200;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
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
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
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
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=51;i<=54;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
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
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=51;i<=54;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=81;i<=84;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
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
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=51;i<=54;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=81;i<=84;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=111;i<=114;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
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
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=51;i<=54;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=81;i<=84;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=111;i<=114;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=141;i<=144;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=171;i<=174;i++)
        ledky[i]=CHSV(0,0,0);
      for(int i=201;i<=204;i++)
        ledky[i]=CHSV(0,0,0);
     break;
    }
  case 6:
    {
      for(int i=21;i<=24;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=51;i<=54;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=81;i<=84;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=111;i<=114;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=141;i<=144;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=171;i<=174;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=201;i<=204;i++)
        ledky[i]=CHSV(0,0,0);
      break;
    }
  case 7:
    {
      for(int i=21;i<=24;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=51;i<=54;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=81;i<=84;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=111;i<=114;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=141;i<=144;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=171;i<=174;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=201;i<=204;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
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
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
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
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=55;i<=58;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
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
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=55;i<=58;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=85;i<=88;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
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
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=55;i<=58;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=85;i<=88;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=115;i<=118;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
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
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=55;i<=58;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=85;i<=88;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=115;i<=118;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=145;i<=148;i++)
         ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=175;i<=178;i++)
         ledky[i]=CHSV(0,0,0);
       for(int i=205;i<=208;i++)
         ledky[i]=CHSV(0,0,0);
     break;
    }
  case 6:
    {
      for(int i=25;i<=28;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=55;i<=58;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=85;i<=88;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=115;i<=118;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=145;i<=148;i++)
         ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=175;i<=178;i++)
         ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
       for(int i=205;i<=208;i++)
         ledky[i]=CHSV(0,0,0);
      break;
    }

  case 7:
    {
      for(int i=25;i<=28;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=55;i<=58;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=85;i<=88;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=115;i<=118;i++)
        ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=145;i<=148;i++)
         ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
      for(int i=175;i<=178;i++)
         ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
       for(int i=205;i<=208;i++)
         ledky[i]=CHSV(barva+(barevnyIndex*i),255,jas);
     break;
    }
 }
 FastLED.show();
}
