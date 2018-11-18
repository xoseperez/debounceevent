# DebounceEvent

Simple push button and toggle switch debounce detection and capacitive/resistive touch library for AVR and ESP8266 platforms under the Arduino framework that reports number of clicks and length

[![version](https://img.shields.io/badge/version-2.0.4-brightgreen.svg)](CHANGELOG.md)
[![license](https://img.shields.io/badge/license-LGPL--3.0-orange.svg)](LICENSE)
<br />
[![donate](https://img.shields.io/badge/donate-PayPal-blue.svg)](https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=xose%2eperez%40gmail%2ecom&lc=US&no_note=0&currency_code=EUR&bn=PP%2dDonationsBF%3abtn_donate_LG%2egif%3aNonHostedGuest)
[![twitter](https://img.shields.io/twitter/follow/xoseperez.svg?style=social)](https://twitter.com/intent/follow?screen_name=xoseperez)

## Features

The main features of the DebounceEvent library are:

* Supports push buttons and toggle switches
* Reports # of clicks and length (in millis) of the last click
* Allows to define internal pull-ups
* Isolation between buttons
* Create modular code using callbacks

* Handles resistive/capacitive touch button detection.

## Operation

The touch detection is based on the time it takes for a pin to return to the opposite state when switched from output to input mode.

A touch button can be implemented by simply connecting a resistor between pin and gnd or pin and vdd (set BUTTON_SET_PULLUP) and connecting a piece of conducting material to the pin. These will form an RC network, which determines the decay time. A change to R or C will be detected.

On ESP8266 it has been tested with a 2.2MOhm resistor between pin and ground and a piece of wire connected to the pin.

The routine works by taking a long term average and a short term averahe. WHen the short term average differs significantly from the long term average this is seen as a button press. To determine if the change is significant the variance of the short term averages is calculated; the detection threshold is a configurable multiplier times the variance.

## Usage

See examples.

## License

Copyright (C) 2015-2018 by Xose Pérez <xose dot perez at gmail dot com>
Contributions by Arjan Mels

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
