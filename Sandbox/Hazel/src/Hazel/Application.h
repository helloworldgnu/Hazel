#ifndef SANBOX_APPLICATION_H
#define SANBOX_APPLICATION_H

#include "Event.h"

#include <memory>

#include "ApplicationEvent.h"
#include "Window.h"
#include "Layer.h"
#include "LayerStack.h"

namespace Hazel {
class Application {
public:
  Application();
  virtual ~Application();
  void Run();

  void OnEvent(Event &e);

  void PushLayer(Layer* layer);
  void PushOverlay(Layer* layer);

private:
  bool OnWindowClose(WindowCloseEvent &e);

private:
  std::unique_ptr<Window> m_Window;
  bool m_Running = true;
  LayerStack m_LayerStack;
};

Application *CreateApplication();
} // namespace Hazel

#endif // SANBOX_APPLICATION_H
