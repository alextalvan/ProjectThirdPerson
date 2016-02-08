#include <glm.hpp>

#include "mge/materials/PostProcessing/PostProcess.hpp"
#include "mge/core/Texture.hpp"
#include "mge/core/Camera.hpp"
#include "mge/core/Mesh.hpp"
#include "mge/core/Renderer.hpp"
#include "mge/config.hpp"
#include <string>

//ShaderProgram* PostProcess::_shader = NULL;

PostProcess::PostProcess(std::string vertexShader, std::string fragmentShader)
{
    InitializeShader(vertexShader,fragmentShader);
}

PostProcess::~PostProcess() {}

void PostProcess::InitializeShader(std::string vertexShader, std::string fragmentShader) {
    //if (!_shader) {
        _shader = new ShaderProgram();
        _shader->addShader(GL_VERTEX_SHADER,config::MGE_SHADER_PATH + vertexShader);
        _shader->addShader(GL_FRAGMENT_SHADER,config::MGE_SHADER_PATH + fragmentShader);
        //_shader->addShader(GL_VERTEX_SHADER, config::MGE_SHADER_PATH+"PostProcessing/identity.vs");
        //_shader->addShader(GL_FRAGMENT_SHADER, config::MGE_SHADER_PATH+"PostProcessing/identity.fs");
        _shader->finalize();
    //}
}

void PostProcess::render(GLuint frameTextureID)
{
    _shader->use();

    //setup texture slot 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, frameTextureID);
    glUniform1i (_shader->getUniformLocation("frameTexture"), 0);



    GLuint vertexArray = Renderer::GetPostProcessVertexAttrib();
    GLuint vertexAttrib = _shader->getAttribLocation("vertex");

    glEnableVertexAttribArray(vertexAttrib);

    glBindBuffer(GL_ARRAY_BUFFER, vertexArray);
    glVertexAttribPointer(
    vertexAttrib,  // attribute
    2,                  // number of elements per vertex, here (x,y)
    GL_FLOAT,           // the type of each element
    GL_FALSE,           // take our values as-is
    0,                  // no extra data between each position
    0                   // offset of first element
    );
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDisableVertexAttribArray(vertexAttrib);

    glBindBuffer(GL_ARRAY_BUFFER, 0);


    //now inform mesh of where to stream its data
    //pGameObject->getMesh()->streamToOpenGL(
    //    _shader->getAttribLocation("vertex"),
    //    _shader->getAttribLocation("normal"),
    //    _shader->getAttribLocation("uv")
    //);
}
