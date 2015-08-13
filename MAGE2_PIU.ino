#define _DEBUG

#ifdef _DEBUG
  #define DEBUG(x) x
#else
  #define DEBUG(x)
#endif

#define Trace Serial
#define XB Serial1
#define BT Serial2

bool XBMsgRx = false;
bool XBMsgStart = false;
bool BTMsgRx = false;
bool BTMsgStart = false;

void setup()
{
  DEBUG(Trace.begin(9600));
  XB.begin(9600);
  BT.begin(9600);
}

void loop()
{
  CheckXB();
  CheckBT();
  ProcessMessages();
  UpdateDisplay();
}

void CheckXB()
{
  if (XB.available() > 0)
  {
    //read bytes into packet
    //set flags
  }
}

void CheckBT()
{
  if (BT.available() > 0)
  {
    //read bytes into packet
    //set flags
  }
}

void ProcessMessages()
{
  //use packets to alter state
  //send any necessary responses
}

void UpdateDisplay()
{
  //do light stuff here
}
