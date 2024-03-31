#include <Hazel.h> 

class ExampleLayer : public Hazel::Layer {
public:
    using Hazel::Layer::Layer;

  explicit ExampleLayer() : Hazel::Layer::Layer("Example") {

  }

    ~ExampleLayer() override {
        
    }

  void OnUpdate() override{
      HZ_INFO("ExampleLayer::Update");
  }

  void OnEvent(Hazel::Event& event) override {
      HZ_TRACE("ExampleLayer::OnEvent {0}", event.ToString());
  }

    void OnAttach() override {}
    void OnDetach() override {}
}; 
  
class Sandbox : public Hazel::Application {
public:
    Sandbox(){
        PushOverlay(new ExampleLayer());
    }

};

Hazel::Application* Hazel::CreateApplication() {
    return new Sandbox();
}