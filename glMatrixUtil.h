#ifndef _GL_MATRIX_UTIL_H_
#define _GL_MATRIX_UTIL_H_
#include <GLES2/gl2.h>

typedef struct 
{
    GLfloat m[4][4];
}glMatrix;

void muInit(glMatrix* result);
void muMultiply(glMatrix* result, glMatrix* srcA, glMatrix* srcB);
void muTranslate(glMatrix* result, GLfloat x, GLfloat y, GLfloat z);
void muScale(glMatrix* result, GLfloat sx, GLfloat sy, GLfloat sz);
void muRotate(glMatrix* result, GLfloat angle, GLfloat sx, GLfloat sy, GLfloat sz);

void printMatrix(glMatrix* result);

#endif