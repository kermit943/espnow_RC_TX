
#include "timecallback.h"
#include <Arduino.h>

TimeCallback::TimeCallback() { this->timers.clear(); }

/**
 * @brief Handler which should be run in loop to handle all timers
 */
void TimeCallback::timerHandler() {
  unsigned long t = millis();
  for (int i = 0; i < this->timers.size(); i++) {
    this->timers.at(i).ct = t;
    if (this->timers.at(i).ct >= this->timers.at(i).ms) {
      this->timers.at(i).ms += this->timers.at(i).base_ms;
      if (this->timers.at(i).use_inline) {
        this->timers.at(i).inline_callback();
      } else {
        this->timers.at(i).callback();
      }
    }
  }
}

/**
 * @brief Add timer with function reference
 *
 * @param id p_id: Unique name
 * @param ms p_ms: Delay time in ms
 * @param callback p_callback: Referenced function
 *
 * @return bool False if name is already in use, otherwise true
 */
bool TimeCallback::addTimer(const char *id, unsigned long ms,
                            void (*callback)()) {

  /* Check if id already exist */
  if (!this->isTimerIdUnique(id)) {
    return false;
  }

  /* Build and new timer */
  timeCallbackItem newTimer;
  newTimer.id = id;
  newTimer.ct = 0;
  newTimer.ms = ms;
  newTimer.base_ms = ms;
  newTimer.callback = callback;
  newTimer.use_inline = false;
  this->timers.insert(this->timers.end(), newTimer);

  /* Everything seems fine */
  return true;
}

/**
 * @brief Add timer with inline function
 *
 * @param id p_id: Unique name
 * @param ms p_ms: Delay time in ms
 * @param callback p_callback: Inline function
 *
 * @return bool False if name is already in use, otherwise true
 */
bool TimeCallback::addInlineTimer(const char *id, unsigned long ms,
                                  InlineFunction callback) {

  /* Check if id already exist */
  if (!this->isTimerIdUnique(id)) {
    return false;
  }

  /* Build and new timer */
  timeCallbackItem newTimer;
  newTimer.id = id;
  newTimer.ct = 0;
  newTimer.ms = ms;
  newTimer.base_ms = ms;
  newTimer.inline_callback = callback;
  newTimer.use_inline = true;
  this->timers.insert(this->timers.end(), newTimer);

  /* Everything seems fine */
  return true;
}

/**
 * @brief Remove timer by name
 *
 * @param id p_id: Unique name
 *
 * @return bool True if timer was removed, otherwise false
 */
bool TimeCallback::removeTimer(const char *id) {
  for (int i = 0; i < this->timers.size(); i++) {
    if (this->timers.at(i).id == id) {
      this->timers.erase(this->timers.begin() + i);
      return true;
    }
  }
  return false;
}

/**
 * @brief Check if name is already in use
 *
 * @param id p_id: Unique name
 *
 * @return bool False name is already in use, otherwise false
 */
bool TimeCallback::isTimerIdUnique(const char *id) {
  for (int i = 0; i < this->timers.size(); i++) {
    if (this->timers.at(i).id == id) {
      return false;
    }
  }
  return true;
}
