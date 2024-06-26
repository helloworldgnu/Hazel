#include "OpenGLShader.h"

#include <array>
#include <fstream>

#include "glad/glad.h"
#include "glm/gtc/type_ptr.hpp"

#include "Log.h"
#include "Base.h"
#include "Debugger/Instrumentor.h"

namespace Hazel{

    static GLenum ShaderTypeFromString(const std::string& type) {
        if (type == "vertex") {
            return GL_VERTEX_SHADER;
        }

        if (type == "fragment" || type == "pixel") {
            return GL_FRAGMENT_SHADER;
        }

        HZ_CORE_ASSERT(false, "Unknown shader type!");
        return 0;
    }

    std::string OpenGLShader::ReadFile(const std::string &filePath) {
        HZ_PROFILE_FUNCTION();

        std::string result;
        std::ifstream in(filePath, std::ios::in | std::ios::binary);
        if (in) {
            // 移动到文件末尾
            in.seekg(0, std::ios::end);
            // tellg()当前索引，即获取文件的二进制长度，按照字节来计算偏移
            // string.resize()按照char为单位，也是字节
            result.resize(in.tellg());
            // 移动到文件开头
            in.seekg(0, std::ios::beg);
            // 读取文件内容到result
            // 注意! &result[0]获取的是真正放字符串内容的内存地址，和std::string的地址是不一样的
            in.read(&result[0], result.size());
            in.close();
        } else {
            HZ_CORE_ERROR("Could not open file '{0}'", filePath);
        }

        return result;
    }


