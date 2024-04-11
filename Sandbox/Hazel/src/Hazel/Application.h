#ifndef SANBOX_APPLICATION_H
#define SANBOX_APPLICATION_H

#include "Event.h"

#include <memory>

#include "ApplicationEvent.h"
#include "Window.h"
#include "Layer.h"
#include "LayerStack.h"
#include "ImGui/ImGuiLayer.h"
#include "Renderer/Shader.h"

namespace Hazel {
class Application {
public:
  Application();
  virtual ~Application();
  void Run();

  void OnEvent(Event &e);

  void PushLayer(Layer* layer);
  void PushOverlay(Layer* layer);

  inline Window& GetWindow() {
      return *m_Window;
  }
  inline static Application& Get() {
      return *s_Instance;
  }

  bool OnWindowClose(WindowCloseEvent &e);

  std::unique_ptr<Shader> m_Shader;

private:
  std::unique_ptr<Window> m_Window;
  ImGuiLayer* m_ImGuiLayer;
  bool m_Running = true;
  LayerStack m_LayerStack;

  unsigned int m_VertexArray, m_VertexBuffer, m_IndexBuffer;

  // 声明shader id
  unsigned int shaderProgram;

  static Application* s_Instance;
};

Application *CreateApplication();
} // namespace Hazel

#endif // SANBOX_APPLICATION_H
