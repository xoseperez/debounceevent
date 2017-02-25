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

#define BUTTON_PIN     0

void callback(uint8_t pin, uint8_t event, uint8_t count, uint16_t length) {
    Serial.print("Event : "); Serial.print(event);
    Serial.print(" Count : "); Serial.print(count);
    Serial.print(" Length: "); Serial.print(length);
    Serial.println();
}

DebounceEvent button = DebounceEvent(BUTTON_PIN, callback, BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH);

void setup() {
    Serial.begin(115200);
    Serial.println();
    Serial.println();
}

void loop() {
    button.loop();
}
