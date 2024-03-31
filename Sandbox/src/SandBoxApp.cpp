#include <Hazel.h> 

#include "imgui.h"

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glm/ext/scalar_constants.hpp> // glm::pi

glm::mat4 camera(float Translate, glm::vec2 const& Rotate)
{
    glm::mat4 Projection = glm::perspective(glm::pi<float>() * 0.25f, 4.0f / 3.0f, 0.1f, 100.f);
    glm::mat4 View = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -Translate));
    View = glm::rotate(View, Rotate.y, glm::vec3(-1.0f, 0.0f, 0.0f));
    View = glm::rotate(View, Rotate.x, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 Model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
    return Projection * View * Model;
}

class ExampleLayer : public Hazel::Layer {
public:
  using Hazel::Layer::Layer;

  explicit ExampleLayer() : Hazel::Layer::Layer("Example") {
    auto cam = camera(1.0, glm::vec2());
    HZ_INFO("GLM test ------ {}", cam[0][0]);
  }

  ~ExampleLayer() override {
      
  }

  void OnUpdate() override{
    if (Hazel::Input::IsKeyPressed(HZ_KEY_TAB)) {
        HZ_TRACE("TAB key is pressed(poll)!");
    }
  }

  virtual void OnImGuiRender() override {
        ImGui::Begin("Test");
        ImGui::Text("Hello world!");
        ImGui::End();
    }

  void OnEvent(Hazel::Event& event) override {
    if (event.GetEventType() == Hazel::EventType::KeyPressed) {
        auto &e = dynamic_cast<Hazel::KeyPressedEvent&>(event);
        if (e.GetKeyCode() == HZ_KEY_TAB) {
            HZ_TRACE("Tab key is pressed(event)!");
        }
        HZ_TRACE("{}", (char)e.GetKeyCode());
    }
  }

    void OnAttach() override {}
    void OnDetach() override {}
}; 
  
class Sandbox : public Hazel::Application {
public:
    explicit Sandbox(){
    PushOverlay(new ExampleLayer());
}

};

Hazel::Application* Hazel::CreateApplication() {
    return new Sandbox();
}