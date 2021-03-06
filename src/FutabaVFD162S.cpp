/*
  FutabaVFD162S Library
  - Original file created 17 March 2011 (https://playground.arduino.cc/Main/FutabaUsVfd/), unknown author

  Updated by Andreas Taylor Nov 2019
  - Modifications to support latest Arduino IDE and library spec
  - Added methods for most of the control functions listed in datasheet
  - Updated timing definitions
  Updated Feb 2020 to change name to avoid conflict with existing library. 

  https://github.com/Andy4495/FutabaVFD162S

*/

#include "FutabaVFD162S.h"
#include "Arduino.h"

FutabaVFD162S::FutabaVFD162S(uint8_t clockPin, uint8_t dataPin, uint8_t resetPin)
{
  // Copy pin values.
  _clockPin = clockPin;
  _dataPin = dataPin;
  _resetPin = resetPin;

  // Init screen character dimensions to something harmless.
  _numDisplayColumns = 1;
  _numDisplayRows = 1;
  _numDisplayCharacters = _numDisplayColumns * _numDisplayRows;

  // Initialize pin states.
  digitalWrite(clockPin, HIGH);
  pinMode(clockPin, OUTPUT);

  digitalWrite(dataPin, LOW);
  pinMode(dataPin, OUTPUT);

  if (resetPin != DO_NOT_USE_PIN)
  {
    digitalWrite(resetPin, LOW);
    pinMode(resetPin, OUTPUT);
  }
}

void FutabaVFD162S::begin(uint8_t numColumns, int numRows)
{
  _numDisplayColumns = numColumns;
  _numDisplayRows = numRows;
  _numDisplayCharacters = numColumns * numRows;

  // Reset if we have a reset pin.
  if (_resetPin != DO_NOT_USE_PIN)
  {
    digitalWrite(_resetPin, HIGH);
    timingDelay(RESET_HOLD_TIME);
    digitalWrite(_resetPin, LOW);
  }

  // Wait for display to reset.
  timingDelay(POST_RESET_DELAY_TIME);

  // Set I/O mode to mode 0.
  writeCharacterDirect(SET_INPUT_OUTPUT_MODE_CHARACTER);
  writeCharacterDirect(0);

  // Initialize VFD to match default power on settings.
  // This is necessary if we don't have a reset ping since
  // the last sketch run may have changed something.
  if (_resetPin == DO_NOT_USE_PIN)
  {
    clear();
  }
}

void FutabaVFD162S::clear()
{
  writeCharacterDirect(CLEAR_ENTIRE_DISPLAY_CHARACTER);
}

void FutabaVFD162S::home()
{
  setCursor(0, 0);
}

void FutabaVFD162S::createChar(uint8_t characterIndex, uint8_t pixels[] /* 8 bytes, 1 for each row */)
{
  writeCharacterDirect(LOAD_USER_CHARACTER_CHARACTER);

  characterIndex &= NUM_CUSTOM_CHARACTERS-1; // Limit index range.
  writeCharacterDirect(characterIndex + 1);

  // We need to rotate the pixels.
  for (uint8_t testBit = 0b10000; testBit; testBit >>= 1)
  {
    uint8_t currentByte = 0b10000000;

    for (uint8_t i=0; i<CHARACTER_PIXEL_HEIGHT; i++)
    {
      currentByte >>= 1;

      if (pixels[i] & testBit)
      {
        currentByte |= 0b10000000;
      }
    }

    writeCharacterDirect(currentByte);
  }
}

void FutabaVFD162S::setCursor(uint8_t column, uint8_t row)
{
  uint8_t address = row * _numDisplayColumns + column;
  if (address >= _numDisplayCharacters)
  {
    address = 0;
  }

  writeCharacterDirect(SET_CURSOR_POSITION_CHARACTER);
  writeCharacterDirect(address + 1);
}

/*virtual*/ size_t FutabaVFD162S::write(uint8_t character)
{
  // Remap from LCD custom character locations to VFD characters.
  if (character < NUM_CUSTOM_CHARACTERS)
  {
    character += CUSTOM_CHARACTER_TRANSLATE_OFFSET;
  }
  else if (character < ' ')
  {
    // Don't print anything below space, those are control
    // codes for the display that aren't standard ASCII.
    return 0;
  }

  writeCharacterDirect(character);
  return 1;
}

