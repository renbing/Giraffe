//
//  MatrixUtil.h
//  Giraffe
//
//  Created by 思诚 李 on 2/21/12.
//  Copyright (c) 2012 Perfect World. All rights reserved.
//

#ifndef NativeGiraffe_MatrixUtil_h
#define NativeGiraffe_MatrixUtil_h

// OpenGL 列矩阵, TS1 * TS2 * TS3 * Point

typedef struct {
    float a,c,m2,m3;
    float b,d,m6,m7;
    float m8,m9,m10,m11;
    float tx,ty,m14,m15;
} esMatrix;

typedef struct {
    float x,y;
} esPoint;

void esLoadIdentity(esMatrix *matrix);

void esScale(esMatrix *matrix, float sx, float sy);
void esTranslate(esMatrix *matrix, float tx, float ty);
void esRotate(esMatrix *matrix, float angle);

void esMatrixMultiply(esMatrix *dst, esMatrix *src);
void esMatrixInvert(esMatrix *dst, esMatrix *src);
void esMatrixCopy(esMatrix *dst, esMatrix *src);

esPoint esTransformPoint(esPoint *src, esMatrix *matrix);
esPoint esPointMake(float x, float y);

#endif
