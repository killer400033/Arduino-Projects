### A Collection of my most notable Arduino projects

# DAC

Platform: Atmega328p/Atmega168p

This was a simple, fast DAC I made to convert a PWM signal into the "equivalent" analog signal. It worked by using Timer 1 in input capture mode to capture the PWM signal pulse width, and then write the corresponding values to 8 digital out pins connected to a resistor ladder, making this an 8-bit DAC. The only problem with this was whenever the PWM signal was at 0% or 100%, resulting in no edges for Timer 1 to capture. And as the whole thing is interrupt driven, this would cause the DAC to glitch/ stutter.

# ESC

Platform: Atmega328p/Atmega168p

This was my first attempt at making BLDC motor driver. It uses 6 step commutation. For the driver circuitry I used the ir2113 and some N Channel MOSFETs. With this setup I had individual outputs for both the high side and low side MOSFETS, but only 3 were PWM. Hence I implemented soft switching with the high side staying on and low side switching for any 1 of the 6 steps. The main code was interrupt-driven, where the positional hall effect sensor would cause an interrupt whenever the motor moved to the next step. But this meant the code wouldn't run if the motor wasn't moving, so a "startup sequence" was also added to get the motor moving.

# Keyboard

Platform: Seeduino

This was some code I wrote for my brother's keyboard which he had build, based on a Seeduino. I wrote a simple loop to check all the keys for presses and update accordingly. I also wrote an algorithm for some RGB effects, fully taking into account key positions for accurate positioning of the effects.

# Lathe raspberry pi and Lathe Lead Screw

Platform: Arduino DUE and Raspberry Pi

In highschool we got a Lathe, but it lacked most of the change gears in the back and so basically couldn't do any threading or power feed. Hence these 2 projects were made to motorise the lead screw. The Lathe_Lead_Screw project was written for an Arduino DUE and runs all the real-time calculations for reading an encoder position and driving the stepper motor accordingly. The Lathe_Raspberry_Pi project is the interface and communicates to the Arduino through I2C. The final product had 3 modes, metric threading, imperial threading and power feed.

# Soldering Iron Display and Soldering Iron PWM

Platform: Atmega328p/Atmega168p

This was a super simple soldering iron station station I made, with one Arduino to control the PWM output, and another to control a display. I made it by opening a 24V Power supply and hijacking the switching MOSFETs gate signal, and driving it with the Arduino instead. I implemented a super simple control loop that sort of resembles a PI loop but not really.

# TV Surround Sound Control

Platform: Atmega328p/Atmega168p

This was a TV sound system controller I made to control our "custom" sound system. It had both a physical interface (a laptop trackpad) and a wireless interface through a phone (achieved using Blynk). For the trackpad interface, I had to code some crude gesture recognition for sliding, circles and tapping. And then for feedback, there was an LED light strip/ circle made of WS2812 LEDs, which I also coded many animations for.
