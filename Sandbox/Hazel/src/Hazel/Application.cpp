#include "Application.h"

#include "ApplicationEvent.h"
#include "Log.h"
#include <glad/glad.h>

namespace Hazel {
Application::Application() {
  m_Window = std::unique_ptr<Window>(Window::Create());
}

Application::~Application() {}

void Application::Run() {
  WindowResizeEvent e(1200, 720);
  if (e.IsInCategory(EventCategoryApplication)) {
    HZ_TRACE(e.ToString());
  }

  if (e.IsInCategory(EventCategoryInput)) {
    HZ_TRACE(e.ToString());
  }

  while (m_Running) {
    glClearColor(0, 1, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    m_Window->OnUpdate();
  }
}

} // namespace Hazel
