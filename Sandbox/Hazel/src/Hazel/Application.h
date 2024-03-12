#ifndef SANBOX_APPLICATION_H
#define SANBOX_APPLICATION_H

#include "Event.h"

#include <memory>

#include "ApplicationEvent.h"
#include "Window.h"

namespace Hazel {
class Application {
public:
  Application();
  virtual ~Application();
  void Run();

  void OnEvent(Event &e);

private:
  bool OnWindowClose(WindowCloseEvent &e);

private:
  std::unique_ptr<Window> m_Window;
  bool m_Running = true;
};

Application *CreateApplication();
} // namespace Hazel

#endif // SANBOX_APPLICATION_H
