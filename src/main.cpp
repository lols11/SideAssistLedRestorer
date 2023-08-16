//Based on https://www.drive2.com/b/601314663967246021/
#include <spi.h>
#include <mcp2515.h>

#define SAYellowPin 6
#define SAGreenPin 9

const static boolean manageLEDs = true;  //Should I manage the LED for side assist?
const static boolean manageGreenOnly = true; //Should i've menage only green LED for SA?
const static boolean celebration = true;     // Should the SA LED celebration be turned on?
const static boolean showDPFPurge = true; //Should the information about DPF cleaning be displayed?
const unsigned long blinkIntervalTime = 250;    //Interval for blinking DPF light when purging

struct can_frame receivedCanMsg;
struct can_frame infoCanMsg;
struct can_frame drivetrainCanMsg;

static boolean dpfPurgeActualState = false;
static boolean dpfPurgeLastState = false;
unsigned long blinkStartTime = 0;
int brightness = 255;
int canBrightness = 0x64; // Max value for instrument cluster brightness. 0x64 = 100(%) DEC

MCP2515 mcp2515(10);

canid_t infoID= 0x03BA;         // Can ID for controlling communicats in instrument cluster (Side assist, buzzer)
canid_t brightnessID = 0x0470; // Can ID for brightness of instrument cluster
canid_t drivetrainID = 0x0480; // Can ID for drivetrain used for DPF purge info and turning DPF LED on


static void sendBeep()
{
  infoCanMsg.data[2] = infoCanMsg.data[2] | 0b00000001;
  mcp2515.sendMessage(&infoCanMsg);
}

static void showDPFPurgeInfo()
{
 
  if (!dpfPurgeActualState && !dpfPurgeLastState) //DPF Purge not active
    return;

  if (dpfPurgeActualState && !dpfPurgeLastState) //Start of DPF purge
  {
    blinkStartTime = millis();
    sendBeep();
    dpfPurgeLastState = true;
  }

  if (!dpfPurgeActualState && dpfPurgeLastState) //End of DPF purge
  {
    sendBeep();
    dpfPurgeLastState = false;
    return;
  }

  unsigned long actualTime = millis();
  if (blinkStartTime + blinkIntervalTime >= actualTime) //DPF purge active (DPF LED blinking) 
  {
    drivetrainCanMsg.data[5] = drivetrainCanMsg.data[5] | 0b00000010;
  mcp2515.sendMessage(&drivetrainCanMsg);
  }
  else if (blinkStartTime + blinkIntervalTime * 2 >= actualTime)
  blinkStartTime= millis();

  
}

void setup()
{
  ADCSRA = 0;
  pinMode(SAYellowPin, OUTPUT);
  pinMode(SAGreenPin, OUTPUT);
  digitalWrite(SAYellowPin, LOW);
  digitalWrite(SAGreenPin, LOW);
  SPI.begin();

  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ);
  mcp2515.setNormalMode();

  // Celebration
  if (celebration == true)
  {
    if (!manageGreenOnly)
    {
      digitalWrite(SAYellowPin, HIGH);
      delay(500);
    }
    digitalWrite(SAGreenPin, HIGH);
    delay(500);
    if (!manageGreenOnly)
    {
      digitalWrite(SAYellowPin, LOW);
      delay(250);
    }
    digitalWrite(SAGreenPin, LOW);
  }

}

void loop()
{

  if (mcp2515.readMessage(&receivedCanMsg) == MCP2515::ERROR_OK)
  {
    if (showDPFPurge && receivedCanMsg.can_id == drivetrainID)
    {
      if ((receivedCanMsg.data[0] & 0b100001) == 0b100001){
        dpfPurgeActualState=true;
        drivetrainCanMsg=receivedCanMsg;
    }else
        dpfPurgeActualState=false;
    
    showDPFPurgeInfo();
    }

    if (manageLEDs && receivedCanMsg.can_id == brightnessID)
    {
    canBrightness = receivedCanMsg.data[4];
    brightness = map(canBrightness, 0x1B, 0x64, 69, 255);
    return;
    }
    if (manageLEDs && receivedCanMsg.can_id == infoID)
    {
      if (receivedCanMsg.data[2] == 0x08 && !manageGreenOnly)
      {
        analogWrite(SAYellowPin, brightness);
        digitalWrite(SAGreenPin, LOW);
      }
      else if (receivedCanMsg.data[2] == 0x10)
      {
        digitalWrite(SAYellowPin, LOW);
        analogWrite(SAGreenPin, brightness);
      }
      else
      {
        if (!manageGreenOnly)
          digitalWrite(SAYellowPin, LOW);
        digitalWrite(SAGreenPin, LOW);
      }
      infoCanMsg= receivedCanMsg;
    }
  }

}


