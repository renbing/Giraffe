//
//  RenderContextGL2.cpp
//  Giraffe
//
//  Created by 思诚 李 on 2/21/12.
//  Copyright (c) 2012 Perfect World. All rights reserved.
//

#include "RenderContextGL2.h"
#include "MovieClip.h"

const char *defaultVertShader =
#include "default.vert.h"

const char *fillFragShader =
#include "fill.frag.h"

const char *textureFragShader =
#include "texture.frag.h"

RenderContextGL2 * RenderContextGL2::m_instance = NULL;

RenderContextGL2::RenderContextGL2()
    : m_width(100),m_height(100)
{
    m_programs[TEXTURE_PROGRAM] = buildTexProgram();
    m_programs[FILLRECT_PROGRAM] = buildFillProgram();
    
    applyOrtho(0, m_width, 0, m_height);

    m_matrixStackIndex = 0;
    esLoadIdentity(&m_matrixStack[0]);
    esMatrixCopy(&m_tsrMatrix, &m_matrixStack[0]);
    m_alphaStack[0] = 1.0;
    
    switchToProgram(TEXTURE_PROGRAM);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
}

RenderContextGL2::~RenderContextGL2()
{
}

void RenderContextGL2::layout(float width, float height)
{
    m_width = width;
    m_height = height;
    LOG("layout with widthInPixels=%f, heightInPixels=%f", m_width, m_height)
    
    applyOrtho(0, m_width, 0, m_height);
    switchToProgram(TEXTURE_PROGRAM);
}

