# Side assist LED restorer
WARNING! Not tested live!

Restore green side assist led afer update VW instrument cluster to 10XX SW in PQ platform, using Arduino and MCP2515

TODO:
  - Better brightness maping 
  - Bluetooth icon restore with Discover/Composition media unit (The can messages responsible for the appearance of the icon are currently publicly unknown)
  - Front assist restore with 7N0 radar (as above)
  - DPF Purge information (maybe)

Based on: https://www.drive2.com/b/601314663967246021/

Default pinout:
  Green LED (+): 9
  Yellow LED (+): 6
  
Must be PWM output for proper operation.
