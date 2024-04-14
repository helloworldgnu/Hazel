#include "Renderer.h"

#include "RenderCommand.h"

namespace Hazel {
    Renderer::SceneDatta* Renderer::m_SceneData = new Renderer::SceneDatta;

	void Renderer::BeginScene(OrthographicCamera(& camera)) {
        m_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
    }

	void Renderer::EndScene()
	{
	}

	void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray> &vertexArray, const glm::mat4& transform) {
        shader->Bind();
        shader->UploadUniformMat4("u_ViewProjection", m_SceneData->ViewProjectionMatrix);
        shader->UploadUniformMat4("u_Transform", transform);
        vertexArray->Bind();
        RenderCommand::DrawIndexed(vertexArray);
    }

}