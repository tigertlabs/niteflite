# NiteFlite
Simple Arduino sketch to create "ICAO style" navigation lights for RC aircraft using a Nano board.

Adapted from "Erics DIY Aircraft Navigation Lights Project", website: http://mkme.org,
which, according to the credits, was adapted from Harold Asbridge.

Further adapted and mashed up by Tuomas Kuosmanen <tuomas.kuosmanen@gmail.com> 
I also pushed this to github so I can keep better track of changes (and so I 
can point others to the code if needed.

## Features

  * Two fading anti-collision beacons (above and below fuselage). These are always 
    on when the system is powered.
  * Standard navigation lights: red for left wingtip, green for right wingtip,
    and a white tail light.
  * Two flashing strobe lights. Very useful with bright white leds.
  * Navigation lights and strobes can be controlled via a servo channel by using 
    a three-position switch for example. First notch turns on navigation lights,
    and the second notch turns on strobes. However this is still work in progress 
    and the code is commented out, as I have not yet tested or figured out the 
    wiring for that. See the FIXME below in the code, the servo channel should go
    to digital input 2.

## Wiring

Google for "Arduino led wiring" if you are new to this stuff like me. 
The Nano has one shared ground pin that needs to be shared with each led.
Additionally you need a resistor on the anode (+) wire of each led to avoid
burning them or the Arduino (Good explanation: http://bit.ly/1SQme3m)

```
                           ._____        __  _
    anode (+)  ------------|      \  |   |_  | \
  cathode (-)    ----------|______/  !__ !__ !_/

```

Leds have two wires, anode and cathode. Cathode has a shorter wire and usually 
the bubble lens has a flat spot on its side. Cathode is wired to the GND pin, 
and anode to the output pin driving that particular led, with the resistor in
between. I shortened the led pins and soldered the resistor directly to the 
anode and the wire into the resistor, and put a shrinkwrap over it. Then I
soldered the other wire to the cathode and put yet another shrinkwrap over the
whole thing.

```
-----.
 D12 o|
 D11 o|
 D10 o|
  D9 o+-[r]---------------+======@ Nav tail (white)
  D8 o+-[r]-------------+=|======@ Nav right (red)
  D7 o+-[r]-----------+=|=|======@ Nav left (green)
  D6 o+-[r]---------+=|=|=|======@ Beacon 2
  D5 o+-[r]-------+-|=|=|=|======@ Beacon 1
  D4 o+-[r]-----+=|=|=|=|=|======@ Strobe 2
  D3 o+-[r]---+=|=|=|=|=|=|======@ Strobe 1
  D2 o|       | | | | | | |
 GND o--------+-+-+-+-+-+-+
 RST o|
  RX o|
  TX o|
------'
```

