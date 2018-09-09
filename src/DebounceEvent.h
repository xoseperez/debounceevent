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

#ifndef _DEBOUNCE_EVENT_h
#define _DEBOUNCE_EVENT_h

#ifdef ESP8266
#include <functional>
#define DEBOUNCE_EVENT_CALLBACK_SIGNATURE std::function<void(uint8_t pin, uint8_t event, uint8_t count, uint16_t length)> callback
#else
#define DEBOUNCE_EVENT_CALLBACK_SIGNATURE void (*callback)(uint8_t pin, uint8_t event, uint8_t count, uint16_t length)
#endif

#define BUTTON_PUSHBUTTON       0
#define BUTTON_SWITCH           1
#define BUTTON_DEFAULT_HIGH     2
#define BUTTON_SET_PULLUP       4

#define DEBOUNCE_DELAY          50
#define REPEAT_DELAY            500

#define EVENT_NONE              0
#define EVENT_CHANGED           1
#define EVENT_PRESSED           2
#define EVENT_RELEASED          3

class DebounceEvent {

    public:

        DebounceEvent(uint8_t pin, DEBOUNCE_EVENT_CALLBACK_SIGNATURE, uint8_t mode = BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH, unsigned long delay = DEBOUNCE_DELAY, unsigned long repeat = REPEAT_DELAY);
        DebounceEvent(uint8_t pin, uint8_t mode = BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH, unsigned long delay = DEBOUNCE_DELAY, unsigned long repeat = REPEAT_DELAY);
        unsigned char loop();
        bool pressed() { return (_status != _defaultStatus); }
        unsigned long getEventLength() { return _event_length; }
        unsigned long getEventCount() { return _event_count; }

    private:

        uint8_t _pin;
        uint8_t _mode;
        bool _status;
        bool _ready = false;
        bool _reset_count = true;
        unsigned long _event_start;
        unsigned long _event_length;
        unsigned char _event_count = 0;
        uint8_t _defaultStatus;
        unsigned long _delay;
        unsigned long _repeat;
        DEBOUNCE_EVENT_CALLBACK_SIGNATURE;

        void _init(uint8_t pin, uint8_t mode, unsigned long delay, unsigned long repeat);

};

#endif
