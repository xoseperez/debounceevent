#include <Arduino.h>
#include <ArduinoUnitTests.h>
#include <DebounceEvent.h>

const int buttonPin               = 4;
const unsigned long debounceDelay = 50;
const unsigned long repeatDelay   = 500;
DebounceEvent* button;

// Declare state and reset it for each test
GodmodeState* state = GODMODE();
unittest_setup() {
  state->reset();
}

unittest(starts_with_no_event) {
  DebounceEvent button(buttonPin,
                       BUTTON_PUSHBUTTON,
                       debounceDelay,
                       repeatDelay);
  unsigned int event;

  state->micros                = 0;
  event = button.loop();
  assertEqual(EVENT_NONE, event);
  assertEqual(0, state->micros);            // no running the clock
  assertEqual(0, button.getEventCount());
  //assertEqual(0, button.getEventLength()); uninitialized
}

unittest(debounce_waits_for_settling) {
  DebounceEvent button(buttonPin,
                       BUTTON_PUSHBUTTON,
                       debounceDelay,
                       repeatDelay);
  unsigned int event;

  state->micros                = 2000;
  state->digitalPin[buttonPin] = HIGH;      // simulate press
  event = button.loop();
  assertEqual(EVENT_PRESSED, event);
  assertEqual(2000 + (debounceDelay * 1000), state->micros);

  state->micros                = 60000;
  event = button.loop();
  assertEqual(EVENT_NONE, event);
  assertEqual(60000, state->micros);

  state->micros                = 100000;
  state->digitalPin[buttonPin] = LOW;       // simulate release
  event = button.loop();
  assertEqual(EVENT_NONE, event);
  assertEqual(100000 + (debounceDelay * 1000), state->micros);
  assertEqual(150000, state->micros);

  state->micros                = 552999;    // boundary condition, not sure why this value
  event = button.loop();
  assertEqual(EVENT_NONE, event);

  state->micros                = 553000;    // event sent 403ms after pin went low.  math???
  event = button.loop();
  assertEqual(EVENT_RELEASED, event);
  assertEqual(1, button.getEventCount());
  assertEqual(98, button.getEventLength());
}


unittest_main()
