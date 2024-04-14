#include "Renderer2D.h"

#include <chrono>

#include "imgui.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Platform/OpenGL/OpenGLShader.h"

template<typename Fn>
class Timer
{
public:
    Timer(const char* name, Fn&& func) : m_Name(name), m_Func(func), m_Stopped(false)
    {
        m_StartTimepoint = std::chrono::high_resolution_clock::now();
    }

    ~Timer()
    {
        if(!m_Stopped) {
            Stop();
        }
    }

    void Stop() {
        auto endTimPoint = std::chrono::high_resolution_clock::now();
        long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
        long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimPoint).time_since_epoch().count();

        m_Stopped = true;

        float duration = (end - start) * 0.001f;
        m_Func({m_Name, duration});
    }

private:
    const char* m_Name;
    Fn m_Func;
    std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
    bool m_Stopped;
};

#define PROFILE_SCOPE(name) Timer timer##__LINE__(name, [&](ProfileResult profileResult) { m_ProfileResults.push_back(profileResult); })

Renderer2D::Renderer2D() : Layer("Renderer2D"), m_CameraController(1280.0f / 720.0f, true) {
}

void Renderer2D::OnAttach() {
    m_CheckerboardTexture = Hazel::Texture2D::Create("../assets/textures/Checkerboard.png");
}

void Renderer2D::OnDetach() {
}

void Renderer2D::OnUpdate(Hazel::Timestep ts) {
	{
		PROFILE_SCOPE("Sandbox2D::OnUpdate");
		// Update
		m_CameraController.OnUpdate(ts);
	}
    
	{
		PROFILE_SCOPE("Renderer Draw");
		// Render
		Hazel::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Hazel::RenderCommand::Clear();

		Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());
	
		Hazel::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
		Hazel::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, { 0.2f, 0.3f, 0.8f, 1.0f });
		Hazel::Renderer2D::DrawQuad({0.0f, 0.0f, -0.1f}, {5.f, 5.f}, m_CheckerboardTexture);
		Hazel::Renderer2D::EndScene();
	}
}

void Renderer2D::OnImGuiRender() {
    ImGui::Begin("Settings");
    ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));

    for (auto& result : m_ProfileResults) {
        char label[50];
        strcpy(label, "%.3fms ");
        strcat(label, result.Name);
        ImGui::Text(label, result.Time);
    }

    m_ProfileResults.clear();

    ImGui::End();
}

void Renderer2D::OnEvent(Hazel::Event &e) {
    m_CameraController.OnEvent(e);
}