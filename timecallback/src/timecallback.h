
#ifndef TIMECALLBACK_H
#define TIMECALLBACK_H

#include <functional>
#include <vector>

typedef std::function<void(void)> InlineFunction;

struct timeCallbackItem {
  const char *id;
  bool use_inline;
  unsigned long ct;
  unsigned long ms;
  unsigned long base_ms;
  void (*callback)();
  InlineFunction inline_callback;
};

class TimeCallback {

public:
  TimeCallback();

  /**
   * @brief Handler which should be run in loop to handle all timers
   */
  void timerHandler();

  /**
   * @brief Add timer with function reference
   *
   * @param id p_id: Unique name
   * @param ms p_ms: Delay time in ms
   * @param callback p_callback: Referenced function
   *
   * @return bool False if name is already in use, otherwise true
   */
  bool addTimer(const char *id, long unsigned int ms, void (*callback)());

  /**
   * @brief Add timer with inline function
   *
   * @param id p_id: Unique name
   * @param ms p_ms: Delay time in ms
   * @param callback p_callback: Inline function
   *
   * @return bool False if name is already in use, otherwise true
   */
  bool addInlineTimer(const char *id, long unsigned int ms,
                      InlineFunction callback);

  /**
   * @brief Remove timer by name
   *
   * @param id p_id: Unique name
   *
   * @return bool True if timer was removed, otherwise false
   */
  bool removeTimer(const char *id);

private:
  std::vector<timeCallbackItem> timers;

  /**
   * @brief Check if name is already in use
   *
   * @param id p_id: Unique name
   *
   * @return bool False name is already in use, otherwise false
   */
  bool isTimerIdUnique(const char *id);
};

#endif // TIMECALLBACK_H
