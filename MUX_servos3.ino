// Servo Multiplexer
// RK 15.12.2019  V3
// Umschaltung ohne Signal dauert länger - ist egal, weil dann sowieso was gröber schief läuft
// Interrupts

//#define USE_SER

#if defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
#define ATTINY
#endif

#ifdef ATTINY
 // ATTiny
  #define empfaengerPin 2 // Empfänger Signalleitung 
  #define Kanal1 1 // Ausgang 1 xS
  #define Kanal2 3 // Ausgang 1 Sx
  #define LED 0
  #define TEST 4
#else
// ATMega - Arduino
  #define empfaengerPin 2 // Empfänger Signalleitung 
  #define Kanal1 3 // Ausgang 1 xS
  #define Kanal2 4 // Ausgang 1 Sx
  #define LED 5
  #define TEST 6
#endif

#define MM 0
#define MS 1
#define SS 3
#define SM 2

// 1000us unten  2000us oben  5 Teile -> 200
// 1000-1199 1200-1399 1400-1600 
//    SS        MS         MM
// 2000-1801 1800-1601 1600-1400 
//    SS        SM         MM 

#define time0 1199
#define time1 1399
#define time2 1600
#define time3 1800

int pulslaenge; // Pulslänge in Mikrosekunden
int mode=0,newmode=0,newmodelast=0,modecnt=0;
bool test_value=false;
volatile int pwm_value = 0;
volatile int prev_time = 0;

void setup() 
{
  pinMode(empfaengerPin, INPUT);
  attachInterrupt(0, rising, RISING);
  
  pinMode(Kanal1, OUTPUT);
  pinMode(Kanal2, OUTPUT);
  pinMode(TEST, OUTPUT);
  digitalWrite(Kanal1, 0);
  digitalWrite(Kanal2, 0);
  digitalWrite(TEST, 0);
  
  pinMode(LED, OUTPUT);
  digitalWrite(LED, 0);
#ifdef USE_SER
  Serial.begin(115200);
#endif
}

void rising() 
{
  attachInterrupt(0, falling, FALLING);
  prev_time = micros();
}
 
void falling() 
{
  attachInterrupt(0, rising, RISING);
  pwm_value = micros()-prev_time;
}

void loop() 
{
  bool nosignal=false;

  pulslaenge = pwm_value;
  pwm_value=0;  // geht weil es länger dauert und so immer ein neuer PWM Wert da sein muß, oder eben kein Signal
  
  if (pulslaenge<800 or pulslaenge>2100 /*or (waiting4falling and (micros()-prev_time > 2200))*/)  // kein Signal -> 0 oder Unterbrechung und sehr langer Puls  
  {
    newmode=MM;  // default - no signal
    digitalWrite(LED,0);
    delay(80);    
    nosignal=true;
  }
  else if (pulslaenge>time3) newmode=SS;
  else if (pulslaenge>time2) newmode=SM;
  else if (pulslaenge>time1) newmode=MM;
  else if (pulslaenge>time0) newmode=MS;
  else                       newmode=SS;

  if (mode != newmode)    // es gibt eine Änderung gegenüber dem eingestellten Wert
  {
    if (newmode != newmodelast)   // der neue Wert hat sich geändert entweder am Start der Änderung oder mittendrin
    {
      modecnt=0;                  // auf Stabilisierung warten
      newmodelast = newmode;
    }
    else
    {
      modecnt++;
      if (modecnt>=5)             // der neue Wert steht lang genug an
      {
        mode=newmode;             // übernehmen
        newmodelast=mode;         // für die nächste Änderung
        modecnt=0;
      }
    }
  }
  else
  {
    modecnt=0;                    // bei kurzen Fehlern nachher zurücksetzen, damit es durchgehend sein muß 
  }

  // immer setzen oder nur bei Änderung ??? Solange keine Spikes auftreten egal
  digitalWrite(Kanal1, mode & 0x01);
  digitalWrite(Kanal2, mode & 0x02);

 #ifdef USE_SER
  Serial.print(pulslaenge);
  Serial.print(" --- ");
  Serial.print(newmode);
  Serial.print(" - ");
  Serial.print(newmodelast);
  Serial.print(" - ");
  Serial.print(modecnt);
  Serial.print(" -> ");
  Serial.println(mode);
 #endif

 digitalWrite(LED,1);  
  delay(20);
  if (!nosignal)
    delay(80);
  
  // für Test der Taktfrequenz  
  test_value = !test_value;
  digitalWrite(TEST, test_value);
}
