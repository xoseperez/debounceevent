/*

  Debounce buttons and trigger events
  Copyright (C) 2015-2018 by Xose Pérez <xose dot perez at gmail dot com>

  The DebounceEvent library is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  The DebounceEvent library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with the DebounceEvent library.  If not, see <http://www.gnu.org/licenses/>.


*/

#include <Arduino.h>
#include "DebounceEvent.h"

DebounceEvent::DebounceEvent(uint8_t pin, DEBOUNCE_EVENT_CALLBACK_SIGNATURE, uint8_t mode, unsigned long delay, unsigned long repeat) {
    this->callback = callback;
    _init(pin, mode, delay, repeat);
}

DebounceEvent::DebounceEvent(uint8_t pin, uint8_t mode, unsigned long delay, unsigned long repeat) {
    this->callback = NULL;
    _init(pin, mode, delay, repeat);
}


void DebounceEvent::_init(uint8_t pin, uint8_t mode, unsigned long delay, unsigned long repeat) {

    // store configuration
    _pin = pin;
    _mode = mode & 0x01;
    _defaultStatus = ((mode & BUTTON_DEFAULT_HIGH) > 0);
    _delay = delay;
    _repeat = repeat;

    // set up button
    #if ESP8266
    if (_pin == 16) {
        if (_defaultStatus) {
            pinMode(_pin, INPUT);
        } else {
            pinMode(_pin, INPUT_PULLDOWN_16);
        }
    } else {
    #endif // ESP8266
        if ((mode & BUTTON_SET_PULLUP) > 0) {
            pinMode(_pin, INPUT_PULLUP);
        } else {
            pinMode(_pin, INPUT);
        }
    #if ESP8266
    }
    #endif // ESP8266

    _status = (_mode == BUTTON_SWITCH) ? digitalRead(_pin) : _defaultStatus;

}

unsigned char DebounceEvent::loop() {

    unsigned char event = EVENT_NONE;

    if (digitalRead(_pin) != _status) {

        // Debounce
        unsigned long start = millis();
        while (millis() - start < _delay) delay(1);

        if (digitalRead(_pin) != _status) {

            _status = !_status;

            if (_mode == BUTTON_SWITCH) {

                event = EVENT_CHANGED;

            } else {

                // released
                if (_status == _defaultStatus) {

                    _event_length = millis() - _event_start;
                    _ready = true;

                // pressed
                } else {

                    event = EVENT_PRESSED;
                    _event_start = millis();
                    _event_length = 0;
                    if (_reset_count) {
                        _event_count = 1;
                        _reset_count = false;
                    } else {
                        ++_event_count;
                    }
                    _ready = false;

                }

            }

        }
    }

    if (_ready && (millis() - _event_start > _repeat)) {
        _ready = false;
        _reset_count = true;
        event = EVENT_RELEASED;
    }

    if (event != EVENT_NONE) {
        if (this->callback) this->callback(_pin, event, _event_count, _event_length);
    }

    return event;

}
