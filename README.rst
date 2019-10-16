# Poly

## Teensy 4.0 Polysynth

This is an 8-voice polyphonic synthesizer.

## Features
* Two oscillators with configurable waveform and volume
  * sine, reverse triangle, triangle, sawtooth, square
* Selectable pink or white noise with configurable volume
* Saturation with velocity response
* ADSR Envelope
* Filter with resonance, frequency, and velocity response
* Vibrato with configurable rate and depth
* LFO with configurable rate and depth
* Pitch bend

The signal chain looks like this:

```
/****************************************************************************
* 
* Waveform1 -\
*             \   | combine  | 
* Waveform2 ---\  |   to     |   
*               ->|   one    | -> [sat] -> [envelope] -> [filter] -\
* Pink --------/  | inMixer  |                                      \
*             /                                                      \   | condense |
* White -----/                                                        \__|    to    |
*                                                                     /  |   one    | -> [endFilter] -> [LFO] -> lineout
*                                                                    /   | outMixer |
* ... etc                                                           /
* for all 8 --------------------> [sat] -> [envelope] -> [filter] -/
* voices                                                        ...                                      
*/   
```

## How to build PlatformIO based project

1. `Install PlatformIO Core <http://docs.platformio.org/page/core.html>`_
2. Download `development platform with examples <https://github.com/platformio/platform-teensy/archive/develop.zip>`_
3. Extract ZIP archive
4. Run these commands:

.. code-block:: bash

    # Change directory to example
    > cd platform-teensy/examples/arduino-blink

    # Build project
    > platformio run

    # Upload firmware
    > platformio run --target upload

    # Build specific environment
    > platformio run -e teensy31

    # Upload firmware for the specific environment
    > platformio run -e teensy31 --target upload

    # Clean build files
    > platformio run --target clean
