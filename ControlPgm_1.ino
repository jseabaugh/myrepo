#define NUM_LINES  8
#define PULSE_TIME 100    // milliseconds
#define PULSE_ON   LOW
#define PULSE_OFF  HIGH
#define IOADJUST 1        //ex. if pin 2 drives relay 1 = 1

//const int ledPins[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13};
typedef struct _stRelay {
  int           iState;         // 0 = inactive, 1 = pulsing
  unsigned long ulTimerStart;   // start time for pulse
} st_Relay, *pst_Relay;
st_Relay stRelays[NUM_LINES];

// prototypes
int ProcessSerialIn(char* cSerialIn, int iSerialIndex);
void ProcessRelays();

void setup()
{
 for (int i = 0; i < NUM_LINES; i++) {
  stRelays[i].iState = 0;
  stRelays[i].ulTimerStart = 0;
  pinMode(i + 1 + IOADJUST, OUTPUT);
  digitalWrite(i + 1 + IOADJUST, PULSE_OFF);
 }
  Serial.begin(9600);
}

void loop()
{
  char cSerialIn[2];
  int iSerialInIndex = 0;
  
  while (Serial.available()) Serial.read();  // flsuh any current input
  do
  {
    if (Serial.available())
    {
      char c = Serial.read();
      if (c < ' ') {
        ProcessSerialIn(cSerialIn, iSerialInIndex);
        iSerialInIndex = 0;
      } else {
        iSerialInIndex %= 2;
        cSerialIn[iSerialInIndex++] = c;
      }
    }
    ProcessRelays();
  } while (true);
}


int ProcessSerialIn(char* pcIn, int num)
{
  int val = 0;
  for (int i = 0; i < num; i++) {
    char c = pcIn[i];
    if ((c >= '0') && (c <= '9')) {
      val = (val * 10) + (c - '0');
    } else {
      val = 0;
    }
  }
  if (val != 0) {
    if (val <= NUM_LINES) {
      digitalWrite(val + IOADJUST, PULSE_ON);
      stRelays[val - IOADJUST].iState = 1;
      stRelays[val - IOADJUST].ulTimerStart = millis();
    }//i can add spacial processing for other numbers
  }
}

void ProcessRelays()
{
  for (int i = 0; i < NUM_LINES; i++) {
    if (stRelays[i].iState == 1) {
      unsigned long now = millis();
      unsigned long endTime = stRelays[i].ulTimerStart;
      if (now < endTime) {
        now += (0xffffffff - endTime);
        endTime = 0;
      }
      if ((endTime + PULSE_TIME) < now) {
        digitalWrite(i + 1 + IOADJUST, PULSE_OFF);
        stRelays[i].iState = 0;
      }
    }
  }
}


