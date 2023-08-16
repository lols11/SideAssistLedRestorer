# Side assist LED restorer
Based on: https://www.drive2.com/b/601314663967246021/
WARNING! Not tested live!

Short description:
Restore green side assist led afer update VW instrument cluster to 10XX SW in PQ platform, using Arduino and MCP2515

Functions:
  - Restores operation and manages the green Side Assist LED
  - Can also manage the yellow Side Assist LED
  - Provides information about ongoing DPF filter regeneration

I am searching for logs from the VW drivetrain CAN bus, in which there is an RNS radio, Bluetooth module, and a colorful MFA or MFA 3D display. 
I am interested in a log from the moment when a device is connected via Bluetooth. Unfortunately, I have Discover Media in my case, so I am unable to analyze this in terms of the Bluetooth icon. 
If you have such logs, I would appreciate your contact


Default pinout:
  Green LED (+): 9
  Yellow LED (+): 6
Must be PWM output for proper operation.

TODO:
  - Bluetooth icon restore with Discover/Composition media unit (The can messages responsible for the appearance of the icon are currently publicly unknown)
  - Front assist restore with 7N0 radar (as above)
