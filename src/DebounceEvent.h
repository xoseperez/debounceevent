/*

  Debounce buttons and trigger events
  Detects capacitive and resistive touch
  Copyright (C) 2015-2018 by Xose Pérez <xose dot perez at gmail dot com>
  Conttributors: Arjan Mels

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

#define BUTTON_BUTTONTYPE_MASK  (1+8)
#define BUTTON_PUSHBUTTON       0
#define BUTTON_SWITCH           1
#define BUTTON_DEFAULT_HIGH     2
#define BUTTON_SET_PULLUP       4
#define BUTTON_TOUCHBUTTON      8

#define DEBOUNCE_DELAY          50
#define REPEAT_DELAY            500
#define TOUCH_TIMEOUT           1000 // in microseconds

#define TOUCH_DIVIDER           10000   // DIVIDER for teh following alpha (exponential weight values)
#define TOUCH_SHORT_ALPHA       1000    // exponential weight for short term average
#define TOUCH_LONG_ALPHA        1       // exponential weight for long term average 
#define TOUCH_VAR_ALPHA         1       // exponential weight for long term variance 

#define TOUCH_SLOWDOWN          10      // factor to slowdown averaging when touched
#define TOUCH_SENSITIVITY       6       // threshold for detection in x standard deviations from the long term average

#define EVENT_NONE              0
#define EVENT_CHANGED           1
#define EVENT_PRESSED           2
#define EVENT_RELEASED          3

class DebounceEvent {

    public:

        DebounceEvent(uint8_t pin, DEBOUNCE_EVENT_CALLBACK_SIGNATURE, uint8_t mode = BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH, unsigned long delay = DEBOUNCE_DELAY, unsigned long repeat = REPEAT_DELAY, unsigned int touch_timeout = TOUCH_TIMEOUT, unsigned int touch_sensitivity = TOUCH_SENSITIVITY);
        DebounceEvent(uint8_t pin, uint8_t mode = BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH, unsigned long delay = DEBOUNCE_DELAY, unsigned long repeat = REPEAT_DELAY, unsigned int touch_timeout = TOUCH_TIMEOUT, unsigned int touch_sensitivity = TOUCH_SENSITIVITY);
        unsigned char loop();
        bool pressed() { return (_status != _defaultStatus); }
        unsigned long getEventLength() { return _event_length; }
        unsigned long getEventCount() { return _event_count; }
        void setTouchSensitivity(int sensitivity) { _touch_sensitivity = sensitivity; }

    private:

        uint8_t _pin;
        uint8_t _mode;
        unsigned int _touch_timeout;
        bool _status;
        bool _ready = false;
        bool _reset_count = true;
        unsigned long _last_time;
        unsigned long _debounce_start;
        bool _debounce_status;
        unsigned long _event_start;
        unsigned long _event_length;
        unsigned char _event_count;
        bool _defaultStatus;
        unsigned long _delay;
        unsigned long _repeat;
        
        int _touch_alpha;
        unsigned long _touch_short_avg;
        unsigned long _touch_long_avg;
        unsigned long long _touch_long_var;
        bool _touch_prev_touched;
        uint16_t _touch_maxcount; 

        unsigned int _touch_sensitivity; 

        DEBOUNCE_EVENT_CALLBACK_SIGNATURE;

        void _init(uint8_t pin, uint8_t mode, unsigned long delay, unsigned long repeat, unsigned int touch_timeout, unsigned int touch_sensitivity);
        bool _readPin();
        unsigned long _readTouch();
        void _calibrateMaxCount();

};

#endif
