#pragma once

#include <glm/glm.hpp>

#include "Renderer/Shader.h"

typedef unsigned int GLenum;

namespace Hazel {
    class OpenGLShader : public Shader {
    public:
        // 支持参数为文件路径的构造函数
        OpenGLShader(const std::string& filePath);
        OpenGLShader(const std::string name, const std::string& vertexSrc, const std::string& fragmentSrc);
        ~OpenGLShader();

        const std::string &GetName() const override;
        
        void Bind() const override;
        void Unbind() const override;

        void UploadUniformInt(const std::string& name, int value);

        void UploadUniformFloat(const std::string& name, float value);
        void UploadUniformFloat2(const std::string& name, const glm::vec2& value);
        void UploadUniformFloat3(const std::string& name, const glm::vec3& value);
        void UploadUniformFloat4(const std::string& name, const glm::vec4& value);

        void UploadUniformMat3(const std::string& name, const glm::mat3 value);
        void UploadUniformMat4(const std::string& name, const glm::mat4 value);

    private:
        // 读取文件到字符串
        std::string ReadFile(const std::string& filePath);
        // shader存储到map中, key为shader类别
        std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
        // 编译Shader生成Shader program
        void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);

    private:
        uint32_t m_RendererID;
        std::string m_Name;
    };
}