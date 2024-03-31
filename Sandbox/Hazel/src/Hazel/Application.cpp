#include "Application.h"

#include <functional>

#include "ApplicationEvent.h"
#include "Log.h"
#include <glad/glad.h>

namespace Hazel {
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

Application* Application::s_Instance = nullptr;

Application::Application() {
  s_Instance = this;

  m_Window = std::unique_ptr<Window>(Window::Create());

  // 绑定类成员函数，需要传this
  //        Window::EventCallbackFn fn = std::bind(&Application::OnEvent, this,
  //        std::placeholders::_1); m_Window->SetEventCallback(fn);
  m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));
}

Application::~Application() {}

void Application::PushLayer(Layer *layer) {
  m_LayerStack.PushLayer(layer);
  layer->OnAttach();
}

void Application::PushOverlay(Layer *layer) {
    m_LayerStack.PushOverlay(layer);
    layer->OnAttach();
}

void Application::OnEvent(Hazel::Event &e) {
  EventDispatcher dispatcher(e);
  dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
  HZ_CORE_TRACE("{}", e.ToString());

  for (auto it = m_LayerStack.end();  it != m_LayerStack.begin();) {
      (*--it)->OnEvent(e);
      if (e.Handled) {
          break;
      }
  }
}

bool Application::OnWindowClose(WindowCloseEvent &e) {
    m_Running = false;
    return true;
  }

void Application::Run() {
  WindowResizeEvent e(1200, 720);
  if (e.IsInCategory(EventCategoryApplication)) {
    HZ_TRACE(e.ToString());
  }

  if (e.IsInCategory(EventCategoryInput)) {
    HZ_TRACE(e.ToString());
  }

  while (m_Running) {
    glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT);
    for (Layer* layer : m_LayerStack) {
        layer->OnUpdate();
    }
    m_Window->OnUpdate();
  }
}

} // namespace Hazel
