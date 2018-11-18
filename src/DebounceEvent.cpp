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

#include <Arduino.h>
#include "DebounceEvent.h"


DebounceEvent::DebounceEvent(uint8_t pin, DEBOUNCE_EVENT_CALLBACK_SIGNATURE, uint8_t mode, unsigned long delay, unsigned long repeat, unsigned int touch_timeout, unsigned int touch_sensitivity) {
    this->callback = callback;
    _init(pin, mode, delay, repeat, touch_timeout, touch_sensitivity);
}

DebounceEvent::DebounceEvent(uint8_t pin, uint8_t mode, unsigned long delay, unsigned long repeat, unsigned int touch_timeout, unsigned int touch_sensitivity) {
    this->callback = NULL;
    _init(pin, mode, delay, repeat, touch_timeout, touch_sensitivity);
}

void DebounceEvent::_calibrateMaxCount() {
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, _defaultStatus?LOW:HIGH);

    // read pin port & mask for fast access
    uint8_t pinMask = digitalPinToBitMask(_pin);
    volatile uint32_t *pinPort = portInputRegister(digitalPinToPort(_pin));

    // increase _touch_maxcount till larger then _touch_timeout
    _touch_maxcount = 1;
    while (true) {
        uint16_t count = 0;
        noInterrupts();
        unsigned long start = micros();
        while ( count < _touch_maxcount && ((*pinPort & pinMask) != 0) != _defaultStatus ) 
            count++;
        unsigned long end = micros();
        interrupts();
        if ((end-start)>_touch_timeout) {
            break;
        }
        _touch_maxcount *= 2;
    }
}

unsigned long DebounceEvent::_readTouch() {
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, _defaultStatus?LOW:HIGH);
    
    delayMicroseconds(_touch_timeout);

    uint8_t mode = (_mode & BUTTON_SET_PULLUP) ? INPUT_PULLUP : INPUT;

    // read pin port & mask for fast access
    uint8_t pinMask = digitalPinToBitMask(_pin);
    volatile uint32_t *pinPort = portInputRegister(digitalPinToPort(_pin));

    uint16_t count = 0;
    pinMode(_pin, mode);
    while ( count < _touch_maxcount && ((*pinPort & pinMask) != 0) != _defaultStatus ) 
        count++;

    return count;
}

bool DebounceEvent::_readPin() {
    if ((_mode & BUTTON_BUTTONTYPE_MASK) != BUTTON_TOUCHBUTTON) {
        return digitalRead(_pin) == HIGH;
    }
    else {
        using std::max;
        using std::min;

        // median of 3 to remove outlier peaks
        unsigned long r1=_readTouch();
        unsigned long r2=_readTouch();
        unsigned long r3=_readTouch();
        unsigned long median = max(min(r1,r2), min(max(r1,r2),r3));
        
        // scale result to get more accuraccy in averages
        median *= TOUCH_DIVIDER;

        unsigned long prev_short_avg = _touch_short_avg;

        // calculate exponentially weighted short average
        _touch_short_avg = _touch_short_avg + ((long long)median - _touch_short_avg) * max(_touch_alpha, TOUCH_SHORT_ALPHA) / TOUCH_DIVIDER;
  
        // calculate weighted delta
        long long check_delta = ((long long)_touch_short_avg - (long long)_touch_long_avg)*_touch_timeout*TOUCH_DIVIDER/(_touch_long_avg+1);

        // compare squared delta with TOUCH_STDDEV^2 x the variance (equivalent to delta versus 6 x the sdtandard deviation, or when touched with 3x standard deviation)
        // when touched lower treshhold to (TOUCH_STDDEV/2)^2
        int stddev= _touch_prev_touched ? (_touch_sensitivity/2) : _touch_sensitivity;
        bool touched = (_touch_long_var > 0) && (check_delta * check_delta > _touch_long_var*(stddev*stddev));
        _touch_prev_touched = touched;

        // slow down averaging when button is touched
        long touch_divider = touched ? TOUCH_DIVIDER*TOUCH_SLOWDOWN : TOUCH_DIVIDER;
        
        // calculate exponentially weighted long average
        _touch_long_avg = _touch_long_avg + ((long long)_touch_short_avg - _touch_long_avg) * max(_touch_alpha, TOUCH_LONG_ALPHA) / touch_divider;

        // calculate exponentially weighted relative variance of previous short average versus current
        // (on purpose include first step when prev=0, to start off with high variance)
        long long alpha = max(_touch_alpha,TOUCH_VAR_ALPHA);
        long long delta = ((long long)_touch_short_avg - prev_short_avg)*_touch_timeout*touch_divider/((_touch_short_avg+prev_short_avg)/2+1);
        long long incr = delta * delta * alpha/touch_divider;
        _touch_long_var = (_touch_long_var + incr)*(touch_divider-alpha)/touch_divider;

        // during startup alpha is bigger to allow quicker convergence to stable averages and variance
        if (_touch_alpha>0)
            _touch_alpha = _touch_alpha * 99/100;

//        Serial1.printf("L: cnt: %9lu, val: %9lu, prev: %9lu, cur: %9lu, lng: %9lu, on: %d, dlt:  %9ld, var: %9lu\n", _touch_alpha, median, prev_short_avg, _touch_short_avg, _touch_long_avg, (int)touched, (long)(check_delta), (long)sqrt(_touch_long_var*stddev*stddev));
        return touched != _defaultStatus;
    }
}

void DebounceEvent::_init(uint8_t pin, uint8_t mode, unsigned long delay, unsigned long repeat, unsigned int touch_timeout, unsigned int touch_sensitivity) {

    // store configuration
    _pin = pin;
    _mode = mode;
    _defaultStatus = (mode & BUTTON_DEFAULT_HIGH) != 0;
    _status = ((_mode & BUTTON_BUTTONTYPE_MASK) == BUTTON_SWITCH) ? _readPin() : _defaultStatus;
    _delay = delay;
    _repeat = repeat;
    _touch_timeout = touch_timeout;
    _debounce_status = false;
    _touch_long_avg = 0;
    _touch_short_avg = 0;
    _touch_long_var = 0;
    _event_count = 0;
    _last_time = 0;
    _touch_alpha = TOUCH_DIVIDER;
    _touch_prev_touched = false;
    _touch_sensitivity = touch_sensitivity;

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

    if ((_mode & BUTTON_BUTTONTYPE_MASK) == BUTTON_TOUCHBUTTON) {
        _calibrateMaxCount();
    }
}

unsigned char DebounceEvent::loop() {
    unsigned char event = EVENT_NONE;

    bool pinStatus =  _readPin();

    if (!_debounce_status && pinStatus != _status) {
        _debounce_status = true;
        _debounce_start = millis();
    }

    if (_debounce_status && millis() > _debounce_start + _delay) {
        
        _debounce_status = false;
        
        if ( pinStatus != _status ) {

            _status = !_status;

            if ((_mode & BUTTON_BUTTONTYPE_MASK) == BUTTON_SWITCH) {

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
