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
#include "DebounceEvent.h"

DebounceEvent::DebounceEvent(uint8_t pin, TDebounceEventCallback callback, uint8_t mode, unsigned long delay) {
    _callback = callback;
    _init(pin, mode, delay);
}

DebounceEvent::DebounceEvent(uint8_t pin, uint8_t mode, unsigned long delay) {
    _init(pin, mode, delay);
}


void DebounceEvent::_init(uint8_t pin, uint8_t mode, unsigned long delay) {

    // store configuration
    _pin = pin;
    _mode = mode & 0x01;
    _status = _defaultStatus = (mode & BUTTON_DEFAULT_HIGH) > 0;
    _delay = delay;

    // set up button
    if ((mode & BUTTON_SET_PULLUP) > 0) {
        pinMode(_pin, INPUT_PULLUP);
    } else {
        pinMode(_pin, INPUT);
    }

}

bool DebounceEvent::loop() {

    // holds whether status has changed or not
    bool changed = false;
    _event = EVENT_NONE;

    if (digitalRead(_pin) != _status) {

        // Debounce
        unsigned long start = millis();
        while (millis() - start < _delay) {
            delay(1);
        }

        uint8_t newStatus = digitalRead(_pin);
        if (newStatus != _status) {

            changed = true;
            _status = newStatus;

            if (_mode == BUTTON_PUSHBUTTON) {

                _clicked = false;

                // released
                if (_status == _defaultStatus) {

                    // get event
                    if (millis() - _this_start > LONG_CLICK_DELAY) {
                        _event = EVENT_LONG_CLICK;
                    } else if (millis() - _last_start < DOUBLE_CLICK_DELAY ) {
                        _event = EVENT_DOUBLE_CLICK;
                    } else {

                        // We are not setting the event type here because we still don't
                        // know what kind of event it will be (it might be a double click).
                        // Instead we are setting the _clicked variable to check later
                        _clicked = true;
                        changed = false;

                    }

                // pressed
                } else {

                    _last_start = _this_start;
                    _this_start = millis();
                    _event = EVENT_PRESSED;

                }

            } else {
                _event = EVENT_CHANGED;
            }

        }
    }

    if (_clicked && (millis() - _this_start > DOUBLE_CLICK_DELAY) && (!changed) && (_status == _defaultStatus)) {
        _clicked = false;
        changed = true;
        _event = EVENT_SINGLE_CLICK;
    }

    if (changed && _callback) {
        if (_event != EVENT_CHANGED) _callback(_pin, EVENT_CHANGED);
        _callback(_pin, _event);
    }

    return changed;

}

bool DebounceEvent::pressed() {
    return (_status != _defaultStatus);
}

uint8_t DebounceEvent::getEvent() {
    return _event;
}
