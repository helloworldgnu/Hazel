#include <Hazel.h> 

class ExampleLayer : public Hazel::Layer {
public:
    using Hazel::Layer::Layer;

  explicit ExampleLayer() : Hazel::Layer::Layer("Example") {

  }

    ~ExampleLayer() override {
        
    }

  void OnUpdate() override{
    if (Hazel::Input::IsKeyPressed(HZ_KEY_TAB)) {
        HZ_TRACE("TAB key is pressed(poll)!");
    }
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
    PushOverlay(new Hazel::ImGuiLayer());
}

};

Hazel::Application* Hazel::CreateApplication() {
    return new Sandbox();
}