#include "Renderer2D.h"

#include <chrono>

#include "imgui.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Platform/OpenGL/OpenGLShader.h"
#include "Debugger/Instrumentor.h"

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
	HZ_PROFILE_FUNCTION();

    m_CheckerboardTexture = Hazel::Texture2D::Create("../assets/textures/Checkerboard.png");
    m_CoverTexture = Hazel::Texture2D::Create("../assets/textures/Cover.jpeg");
}

void Renderer2D::OnDetach() {
	HZ_PROFILE_FUNCTION();
}

void Renderer2D::OnUpdate(Hazel::Timestep ts) {
	HZ_PROFILE_FUNCTION();

	m_CameraController.OnUpdate(ts);

    Hazel::Renderer2D::ResetStats();
    
	{
		HZ_PROFILE_SCOPE("Renderer Prep");
		// Render
		Hazel::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Hazel::RenderCommand::Clear();
	}

	{
		HZ_PROFILE_SCOPE("Renderer Draw");

        static float rotation = 0.f;
        rotation += ts * 50.0f;

        Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());
        // 旋转45°
        Hazel::Renderer2D::DrawQuad({1.0f, 0.0f}, {0.8f, 0.8f}, -45, {0.8f, 0.2f, 0.3f, 1.0f});

        // 静态矩形
        Hazel::Renderer2D::DrawQuad({-1.0f, 0.0f}, {0.8f, 0.8f}, {0.8f, 0.2f, 0.3f, 1.0f});
        Hazel::Renderer2D::DrawQuad({0.5f, -0.5f}, {0.5f, 0.75f}, {0.2f, 0.3f, 0.8f, 1.0f});
        Hazel::Renderer2D::DrawQuad({0.0f, 0.0f, -0.1f}, {10.0f, 10.0f}, m_CheckerboardTexture, 10.0f);

        // 持续旋转45°
        Hazel::Renderer2D::DrawQuad({1.f, 0.5f, 0.1f}, {0.8f, 0.8f}, rotation, m_CoverTexture, 1.0f);
        Hazel::Renderer2D::EndScene();

        Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());
        for(float y = -5.0f; y < 5.0f; y += 0.5f) {
            for (float x = -5.0f; x < 5.0f; x += 0.5f) {
                glm::vec4 color = {(x+5.0f)/10.0f, 0.4f, (y+5.0f)/10.0f, 0.7f};
                Hazel::Renderer2D::DrawQuad({x, y}, {0.45f, 0.45f}, color);
            }
        }
        Hazel::Renderer2D::EndScene();
	}
}

void Renderer2D::OnImGuiRender() {
	HZ_PROFILE_FUNCTION();

    ImGui::Begin("Settings");
    
    auto stats = Hazel::Renderer2D::GetStats();
    ImGui::Text("Renderer2D Stats:");
    ImGui::Text("Draw Calls : %d", stats.DrawCalls);
    ImGui::Text("Quads : %d", stats.QuadCount);
    ImGui::Text("Vertices : %d", stats.GetTotalVertexCount());
    ImGui::Text("Indices : %d", stats.GetTotalIndexCount());

    ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));

    ImGui::End();
}

void Renderer2D::OnEvent(Hazel::Event &e) {
    m_CameraController.OnEvent(e);
}