void RenderContextGL2::fillRect(const Color4f &color, float x, float y, float w, float h)
{
    LOG("RenderContextGL2 fillRect with x=%f, y=%f, w=%f, h=%f", x, y, w, h)
    
    if (m_program != FILLRECT_PROGRAM) {
        switchToProgram(FILLRECT_PROGRAM);
    }
    
    GLfloat vertices[8];
    
    vertices[0] = x;
    vertices[1] = y;
    vertices[2] = vertices[0];
    vertices[3] = y+h;
    vertices[4] = x+w;
    vertices[5] = vertices[3];
    vertices[6] = vertices[4];
    vertices[7] = vertices[1];
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    
    float alpha = m_alphaStack[m_matrixStackIndex];
    GLfloat nfc[] = {color.r * alpha, color.g * alpha, color.b * alpha, color.a * alpha};
    glUniform4fv(m_uniforms[FILLRECT_PROGRAM].FILLCOLOR, 1, nfc);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glVertexAttribPointer(m_attributes[FILLRECT_PROGRAM].POSITION, 2, GL_FLOAT, GL_FALSE, 0, vertices);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void RenderContextGL2::drawRect(const Image *image, GLuint vbo)
{
    if (m_program != TEXTURE_PROGRAM) {
        switchToProgram(TEXTURE_PROGRAM);
    }
    
    if( !image ) {
        ERROR("RenderContextGL2 drawRect NULL Image")
        return;
    }
    
    if( image->hasAlpha ) {
        if( image->premultiAlpha ) {
            glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        }else {
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
    }else {
        glDisable(GL_BLEND);
    }
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, image->getTexture());
    glUniform1i(m_uniforms[TEXTURE_PROGRAM].SAMPLER, 0);
    glUniform1f(m_uniforms[TEXTURE_PROGRAM].ALPHA, m_alphaStack[m_matrixStackIndex]);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    //glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
    glVertexAttribPointer(m_attributes[TEXTURE_PROGRAM].POSITION, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
    glVertexAttribPointer(m_attributes[TEXTURE_PROGRAM].TEXTURE_COORD, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)32);
    
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}


GLuint RenderContextGL2::buildShader(const char *shaderSource, GLenum shaderType)
{
    GLuint shaderHandler = glCreateShader(shaderType);
    glShaderSource(shaderHandler, 1, &shaderSource, 0);
    glCompileShader(shaderHandler);
    
    GLint compileSuccess;
    glGetShaderiv(shaderHandler, GL_COMPILE_STATUS, &compileSuccess);
    if (compileSuccess == GL_FALSE) {
        ERROR("compileShaderError with source=%s", shaderSource)
        exit(1);
    }
    return shaderHandler;
}

GLuint RenderContextGL2::buildProgram(const char *vertexShaderSource, const char *fragmentShaderSource)
{
    GLuint vertexShader = buildShader(vertexShaderSource, GL_VERTEX_SHADER);
    GLuint fragmentShader = buildShader(fragmentShaderSource, GL_FRAGMENT_SHADER);
    
    GLuint programHandler = glCreateProgram();
    glAttachShader(programHandler, vertexShader);
    glAttachShader(programHandler, fragmentShader);
    glLinkProgram(programHandler);
    
    GLint linkSuccess;
    glGetProgramiv(programHandler, GL_LINK_STATUS, &linkSuccess);
    if( linkSuccess == GL_FALSE ) {
        ERROR("glLinkError in buildProgram!!!");
        exit(1);
    }
    
    if( vertexShader ) {
        glDeleteShader(vertexShader);
    }
    if( fragmentShader ) {
        glDeleteShader(fragmentShader);
    }
    
    return programHandler;
}

GLuint RenderContextGL2::buildTexProgram()
{
    GLuint programHandler = buildProgram(defaultVertShader, textureFragShader);
    
    UNIFORM &uniform = m_uniforms[TEXTURE_PROGRAM];
    
    uniform.TSR_MATRIX = glGetUniformLocation(programHandler, "TSRMatrix");
    uniform.PROJECTION = glGetUniformLocation(programHandler, "Projection");
    uniform.ALPHA = glGetUniformLocation(programHandler, "Alpha");
    uniform.SAMPLER = glGetUniformLocation(programHandler, "Sampler");
    
    uniform.FILLCOLOR = 0;
    uniform.TEXTCOLOR = 0;
   
    ATTRIBUTE &attribute = m_attributes[TEXTURE_PROGRAM];
    
    attribute.POSITION = glGetAttribLocation(programHandler, "Position");
    attribute.TEXTURE_COORD = glGetAttribLocation(programHandler, "TextureCoord");
    
    return programHandler;
}

GLuint RenderContextGL2::buildFillProgram()
{
    GLuint programHandler = buildProgram(defaultVertShader, fillFragShader);
    
    UNIFORM &uniform = m_uniforms[FILLRECT_PROGRAM];
    
    uniform.TSR_MATRIX = glGetUniformLocation(programHandler, "TSRMatrix");
    uniform.PROJECTION = glGetUniformLocation(programHandler, "Projection");
    
    uniform.FILLCOLOR = glGetUniformLocation(programHandler, "FillColor");
    uniform.TEXTCOLOR = 0;
    
    ATTRIBUTE &attribute = m_attributes[FILLRECT_PROGRAM];
    attribute.POSITION = glGetAttribLocation(programHandler, "Position");
    attribute.TEXTURE_COORD = glGetAttribLocation(programHandler, "TextureCoord");
    
    return programHandler;
}

void RenderContextGL2::applyOrtho(float left, float right, float bottom, float top)
{
    float a1 = 2.0f/(right-left);
    float b1 = 2.0f/(top-bottom);
    float tx = - (right+left)/(right-left);
    float ty = - (top+bottom)/(top-bottom);
    
    glViewport(0, 0, (right-left), (top-bottom));
    esLoadIdentity(&m_projectionMatrix);
    m_projectionMatrix.a = a1;
    m_projectionMatrix.d = b1;
    m_projectionMatrix.tx = tx;
    m_projectionMatrix.ty = ty;
}

void RenderContextGL2::switchToProgram(PROGRAM_STATE program)
{
    m_program = program;
    UNIFORM &uniform = m_uniforms[program];
    ATTRIBUTE &attribute = m_attributes[program];
    
    glUseProgram(m_programs[program]);
    glUniformMatrix4fv(uniform.PROJECTION, 1, 0, (GLfloat *)&m_projectionMatrix);
    applyTSRMatrix();
    
    glEnableVertexAttribArray(attribute.POSITION);
    if(program == FILLRECT_PROGRAM) {
        glDisableVertexAttribArray(attribute.TEXTURE_COORD);
    }
    else {
        glEnableVertexAttribArray(attribute.TEXTURE_COORD);
    }
}