void FutabaVFD162S::setBrightness(uint8_t level)
{
  writeCharacterDirect(SET_DISPLAY_BRIGHTNESS_CHARACTER);
  writeCharacterDirect(level);
}

// Cursor control
// Changes do not take affect immediately; next character will be printed to current cursor postion before new mode takes affect.
void FutabaVFD162S::setCursorModeAutoInc() // Auto-increment cursor each time character is printed (default)
{
  writeCharacterDirect(SET_CURSOR_MODE_CHARACTER);
  writeCharacterDirect(0x01);
}

void FutabaVFD162S::setCursorModeAutoDec() // Auto-decrement cursor each time character is printed
{
  writeCharacterDirect(SET_CURSOR_MODE_CHARACTER);
  writeCharacterDirect(0x02);
}

void FutabaVFD162S::setCursorModeNonInc()  // Cursor stays at same position when character is printed
{
  writeCharacterDirect(SET_CURSOR_MODE_CHARACTER);
  writeCharacterDirect(0x03);
}

// Character flashing configuration
void FutabaVFD162S::setFlashPositions(uint8_t start, uint8_t stop) // Several ranges can be chosen with multiple calls and may overlap
{
  writeCharacterDirect(SET_FLASH_POSITION_CHARACTER);
  writeCharacterDirect(start);
  writeCharacterDirect(stop);
}

void FutabaVFD162S::setFlashModeEnabled()
{
  writeCharacterDirect(FLASH_MODE_CHARACTER);
  writeCharacterDirect(0x02);
}

void FutabaVFD162S::setFlashModeDisabled()    // Default
{
  writeCharacterDirect(FLASH_MODE_CHARACTER);
  writeCharacterDirect(0x01);
}

void FutabaVFD162S::setFlashRate50() // 50 Hz - Effectively makes characters look 1/2 as bright as non-flashing characters
{
  writeCharacterDirect(SET_FLASH_RATE_CHARACTER);
  writeCharacterDirect(0x01);
}

void FutabaVFD162S::setFlashRate1()  // 1 Hz - Default rate
{
  writeCharacterDirect(SET_FLASH_RATE_CHARACTER);
  writeCharacterDirect(0x30);
}

void FutabaVFD162S::setFlashRateTenth() // 1/10 Hz
{
  writeCharacterDirect(SET_FLASH_RATE_CHARACTER);
  writeCharacterDirect(0xff);
}

// Font selection
void FutabaVFD162S::setFontWestern()         // Default
{
  writeCharacterDirect(SELECT_FONT_CHARACTER);
  writeCharacterDirect(0x01);
}

void FutabaVFD162S::setFontKatakana()
{
  writeCharacterDirect(SELECT_FONT_CHARACTER);
  writeCharacterDirect(0x02);
}

// Display buffer modes
void FutabaVFD162S::setNonBufferedMode()  // Default
{
  writeCharacterDirect(SET_BUFFERED_MODE_CHARACTER);
  writeCharacterDirect(0x01);
}

void FutabaVFD162S::setBufferedMode()
{
  writeCharacterDirect(SET_BUFFERED_MODE_CHARACTER);
  writeCharacterDirect(0x02);
}

void FutabaVFD162S::printMessageBuffer()
{
  writeCharacterDirect(PRINT_MESSAGE_BUFFER_CHARACTER);
}

void FutabaVFD162S::writeCharacterDirect(uint8_t character)
{
  for (uint8_t mask=0b10000000; mask; mask>>=1)
  {
    digitalWrite(_dataPin, (character & mask) ? HIGH : LOW);
    timingDelay(DATA_SETUP_TIME);
    digitalWrite(_clockPin, LOW);
    timingDelay(HALF_PERIOD_TIME);
    digitalWrite(_clockPin, HIGH);
    timingDelay(HALF_PERIOD_TIME - DATA_SETUP_TIME);
  }

  timingDelay(PROCESS_TIME);
}
