# leds-controler-of-an-effect-piano
including the program runs on windows (rely on Synthesia or not) and the code runs on arduino

### v1.1 (20200522)

run independently on Windows (using portmidi.h)

Removed special lighting plans (They do not work and I don't know why.Maybe this will be available again in next version.)

Necessary Files:
latency.txt : a int refers to latency(ms)
color.txt :
  - the first line : a int refers to the number of colors
  - then each line includes 3 int refers to r , g and b in range [0,127]

### The first version:

run with Synthesia on Windows

used arduino uno and ws2812b leds

Advantages:

Great stability

Acceptable delay

run with low-price hardwares

9 colors available (change them in color.txt)

3 special lighting plans available (change them in effect_lights.cpp)
