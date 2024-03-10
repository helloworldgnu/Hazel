#ifndef SANBOX_APPLICATION_H
#define SANBOX_APPLICATION_H

#include "Event.h"
#include "Window.h"
#include <memory>

namespace Hazel {
class Application {
public:
  Application();
  virtual ~Application();
  void Run();

private:
  std::unique_ptr<Window> m_Window;
  bool m_Running = true;
};

Application *CreateApplication();
} // namespace Hazel

#endif // SANBOX_APPLICATION_H
