//
//  MatrixUtil.cpp
//  Giraffe
//
//  Created by 思诚 李 on 2/21/12.
//  Copyright (c) 2012 Perfect World. All rights reserved.
//

#include "MatrixUtil.h"
#include <string.h>
#include <math.h>

#define PI 3.1415926535897932384626433832795f

void esLoadIdentity(esMatrix *matrix)
{
    if (matrix == NULL) {
        return;
    }
    memset(matrix, sizeof(esMatrix), 0);
    matrix->a = 1.0f;
    matrix->d = 1.0f;
    matrix->m10 = 1.0f;
    matrix->m15 = 1.0f;
}

void esScale(esMatrix *matrix, float sx, float sy)
{
    if (matrix == NULL) {
        return;
    }
    matrix->a *= sx;
    matrix->c *= sx;
    matrix->b *= sy;
    matrix->d *= sy;
}

void esTranslate(esMatrix *matrix, float tx, float ty)
{
    if (matrix == NULL) {
        return;
    }
    matrix->tx += matrix->a*tx + matrix->b*ty;
    matrix->ty += matrix->c*tx + matrix->d*ty;
}

void esRotate(esMatrix *matrix, float angle)
{
    if (matrix == NULL) {
        return;
    }
    
    float sinAngle = sinf(angle * PI / 180.0f);
    float cosAngle = cosf(angle * PI / 180.0f);
    
    esMatrix rotMat;
    esLoadIdentity(&rotMat);
    rotMat.a = cosAngle;
    rotMat.b = -sinAngle;
    rotMat.c = sinAngle;
    rotMat.d = cosAngle;
    
    esMatrixMultiply(matrix, &rotMat);
}

void esMatrixCopy(esMatrix *dst, esMatrix *src)
{
    if( src == NULL || dst == NULL ) {
        return;
    }
    
    memcpy(dst, src, sizeof(esMatrix));
}

void esMatrixMultiply(esMatrix *dst, esMatrix *src)
{
    if( src == NULL || dst == NULL) {
        return;
    }
    
    float a = dst->a;
    float c = dst->c;
    
    dst->tx = a * src->tx + dst->b * src->ty + dst->tx;
    dst->ty = c * src->tx + dst->d * src->ty + dst->ty;
    
    dst->a = a * src->a + dst->b * src->c;
    dst->b = a * src->b + dst->b * src->d;
    
    dst->c = c * src->a + dst->d * src->c;
    dst->d = c * src->b + dst->d * src->d;
    
}

void esMatrixInvert(esMatrix *dst, esMatrix *src)
{
    if( src == NULL || dst == NULL ) {
        return;
    }
    
    esLoadIdentity(dst);
    
    float i = src->a * src->d - src->b * src->c;
    
    dst->a = src->d / i;
    dst->b = -src->b / i;
    dst->c = -src->c / i;
    dst->d = src->a / i;
    dst->tx = (src->c * src->ty - src->d * src->tx) / i;
    dst->ty = (src->b * src->tx - src->a * src->ty) / i;
}

esPoint esTransformPoint(esPoint *src, esMatrix *matrix)
{
    esPoint dst = {0, 0};
    if( src == NULL || matrix == NULL) {
        return dst;
    }
    dst.x = src->x * matrix->a + src->y * matrix->c + matrix->tx;
    dst.y = src->x * matrix->b + src->y * matrix->d + matrix->ty;
    
    return dst;
}

esPoint esPointMake(float x, float y)
{
    esPoint p = {x, y};
    return p;
}


