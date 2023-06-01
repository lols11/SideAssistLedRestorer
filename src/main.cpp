#include <spi.h>
#include <mcp2515.h>
//#include "LowPower.h" // Low power, not required
#define SAYellowPin 8
#define SAGreenPin 9

struct can_frame canMsg;
static boolean celebration = true;
static boolean manageGreenOnly = true;
MCP2515 mcp2515(10);

canid_t SAid = 0x03BA; // Can ID for Side Assist
canid_t brightnessID = 0x0470; // Can ID for brightness of instrument cluster
int brightness = 255; 
int canBrightness = 0x64; //Max value for instrument cluster brightness

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
    if (manageGreenOnly)
    {
      digitalWrite(SAYellowPin, HIGH);
      delay(500);
      }
    digitalWrite(SAGreenPin, HIGH);
    delay(500);
    if (manageGreenOnly)
    {
      digitalWrite(SAYellowPin, LOW);
      delay(250);
      }
    digitalWrite(SAGreenPin, LOW);
  }
      //============================
    }

    void loop()
    {

      if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK)
      {
    if (canMsg.can_id == brightnessID)
    {
        canBrightness = canMsg.data[4];
        brightness = map(canBrightness, 0x1B, 0x64, 69, 255);
        return;
        }

        if (canMsg.can_id == SAid)
        {
        if (canMsg.data[2] == 0x08 && !manageGreenOnly)
        {
          analogWrite(SAYellowPin, brightness);
          digitalWrite(SAGreenPin, LOW);
          }
          else if (canMsg.data[2] == 0x10)
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
        }
      }
      //LowPower.powerDown(SLEEP_30MS, ADC_OFF, BOD_OFF); // Low power, not required
    }