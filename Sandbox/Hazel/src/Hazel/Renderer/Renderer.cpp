#include "Renderer.h"

#include "RenderCommand.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Core.h"

namespace Hazel {
    Renderer::SceneDatta* Renderer::s_SceneData = new Renderer::SceneDatta;

	void Renderer::BeginScene(OrthographicCamera(& camera)) {
        s_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
    }

	void Renderer::EndScene()
	{
	}

	void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray> &vertexArray, const glm::mat4& transform) {
        shader->Bind();
        std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);
        std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_Transform", transform);
        vertexArray->Bind();
        RenderCommand::DrawIndexed(vertexArray);
    }

    void Renderer::Init() {
        RenderCommand::Init();
    }
}