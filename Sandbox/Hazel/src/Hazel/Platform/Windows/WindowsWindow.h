#ifndef SANBOX_WINDOWSWINDOW_H
#define SANBOX_WINDOWSWINDOW_H

#include "Window.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Renderer/GraphicsContext.h"

namespace Hazel {
class WindowsWindow : public Window {
public:
  explicit WindowsWindow(const WindowProps &props);

  ~WindowsWindow() override;

  void OnUpdate() override;

  inline unsigned int GetWidth() const override { return m_Data.Width; }
  inline unsigned int GetHeight() const override { return m_Data.Height; }

  // Window attributes
  inline void SetEventCallback(const EventCallbackFn &callback) override {
    m_Data.EventCallback = callback;
  }

  void SetVSync(bool enabled) override;
  bool IsVSync() const override;

  GLFWwindow* GetNativeWindow() override {
    return m_Window;
  }

  void GetPixelSize(uint32_t& width, uint32_t& height) const override;

private:
  virtual void Init(const WindowProps &props);
  virtual void Shutdown();

private:
  GLFWwindow *m_Window;
  GraphicsContext *m_Context;

  struct WindowData {
    std::string Title;
    unsigned int Width, Height;
    bool VSync;
    EventCallbackFn EventCallback;
  };

  WindowData m_Data;
};
} // namespace Hazel

#endif // SANBOX_WINDOWSWINDOW_H