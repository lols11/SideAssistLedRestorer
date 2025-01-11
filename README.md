# VW_SideFix-DPFNotify
![License](https://img.shields.io/static/v1?label=License&message=MIT&color=green)
> [!CAUTION]
>  WARNING! Not tested live!
>
> This software is intended for OFF-ROAD USE ONLY.


## About
Restores the green Side Assist LED functionality after updating the VW instrument cluster to 10XX SW on the PQ platform, using an Arduino and MCP2515. Additionally, it provides a optional notification for ongoing DPF regeneration by blinking the DPF LED and gong. For LED restoration, the pins must be soldered directly to the LEDs. For DPF info, only a CAN connection is needed.

## Functions
  - Restores operation of green Side Assist LED
  - Restores operation of yellow Side Assist LED
  - LED brightness set based on information from CAN
  - Information about ongoing DPF filter regeneration
  - Optional gong on start and end DPF regeneration
  - LED celebration
  - Watchdog

## Pinout:
- Green LED (+): 9
- Yellow LED (+): 6
- (Must be PWM output for proper operation.)

## TODO:
  - Bluetooth icon restore with Discover/Composition media unit (The can messages responsible for the appearance of the icon are currently publicly unknown)
  - Front assist LED restore with 7N0 radar (as above)

### Looking for additional help
I am searching for logs from the VW drivetrain CAN bus, in which there is an RNS radio, Bluetooth module, and a colorful MFA or MFA 3D display. I am interested in a log from the moment when a device is connected via Bluetooth. Unfortunately, I have Discover Media in my case, so I am unable to analyze this in terms of the Bluetooth icon. If you have such logs, I would appreciate your contact


Licensed under the MIT License. See LICENSE and NOTICE files for details.

