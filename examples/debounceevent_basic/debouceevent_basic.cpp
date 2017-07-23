/*

  Debounce buttons and trigger events
  Copyright (C) 2015-2017 by Xose Pérez <xose dot perez at gmail dot com>

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <Arduino.h>
#include <DebounceEvent.h>

#define BUTTON_PIN              0

#define CUSTOM_DEBOUNCE_DELAY   50

// Time the library waits for a second (or more) clicks
// Set to 0 to disable double clicks but get a faster response
#define CUSTOM_REPEAT_DELAY     500

DebounceEvent * button;

void setup() {
    Serial.begin(115200);
    Serial.println();
    Serial.println();
    button = new DebounceEvent(BUTTON_PIN, BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH, CUSTOM_DEBOUNCE_DELAY, CUSTOM_REPEAT_DELAY);
}

void loop() {
    if (unsigned int event = button->loop()) {
        if (event == EVENT_RELEASED) {
            Serial.print("Count : "); Serial.print(button->getEventCount());
            Serial.print(" Length: "); Serial.print(button->getEventLength());
            Serial.println();
        }
    }
}