    std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string &source) {
        HZ_PROFILE_FUNCTION();

        std::unordered_map<GLenum, std::string> shaderSource;

        const char* typeToken = "#type";
        size_t typeTokenLength = strlen(typeToken);

        // pos： 指向"#type"开头
        // eol：指向"vertex"末尾
        // begin：指向"vertex"开头
        // nextLinePos：指向"#version"开头
        size_t pos = source.find(typeToken, 0);
        while(pos != std::string::npos) {
            size_t eol = source.find_first_of("\r\n", pos);         //#type后面的换行
            HZ_CORE_ASSERT(eol != std::string::npos, "Syntax error")    // 没有换行，则后面没有逻辑，语法错误
            size_t begin = pos + typeTokenLength + 1;   // glsl第一行代码 +1表示跳过一个空格
            std::string type = source.substr(begin, eol - begin); //
            HZ_CORE_ASSERT(ShaderTypeFromString(type), "Invalid shader type specified!")

            size_t nextLinePos = source.find_first_not_of("\r\n", eol);
            // 找到下一个"#type"
            pos = source.find(typeToken, nextLinePos);
            shaderSource[ShaderTypeFromString(type)] = source.substr(nextLinePos, pos-nextLinePos);
    //      shaderSource[ShaderTypeFromString(type)] = source.substr(nextLinePos, pos-(nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
        }
        return shaderSource;
    }

    void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string> &shaderSources) {
        HZ_PROFILE_FUNCTION();

        GLuint program = glCreateProgram();
        HZ_CORE_ASSERT(shaderSources.size() <= 2, "we only support 2 shaders for now");
        std::array<GLenum, 2> glShaderIDs;
        int glShaderIDIndex = 0;
        for (auto& kv : shaderSources) {
            GLenum type = kv.first;
            const std::string& source = kv.second;
            GLuint shader = glCreateShader(type);
            const GLchar* sourceCStr = source.c_str();
            glShaderSource(shader, 1, &sourceCStr, 0);
            glCompileShader(shader);

            GLint isCompiled = 0;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
            if (isCompiled == GL_FALSE) {
                GLint maxLength = 0;
                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
                std::vector<GLchar> infoLog(maxLength);
                glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);
                glDeleteShader(shader);

                HZ_CORE_ERROR("{0}", infoLog.data());
                HZ_CORE_ASSERT(false, "Shader compilation failure!");
                break;
            }

            glAttachShader(program, shader);
            glShaderIDs[glShaderIDIndex++] = shader;
        }

        m_RendererID = program;
        glLinkProgram(program);
        GLint isLinked = 0;
        glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
        if (isLinked == GL_FALSE) {
            GLint maxLength = 0;
            glGetProgramiv(program ,GL_INFO_LOG_LENGTH, &maxLength);
            std::vector<GLchar> infoLog(maxLength);
            glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
            glDeleteProgram(program);
            for (auto id : glShaderIDs) {
                glDeleteShader(id);
            }
            HZ_CORE_ERROR("{0}", infoLog.data());
            HZ_CORE_ASSERT(false, "Shader link failure!");
            return;
        }
        for (auto id : glShaderIDs) {
            glDetachShader(program, id);
        }
    }

     OpenGLShader::OpenGLShader(const std::string &filePath) {
        HZ_PROFILE_FUNCTION();

        std::string source = ReadFile(filePath);
        auto ShaderSources = PreProcess(source);
        Compile(ShaderSources);

        // 以文件名作为shader名字
        // Extract name from filepath
        auto lastSlash = filePath.find_last_of("/\\");
        lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
        auto lastDot = filePath.rfind('.');
        auto count = lastDot == std::string::npos ? filePath.size() - lastSlash : lastDot - lastSlash;
        m_Name = filePath.substr(lastSlash, count);
    }

    OpenGLShader::OpenGLShader(const std::string name, const std::string& vertexSrc, const std::string& fragmentSrc) : m_Name(name) {
        HZ_PROFILE_FUNCTION();

        std::unordered_map<GLenum, std::string> sources;
        sources[GL_VERTEX_SHADER] = vertexSrc;
        sources[GL_FRAGMENT_SHADER] = fragmentSrc;
        Compile(sources);
    }

    OpenGLShader::~OpenGLShader() {
        HZ_PROFILE_FUNCTION();

        glDeleteProgram(m_RendererID);
    }

    const std::string &OpenGLShader::GetName() const {
        return m_Name;
    }

    void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value) {
        HZ_PROFILE_FUNCTION();
        
        UploadUniformMat4(name, value);
    }

    void OpenGLShader::SetFloat3(const std::string &name, const glm::vec3 &value) {
        HZ_PROFILE_FUNCTION();

        UploadUniformFloat3(name, value);
    }

    void OpenGLShader::SetFloat4(const std::string &name, const glm::vec4 &value) {
        HZ_PROFILE_FUNCTION();

        UploadUniformFloat4(name, value);
    }

    void OpenGLShader::SetInt(const std::string &name, const int value) {
        HZ_PROFILE_FUNCTION();

        UploadUniformInt(name, value);
    }

    void OpenGLShader::SetIntArray(const std::string &name, int *values, uint32_t count) {
        HZ_PROFILE_FUNCTION();

        UploadUniformIntArray(name, values, count);
    }

    void OpenGLShader::SetFloat(const std::string &name, const float value) {
        HZ_PROFILE_FUNCTION();
        
        UploadUniformFloat(name, value);
    }

    void OpenGLShader::Bind() const {
        HZ_PROFILE_FUNCTION();

        glUseProgram(m_RendererID);
    }

    void OpenGLShader::Unbind() const {
        HZ_PROFILE_FUNCTION();

        glUseProgram(0);
    }

    void OpenGLShader::UploadUniformInt(const std::string &name, int value) {
        HZ_PROFILE_FUNCTION();

        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform1i(location, value);
    }

    void OpenGLShader::UploadUniformIntArray(const std::string &name, int *values, uint32_t count) {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform1iv(location, count, values);
    }

    void OpenGLShader::UploadUniformFloat(const std::string &name, float value) {
        HZ_PROFILE_FUNCTION();

        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform1f(location, value);
    }

    void OpenGLShader::UploadUniformFloat2(const std::string &name, const glm::vec2 &value) {
        HZ_PROFILE_FUNCTION();

        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform2f(location, value.x, value.y);
    }

    void OpenGLShader::UploadUniformFloat3(const std::string &name, const glm::vec3 &value) {
        HZ_PROFILE_FUNCTION();

        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform3f(location, value.x, value.y, value.z);
    }

    void OpenGLShader::UploadUniformFloat4(const std::string &name, const glm::vec4 &value) {
        HZ_PROFILE_FUNCTION();

        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform4f(location, value.x, value.y, value.z, value.w);
    }

    void OpenGLShader::UploadUniformMat3(const std::string &name, const glm::mat3 value) {
        HZ_PROFILE_FUNCTION();

        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));

    }

    void OpenGLShader::UploadUniformMat4(const std::string &name, const glm::mat4 value) {
        HZ_PROFILE_FUNCTION();

        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }
}
