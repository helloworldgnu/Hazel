#ifndef SANBOX_KEYEVENT_H
#define SANBOX_KEYEVENT_H

#include "Event.h"
#include "KeyCodes.h"

namespace Hazel {
class KeyEvent : public Event {
public:
  int GetKeyCode() const { return m_KeyCode; }
  EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

protected:
  KeyEvent(const int keycode) : m_KeyCode(keycode) {}
  int m_KeyCode;
};

class KeyPressedEvent : public KeyEvent {
public:
  KeyPressedEvent(const int keyCode, bool isRepeat = false)
      : KeyEvent(keyCode), m_IsRepeat(isRepeat) {}

  bool IsRepeat() const { return m_IsRepeat; }

  std::string ToString() const override {
    std::stringstream ss;
    ss << "KeyPressedEvent: " << m_KeyCode << " (repeat = " << m_IsRepeat
       << ")";
    return ss.str();
  }

  EVENT_CLASS_TYPE(KeyPressed);

private:
  bool m_IsRepeat;
};

class KeyReleaseEvent : public KeyEvent {
public:
  KeyReleaseEvent(const int keycode) : KeyEvent(keycode) {}

  std::string ToString() const override {
    std::stringstream ss;
    ss << "KeyPressedEvent : " << m_KeyCode << " (repeat = " << m_IsRepeat
       << ")";
    return ss.str();
  }

private:
  bool m_IsRepeat;
};

class KeyTypedEvent : public KeyEvent {
public:
  KeyTypedEvent(const int keycode) : KeyEvent(keycode) {}

  std::string ToString() const override {
    std::stringstream ss;
    ss << "KeyTypedEvent: " << m_KeyCode;
    return ss.str();
  }

  EVENT_CLASS_TYPE(KeyTyped);
};

} // namespace Hazel

#endif // SANBOX_KEYEVENT_H