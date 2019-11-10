FutabaUsVfd Library
=============================================================================

Original version downloaded from https://playground.arduino.cc/Main/FutabaUsVfd/

Modifications
-----
+ Updated file structure to match Arduino library specification.
+ Added library.properties.
+ Changed ``#include "WProgram.h"`` to `#include "Arduino.h"`
+ Changed `virtual void write(uint8_t character);` to `virtual size_t write(uint8_t character);`
+ Added methods:
    void setBrightness(uint8_t level);
    void setCursorModeAutoInc(); // Auto-increment cursor each time character is printed (default)
    void setCursorModeAutoDec(); // Auto-decrement cursor each time character is printed
    void setCursorModeNonInc();  // Cursor stays at same position when character is printed
    void setFlashPositions(uint8_t start, uint8_t stop); // Several ranges can be chosen with multiple calls and may overlap
    void setFlashModeEnabled();
    void setFlashModeDisabled();
    void setFlashRate50(); // 50 Hz; Effectively makes characters look 1/2 as bright as non-flashing characters
    void setFlashRate1();  // Default rate
    void setFlashRateTenth(); // 1/10 Hz
    void setFontWestern();
    void setFontKatakana();
    void setNonBufferedMode();  // Default
    void setBufferedMode();
    void printMessageBuffer();

+ Updated timing delay definitions to slightly higher than datasheet specifications to account for system clock frequency errors across a wide range of devices:
  + static const uint8_t HALF_PERIOD_TIME = 25;  // Datasheet specifies 20 us (40 us clock period, TCLOCK)
  + static const uint8_t DATA_SETUP_TIME = 5;    // Datasheet specifies 3 us (tsu)
  + static const uint8_t PROCESS_TIME = 25;      // Datasheet specifies 25 us (tprocess)
  + static const uint8_t RESET_HOLD_TIME = 50;   // Datasheet specifies 30 us (twRST). This parameter seemed to be most sensitive to specfic minimal timing.
  + static const uint16_t POST_RESET_DELAY_TIME = 2500;  // Datasheet specifies 2000 us (twait)

References
----------
+ FutabaUsVfd [library][1]
+ Futaba Model 162SD03C Vacuum Fluorescent Display [datasheet][2]


[1]: https://playground.arduino.cc/Main/FutabaUsVfd/
[2]: https://www.allelectronics.com/mas_assets/media/allelectronics2018/spec/VFD-162.pdf
