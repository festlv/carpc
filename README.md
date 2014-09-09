RaspberryPi based CarPC.
===========================


This repository hosts firmware and software for my RaspberryPi based CarPC
build.

* **avr-firmware/** contains firmware which is running on Atmega 328p
  microcontroller. It requires avr-gcc, avr-binutils and scons.
  Sample usage:

     ```scons upload```

  Flashes the microcontroller (assumes usbasp programmer connected).

* **linux_software/** folder contains userspace driver which passes keypresses
  from serial port to uinput. Requires python2.7, pyserial and evdev
  Sample usage:
  ```bash
     virtualenv .env
     source .env/bin/activate
     pip install -r requirements.txt
     python driver.py /dev/ttyAMA0
    ```
   Runs the input driver which expects that microcontroller is connected to
   /dev/ttyAMA0 (serial port on RasPi).

* **hardware/** contains schematics and PCB layout drawn in KiCad.


For more information, please see: http://wot.lv/category/rpi-carpc.html

```
------------------------------------------------------------------------------
Copyright (c) 2013 Reinis Veips. 

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

  1. Redistributions of source code must retain the above copyright notice,
     this list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright 
     notice, this list of conditions and the following disclaimer in 
     the documentation and/or other materials provided with the distribution.

  3. The names of the authors may not be used to endorse or promote products
     derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL JCRAFT,
INC. OR ANY CONTRIBUTORS TO THIS SOFTWARE BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
```
