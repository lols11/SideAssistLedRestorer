/*
    VW SideFix-DPFNotify v. 1.0

    Copyright (c) 2025 by lols11
    Contact: <contact@matbogucki.pl>
    GitHub: https://github.com/lols11

    Licensed under the MIT License.
    See LICENSE and NOTICE files for details.
*/

/*
    This project restores the functionality of the Side Assist LED
    in the instrument cluster, resolving issues caused by a software update.
    It also adds a notification feature for ongoing DPF regeneration by
    blinking the DPF LED.

    CAUTION: This software is intended for OFF-ROAD USE ONLY.
*/

#include <spi.h>
#include <mcp_can.h>
#include <avr/wdt.h>

//=======================================================  SETTINGS  =======================================================

#define WATCHDOG_TIMEOUT WDTO_500MS
#define MCP_FREQ MCP_8MHZ

#define YELLOW_LED_PIN 6
#define GREEN_LED_PIN 9

const static boolean manageLEDs = true;      // Should I manage the LED for side assist?
const static boolean manageGreenOnly = true; // Should i've menage only green LED for SA?
const static boolean celebration = true;     // Should the SA LED celebration be turned on?
const static boolean showDPFPurge = true;    // Should the information about DPF cleaning be displayed?
const static boolean DPFPurgeBuzzer = true;  // Gong on start and end of DPF cleaning
const unsigned long blinkIntervalTime = 250; // Interval for blinking DPF light when purging

const unsigned int infoID = 0x03BA;       // Can ID for controlling communicats in instrument cluster (Side assist, buzzer)
const unsigned int brightnessID = 0x0470; // Can ID for brightness of instrument cluster
const unsigned int mMotor_5_ID = 0x0480;  // Can ID for drivetrain used for DPF purge info and turning DPF LED on

//==========================================================================================================================

long unsigned int rxId;
unsigned char rxLen = 0;
unsigned char rxCanMsg[8];
unsigned char mMotor_5_msg[8];
unsigned char info_msg[8];

byte canStatus = CAN_FAILINIT;
static bool blinkState = false;
static bool dpfPurgeActualState = false;
static bool dpfPurgeLastState = false;
unsigned long blinkStartTime = 0;
int brightness = 255;
int canBrightness = 0x64; // Max value for instrument cluster brightness

MCP_CAN CAN0(10);

void reboot()
{
  wdt_enable(WDTO_15MS);
  while (1)
  {
  }
}

uint8_t calc_xor_checksum(const uint8_t *d)
{
  uint8_t checksum = 0;
  for (size_t i = 1; i < 8; i++)
  {
    checksum ^= d[i];
  }
  return checksum;
}

static void sendBeep()
{
  info_msg[2] = info_msg[2] | 0b00000001;
  CAN0.sendMsgBuf(infoID, 0, 8, info_msg);
  info_msg[2] &= ~0b00000001;
}

static void showDPFPurgeInfo()
{

  if (!dpfPurgeActualState && !dpfPurgeLastState)
    return;

  if (dpfPurgeActualState && !dpfPurgeLastState)
  {
    blinkStartTime = millis();
    blinkState = false;
    if (DPFPurgeBuzzer)
      sendBeep();
    dpfPurgeLastState = true;
  }

  if (!dpfPurgeActualState && dpfPurgeLastState)
  {
    if (DPFPurgeBuzzer)
      sendBeep();
    dpfPurgeLastState = false;
    return;
  }

  if (dpfPurgeActualState)
  {
    unsigned long actualTime = millis();

    if (actualTime - blinkStartTime >= blinkIntervalTime)
    {
      blinkStartTime = actualTime;

      blinkState = !blinkState;

      if (blinkState)
      {

        mMotor_5_msg[5] |= 0b00000010;
      }
      else
      {

        mMotor_5_msg[5] &= ~0b00000010;
      }
      mMotor_5_msg[7] = calc_xor_checksum(mMotor_5_msg);
      CAN0.sendMsgBuf(mMotor_5_ID, 0, 8, mMotor_5_msg);
    }
  }
}

void setup()
{
  wdt_enable(WATCHDOG_TIMEOUT);
  ADCSRA = 0;
  pinMode(YELLOW_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  digitalWrite(YELLOW_LED_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, LOW);
  SPI.begin();

  canStatus = CAN0.begin(MCP_STD, CAN_500KBPS, MCP_FREQ);

  if (canStatus != CAN_OK)
    reboot();

  canStatus = CAN0.setMode(MCP_NORMAL);

  if (canStatus != CAN_OK)
    reboot();

  // Celebration
  if (celebration == true)
  {
    if (!manageGreenOnly)
    {
      digitalWrite(YELLOW_LED_PIN, HIGH);
      delay(500);
    }
    digitalWrite(GREEN_LED_PIN, HIGH);
    delay(500);
    if (!manageGreenOnly)
    {
      digitalWrite(YELLOW_LED_PIN, LOW);
      delay(250);
    }
    digitalWrite(GREEN_LED_PIN, LOW);
  }
}

void loop()
{
  if (CAN0.readMsgBuf(&rxId, &rxLen, rxCanMsg) == CAN_OK)
  {

    if (showDPFPurge && (rxId == mMotor_5_ID))
    {

      if ((rxCanMsg[0] & 0b100001) == 0b100001)
      {
        dpfPurgeActualState = true;
        memcpy(mMotor_5_msg, rxCanMsg, sizeof(mMotor_5_msg));
      }
      else
      {
        dpfPurgeActualState = false;
      }

      showDPFPurgeInfo();
    }

    if (manageLEDs && (rxId == brightnessID))
    {
      canBrightness = rxCanMsg[4];
      brightness = map(canBrightness, 0x1B, 0x64, 69, 255);
      return;
    }

    if (manageLEDs && (rxId == infoID))
    {
      if ((rxCanMsg[2] == 0x08) && !manageGreenOnly)
      {
        analogWrite(YELLOW_LED_PIN, brightness);
        digitalWrite(GREEN_LED_PIN, LOW);
      }
      else if (rxCanMsg[2] == 0x10)
      {
        digitalWrite(YELLOW_LED_PIN, LOW);
        analogWrite(GREEN_LED_PIN, brightness);
      }
      else
      {
        if (!manageGreenOnly)
        {
          digitalWrite(YELLOW_LED_PIN, LOW);
        }
        digitalWrite(GREEN_LED_PIN, LOW);
      }

      memcpy(info_msg, rxCanMsg, sizeof(info_msg));
    }
  }
  wdt_reset();
}
