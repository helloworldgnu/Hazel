#include "WindowsWindow.h"

#include <iostream>

#include "ApplicationEvent.h"
#include "KeyEvent.h"
#include "Log.h"
#include "MouseEvent.h"
#include "Platform/OpenGL/OpenGLContext.h"
#include "Debugger/Instrumentor.h"

// #include "Assert.h"
namespace Hazel {
static bool s_GLFWInitialized = false;

static void GLFWErrorCallback(int error, const char *description) {
  HZ_CORE_ERROR("GLFW Error({0}:{1})", error, description);
}

Window *Window::Create(const WindowProps &props) {
  return new WindowsWindow(props);
}

WindowsWindow::WindowsWindow(const WindowProps &props) { 
  HZ_PROFILE_FUNCTION();

  Init(props); 
}

WindowsWindow::~WindowsWindow() noexcept { 
  HZ_PROFILE_FUNCTION();

  Shutdown(); 
}

void WindowsWindow::Init(const WindowProps &props) {
  HZ_PROFILE_FUNCTION();

  m_Data.Title = props.Title;
  m_Data.Width = props.Width;
  m_Data.Height = props.Height;

  HZ_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width,
               props.Height);

  if (!s_GLFWInitialized) {
    int success = glfwInit();
    HZ_CORE_ASSERT(success, "Could not initialize GLFW!");

    glfwSetErrorCallback(GLFWErrorCallback);
    s_GLFWInitialized = true;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  {
    HZ_PROFILE_SCOPE("glfwCreateWindow");
    m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
  }
  m_Context = new OpenGLContext(m_Window);
  m_Context->Init();

  glfwSetWindowUserPointer(m_Window, &m_Data);

  SetVSync(true);

  // set GLFW callback
  glfwSetWindowSizeCallback(
      m_Window, [](GLFWwindow *window, int width, int height) {
        WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
        data.Width = width;
        data.Height = height;

        WindowResizeEvent event(width, height);
        data.EventCallback(event);
      });

  glfwSetWindowCloseCallback(m_Window, [](GLFWwindow *window) {
    WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
    WindowCloseEvent event;
    data.EventCallback(event);
  });

  glfwSetKeyCallback(m_Window, [](GLFWwindow *window, int key, int scancode,
                                  int action, int mods) {
    WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
    switch (action) {
    case GLFW_PRESS: {
      KeyPressedEvent event(key, 0);
      data.EventCallback(event);
      break;
    }

    case GLFW_RELEASE: {
      KeyPressedEvent event(key);
      data.EventCallback(event);
      break;
    }
    case GLFW_REPEAT: {
      KeyPressedEvent event(key, 1);
      data.EventCallback(event);
      break;
    }
    }
  });

  glfwSetMouseButtonCallback(
      m_Window, [](GLFWwindow *window, int button, int action, int mods) {
        WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);

        switch (action) {
        case GLFW_PRESS: {
          MouseButtonPressedEvent event(button);
          data.EventCallback(event);
          break;
        }
        case GLFW_RELEASE: {
          MouseButtonReleasedEvent event(button);
          data.EventCallback(event);
          break;
        }
        }
      });

  glfwSetScrollCallback(
      m_Window, [](GLFWwindow *window, double xOffset, double yOffset) {
        WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
        MouseScrolledEvent event((float)xOffset, (float)yOffset);
        data.EventCallback(event);
      });

  glfwSetCursorPosCallback(
      m_Window, [](GLFWwindow *window, double xOffset, double yOffset) {
        WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
        MouseMovedEvent event((float)xOffset, (float)yOffset);
        data.EventCallback(event);
      });
}

void WindowsWindow::Shutdown() {
  HZ_PROFILE_FUNCTION();

  glfwDestroyWindow(m_Window); 
}

void WindowsWindow::OnUpdate() {
  HZ_PROFILE_FUNCTION();

  glfwPollEvents();
  m_Context->SwapBuffers();
  // glfwSwapBuffers(m_Window);
}

void WindowsWindow::SetVSync(bool enabled) {
  HZ_PROFILE_FUNCTION();
  
  if (enabled) {
    glfwSwapInterval(1);
  } else {
    glfwSwapBuffers(0);
  }
  m_Data.VSync = enabled;
}

bool WindowsWindow::IsVSync() const { return m_Data.VSync; }

void WindowsWindow::GetPixelSize(uint32_t& width, uint32_t& height) const {
    glfwGetFramebufferSize(m_Window, reinterpret_cast<int *>(&width), reinterpret_cast<int *>(&height));
}

} // namespace Hazel