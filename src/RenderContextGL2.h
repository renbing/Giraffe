//
//  RenderContextGL2.h
//  Giraffe
//
//  Created by 思诚 李 on 2/21/12.
//  Copyright (c) 2012 Perfect World. All rights reserved.
//

#ifndef __Giraffe__RenderContextGL2__
#define __Giraffe__RenderContextGL2__

#include <math.h>

#include "global.h"
#include "MatrixUtil.h"
#include "Image.h"

enum PROGRAM_STATE {
    TEXTURE_PROGRAM = 0,
    FILLRECT_PROGRAM,
    STRING_PROGRAM,
    PROGRAM_NUM
};

struct ATTRIBUTE {
    GLuint POSITION;
    GLuint TEXTURE_COORD;
};

struct UNIFORM {
    GLuint TSR_MATRIX;
    GLuint PROJECTION;
    GLuint ALPHA;
    GLuint SAMPLER;
    GLuint FILLCOLOR;
    GLuint TEXTCOLOR;
};

#define MATRIX_STACK_DEPTH 32

class RenderContextGL2
{
private:
    static RenderContextGL2 *m_instance;
    
    float m_width;
    float m_height;
    
    PROGRAM_STATE m_program;
    GLuint m_programs[PROGRAM_NUM];
    UNIFORM m_uniforms[PROGRAM_NUM];
    ATTRIBUTE m_attributes[PROGRAM_NUM];
    
    unsigned int m_matrixStackIndex;
    esMatrix m_matrixStack[MATRIX_STACK_DEPTH];
    esMatrix m_tsrMatrix;
    
    float m_alphaStack[MATRIX_STACK_DEPTH];
    
    esMatrix m_projectionMatrix;
    
private:
    RenderContextGL2();
    GLuint buildTexProgram();
    GLuint buildFillProgram();
    
public:
    ~RenderContextGL2();
    static RenderContextGL2 *getInstance();
    
    GLuint buildShader(const char *shaderSource, GLenum shaderType);
    GLuint buildProgram(const char *vertexShaderSource, const char *fragmentShaderSource);

    void applyOrtho(float left, float right, float bottom, float top);
    void applyTSRMatrix() {
        glUniformMatrix4fv(m_uniforms[m_program].TSR_MATRIX, 1, 0, (GLfloat *)&m_tsrMatrix);
    }
        
    void switchToProgram(PROGRAM_STATE program);

public:
    esMatrix * tsrMatrix() { return &m_tsrMatrix; }
    
    void layout(float width, float height);
    void save();
    void restore();
    
    void translate(float x, float y);
    void scale(float sx, float sy);
    void rotate(float angle);
    void scissor(const Rect4f *rect);
    void unscissor();
    void changeAlpha(float alpha);
    
    void fillRect(const Color4f &color,float x, float y, float w, float h);
    void drawRect(const Image *image, GLuint vbo);
};

inline RenderContextGL2 * RenderContextGL2::getInstance()
{
    if( m_instance == NULL ) {
        m_instance = new RenderContextGL2();
    }
    
    return m_instance;
}

inline void RenderContextGL2::save()
{
    if( m_matrixStackIndex+1 >= MATRIX_STACK_DEPTH ) {
        ERROR("RenderContextGL2 TOO MUCH save called, please minimize save call!!!")
        return;
    }
    m_matrixStackIndex++;
    esMatrixCopy(&m_matrixStack[m_matrixStackIndex], &m_tsrMatrix);
    m_alphaStack[m_matrixStackIndex] = m_alphaStack[m_matrixStackIndex-1];
}

inline void RenderContextGL2::restore()
{
    if( m_matrixStackIndex <= 0 ) {
        ERROR("RenderContextGL2 TOO MUCH restore called, please minimize restore call!!!")
    }
    
    m_matrixStackIndex = m_matrixStackIndex > 0 ? m_matrixStackIndex-1 : 0;
    esMatrixCopy(&m_tsrMatrix, &m_matrixStack[m_matrixStackIndex]);
    applyTSRMatrix();
}

inline void RenderContextGL2::translate(float x, float y)
{
    LOG("RenderContextGL2 translate with x=%f, y=%f", x, y)
    esTranslate(&m_tsrMatrix, x, y);
    applyTSRMatrix();
}

inline void RenderContextGL2::scale(float sx, float sy)
{
    LOG("RenderContextGL2 scale with sx=%f, sy=%f", sx, sy)
    esScale(&m_tsrMatrix, sx, sy);
    applyTSRMatrix();
}

inline void RenderContextGL2::rotate(float angle)
{
    LOG("RenderContextGL2 rotate with angle=%f", angle)
    esRotate(&m_tsrMatrix, angle);
    applyTSRMatrix();
}

inline void RenderContextGL2::scissor(const Rect4f *rect)
{
    if( !rect ) return;
    LOG("RenderContextGL2 scissor with bounds(%f,%f,%f,%f)", rect->x, rect->y, rect->w, rect->h);
    glEnable(GL_SCISSOR_TEST);
    
    esPoint src1 = {rect->x, rect->y};
    esPoint src2 = {rect->x+rect->w, rect->y+rect->h};
    esPoint dst1 = esTransformPoint(&src1, &m_tsrMatrix);
    esPoint dst2 = esTransformPoint(&src2, &m_tsrMatrix);
    
    Rect4f newRect;
    newRect.x = fminf(dst1.x, dst2.x);
    newRect.y = fminf(dst1.y, dst2.y);
    newRect.w = fabsf(dst1.x - dst2.x);
    newRect.h = fabsf(dst1.y - dst2.y);
    
    glScissor(newRect.x, newRect.y, newRect.w, newRect.h);
}

inline void RenderContextGL2::unscissor()
{
    glDisable(GL_SCISSOR_TEST);
}

inline void RenderContextGL2::changeAlpha(float alpha)
{
    LOG("RenderContextGL2 change alpha with alpha=%f", alpha)
    m_alphaStack[m_matrixStackIndex] *= alpha;
}


#endif
