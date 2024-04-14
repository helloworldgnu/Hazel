#include "Application.h"

#include <functional>
#include <iostream>
#include <memory>

#include "ApplicationEvent.h"
#include "Core/Base.h"
#include "Core/Timestep.h"
#include "Renderer/Shader.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderCommand.h"
#include "Debugger/Instrumentor.h"

namespace Hazel {
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

Application* Application::s_Instance = nullptr;

Application::Application() {
  HZ_PROFILE_FUNCTION();
  HZ_CORE_ASSERT(!s_Instance, "Application already exists!");

  s_Instance = this;
  m_Window = Scope<Window>(Window::Create());
  // 绑定类成员函数，需要传this
//        Window::EventCallbackFn fn = std::bind(&Application::OnEvent, this, std::placeholders::_1);
//        m_Window->SetEventCallback(fn);
  m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

  Renderer::Init();
  
  m_ImGuiLayer = new ImGuiLayer();
  PushOverlay(m_ImGuiLayer);
}

Application::~Application() {
  HZ_PROFILE_FUNCTION();
  Renderer::Shutdown();
}

void Application::PushLayer(Layer *layer) {
  HZ_PROFILE_FUNCTION();

  m_LayerStack.PushLayer(layer);
  layer->OnAttach();
}

void Application::PushOverlay(Layer *layer) {
    m_LayerStack.PushOverlay(layer);
    layer->OnAttach();
}

void Application::OnEvent(Hazel::Event &e) {
  HZ_PROFILE_FUNCTION();
  EventDispatcher dispatcher(e);
  dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
  dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));

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

bool Application::OnWindowResize(Hazel::WindowResizeEvent &e) {
  HZ_PROFILE_FUNCTION();
  if (e.GetWidth() == 0 || e.GetHeight() == 0) {
    m_Minimized = true;
    return false;
  }
  m_Minimized = false;

  uint32_t pixelWidth = e.GetWidth(), pixelHeight = e.GetHeight();
  m_Window->GetPixelSize(pixelWidth, pixelHeight);
  HZ_INFO("width = {0}, height = {1}, pixelWidth = {2}, pixelHeight = {3}", e.GetWidth(), e.GetHeight(), pixelWidth, pixelHeight);
  Renderer::OnWindowResize(pixelWidth, pixelHeight);
  return false;
}

void Application::Run() {
  HZ_PROFILE_FUNCTION();
  while (m_Running) {
    HZ_PROFILE_SCOPE("RunLoop");
    float time = glfwGetTime();
    Timestep timestep = time - m_LastFrameTime;
    m_LastFrameTime = time;

    if (!m_Minimized) {
      {
          HZ_PROFILE_SCOPE("LayerStack OnUpdate");
          for (Layer *layer: m_LayerStack) {
              layer->OnUpdate(timestep);
          }
      }

      m_ImGuiLayer->Begin();
      {
            HZ_PROFILE_SCOPE("LayerStack OnImGuiRender");
            for (Layer *layer: m_LayerStack) {
                layer->OnImGuiRender();
            }
      }
      m_ImGuiLayer->End();
    }

    m_Window->OnUpdate();
  }
}

} // namespace Hazel
