# Poly

## Teensy 4.0 Polysynth

This is an 8-voice polyphonic synthesizer.

![Poly](https://github.com/dallinw/Poly/blob/master/Screen%20Shot%202019-11-13%20at%208.35.47%20AM.png)

## Features
* Two oscillators with configurable waveform and volume
  * sine, reverse triangle, triangle, square, sawtooth
* Selectable pink or white noise with configurable volume
* Bitcrusher
* Vibrato with configurable rate and depth
* ADSR Envelope
* Filter with configurable resonance and frequency
* LFO with configurable rate and depth
* Pitch bend
* Detune
* Mod selector
     * Up: saturation and filter are velocity-responsive
     * Down: saturation and filter are fixed at the knob-level

## How to build PlatformIO based project

1. `Install PlatformIO Core <http://docs.platformio.org/page/core.html>`_
2. Download `development platform with examples <https://github.com/platformio/platform-teensy/archive/develop.zip>`_
3. Extract ZIP archive
4. Run these commands:

```

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
```
