A Collection of my most notable Arduino projects

#DAC
Platform: Atmega328p/Atmega168p
This was a simple, fast DAC I made to convert a PWM signal into the "equivalent" analog signal. It worked by using Timer 1 in input capture mode to capture the PWM signal pulse width, and then write the corresponding values to 8 digital out pins connected to a resistor ladder, making this an 8-bit DAC. The only problem with this was whenever the PWM signal was at 0% or 100%, resulting in no edges for Timer 1 to capture. And as the whole thing is interrupt driven, this would cause the DAC to glitch/ stutter.

#ESC
Platform: Atmega328p/Atmega168p
This was my first attempt at making BLDC motor driver. It uses 6 step commutation. For the driver circuitry I used the ir2113 and some N Channel MOSFETs. With this setup I had individual outputs for both the high side and low side MOSFETS, but only 3 were PWM. Hence I implemented soft switching with the high side staying on and low side switching for any 1 of the 6 steps. The main code was interrupt-driven, where the positional hall effect sensor would cause an interrupt whenever the motor moved to the next step. But this meant the code wouldn't run if the motor wasn't moving, so a "startup sequence" was also added to get the motor moving.
