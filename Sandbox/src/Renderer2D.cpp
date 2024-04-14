#include "Renderer2D.h"

#include "imgui.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Platform/OpenGL/OpenGLShader.h"

Renderer2D::Renderer2D() : Layer("Renderer2D"), m_CameraController(1280.0f / 720.0f, true) {
}

void Renderer2D::OnAttach() {
    m_CheckerboardTexture = Hazel::Texture2D::Create("../assets/textures/Checkerboard.png");
}

void Renderer2D::OnDetach() {
}

void Renderer2D::OnUpdate(Hazel::Timestep ts) {
    // Update
	m_CameraController.OnUpdate(ts);
	// Render
	Hazel::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	Hazel::RenderCommand::Clear();

	Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());
  
	Hazel::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
	Hazel::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, { 0.2f, 0.3f, 0.8f, 1.0f });
	Hazel::Renderer2D::DrawQuad({0.0f, 0.0f, -0.1f}, {5.f, 5.f}, m_CheckerboardTexture);
	Hazel::Renderer2D::EndScene();
}

void Renderer2D::OnImGuiRender() {
    ImGui::Begin("Settings");
    ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
    ImGui::End();
}

void Renderer2D::OnEvent(Hazel::Event &e) {
    m_CameraController.OnEvent(e);
}