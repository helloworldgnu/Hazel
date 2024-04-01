#include "Application.h"

#include <functional>
#include <iostream>

#include "ApplicationEvent.h"
#include "Core/Base.h"
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
  m_ImGuiLayer = new ImGuiLayer();
  PushOverlay(m_ImGuiLayer);

  // 生成顶点数组
    glGenVertexArrays(1, &m_VertexArray);
    // 绑定顶点数组
    glBindVertexArray(m_VertexArray);
    
    // 生成顶点缓冲
    glGenBuffers(1, &m_VertexBuffer);
    // 绑定数组缓冲
    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);

    // 定义三角形的三个点
    float vertices[3 * 3] = {
            -0.5f, -0.5, 0.0,
            0.5f, -0.5f, 0.0f,
            0.0f, 0.5f, 0.0f
    };
    
    // 填充数组数据。三角形数据绑定到GPU上
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // enable第一个顶点数组，对应shader中的顶点数据
    glEnableVertexAttribArray(0);
    // 定义顶点属性为：从零开始，三组数据，浮点型，不进行归一化，每组数据为3*float(即步幅)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), nullptr);
    
    // 生成索引缓冲，本质都是缓冲和顶点缓冲没区别，存不同的数据而已
    glGenBuffers(1, &m_IndexBuffer);
    // 绑定到索引缓冲
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
    // 定义索引
    unsigned int indices[3] = {0, 1, 2};
    //将indices填充索引缓冲数据(发送到GPU)
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // build and compile our shader program
    // ------------------------------------
    // vertex shader

    const char *vertexShaderSource = "#version 330 core\n"
                                     "layout (location = 0) in vec3 aPos;\n"
                                     "void main()\n"
                                     "{\n"
                                     "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                     "}\0";
    const char *fragmentShaderSource = "#version 330 core\n"
                                       "out vec4 FragColor;\n"
                                       "void main()\n"
                                       "{\n"
                                       "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                       "}\n\0";

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
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

    glUseProgram(shaderProgram);
    glBindVertexArray(m_VertexArray);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);

    for (Layer* layer : m_LayerStack) {
        layer->OnUpdate();
    }

    m_ImGuiLayer->Begin();
    for(Layer* layer : m_LayerStack) {
        layer->OnImGuiRender();
    }
    m_ImGuiLayer->End();

    m_Window->OnUpdate();
  }
}

} // namespace Hazel
