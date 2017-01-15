#include "glMatrixUtil.h"
#include <string.h>
#include <iostream>
#include <math.h>

#define PI 3.1415926535897932384626433832795f

void muInit(glMatrix* result)
{
    memset(result, 0, sizeof(glMatrix));
    result->m[0][0] = 1.0f;
    result->m[1][1] = 1.0f;
    result->m[2][2] = 1.0f;
    result->m[3][3] = 1.0f;

}

void muMultiply(glMatrix* result, glMatrix* srcA, glMatrix* srcB)
{
    glMatrix tmp;
    int i; 

    for(i = 0; i < 4; i++)
    {
        tmp.m[i][0] = (srcA->m[i][0] * srcB->m[0][0]) + 
                        (srcA->m[i][1] * srcB->m[1][0]) +
                        (srcA->m[i][2] * srcB->m[2][0]) +
                        (srcA->m[i][3] * srcB->m[3][0]) ;

        tmp.m[i][1] = (srcA->m[i][0] * srcB->m[0][1]) + 
                        (srcA->m[i][1] * srcB->m[1][1]) +
                        (srcA->m[i][2] * srcB->m[2][1]) +
                        (srcA->m[i][3] * srcB->m[3][1]) ;

        tmp.m[i][2] = (srcA->m[i][0] * srcB->m[0][2]) + 
                        (srcA->m[i][1] * srcB->m[1][2]) +
                        (srcA->m[i][2] * srcB->m[2][2]) +
                        (srcA->m[i][3] * srcB->m[3][2]) ;

        tmp.m[i][3] = (srcA->m[i][0] * srcB->m[0][3]) + 
                        (srcA->m[i][1] * srcB->m[1][3]) +
                        (srcA->m[i][2] * srcB->m[2][3]) +
                        (srcA->m[i][3] * srcB->m[3][3]) ;
    }
    memcpy(result, &tmp, sizeof(glMatrix));
}


void muTranslate(glMatrix* result, GLfloat tx, GLfloat ty, GLfloat tz)
{ 
    result->m[3][0] += (result->m[0][0] * tx + result->m[1][0] * ty + result->m[2][0] * tz);
    result->m[3][1] += (result->m[0][1] * tx + result->m[1][1] * ty + result->m[2][1] * tz);
    result->m[3][2] += (result->m[0][2] * tx + result->m[1][2] * ty + result->m[2][2] * tz);
    result->m[3][3] += (result->m[0][3] * tx + result->m[1][3] * ty + result->m[2][3] * tz);
}

void muScale(glMatrix* result, GLfloat sx, GLfloat sy, GLfloat sz)
{
    result->m[0][0] *= sx;
    result->m[0][1] *= sx;
    result->m[0][2] *= sx;
    result->m[0][3] *= sx;

    result->m[1][0] *= sy;
    result->m[1][1] *= sy;
    result->m[1][2] *= sy;
    result->m[1][3] *= sy;

    result->m[2][0] *= sz;
    result->m[2][1] *= sz;
    result->m[2][2] *= sz;
    result->m[2][3] *= sz;

}

void muRotate(glMatrix* result, GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
    GLfloat sinAngle, cosAngle;
    GLfloat oneMinusCos;
    GLfloat mag = sqrtf(x*x + y*y + z*z);

    sinAngle = sinf(angle * PI / 180.0f);
    cosAngle = cosf(angle * PI / 180.0f);

    if(mag > 0.0f)
    {
        GLfloat xx, yy, zz, xy, yz, zx, xs, ys, zs;

        glMatrix rotMatrix;

        x /= mag;
        y /= mag;
        z /= mag;

        xx = x * x;
        yy = y * y;
        zz = z * z;
        xy = x * y;
        yz = y * z;
        zx = z * x;

        xs = x * sinAngle;
        ys = y * sinAngle;
        zs = z * sinAngle;

        oneMinusCos = 1.0f - cosAngle;

        rotMatrix.m[0][0] = (oneMinusCos * xx) + cosAngle;
        rotMatrix.m[0][1] = (oneMinusCos * xy) - zs;
        rotMatrix.m[0][2] = (oneMinusCos * zx) + ys;
        rotMatrix.m[0][3] = 0.0f;

        rotMatrix.m[1][0] = (oneMinusCos * xy) + zs;
        rotMatrix.m[1][1] = (oneMinusCos * yy) + cosAngle;
        rotMatrix.m[1][2] = (oneMinusCos * yz) - xs;
        rotMatrix.m[1][3] = 0.0f;

        rotMatrix.m[2][0] = (oneMinusCos * zx) - ys;
        rotMatrix.m[2][1] = (oneMinusCos * yz) + xs;
        rotMatrix.m[2][2] = (oneMinusCos * zz) + cosAngle;
        rotMatrix.m[2][3] = 0.0f;

        rotMatrix.m[3][0] = 0.0f;
        rotMatrix.m[3][1] = 0.0f;
        rotMatrix.m[3][2] = 0.0f;
        rotMatrix.m[3][3] = 1.0f;

        muMultiply(result, &rotMatrix, result);

    }

}

void printMatrix(glMatrix* result)
{
    for(int i = 0; i < 4; i++ )
    {
        for(int j = 0; j < 4; j++)
        {
            std::cout << result->m[i][j]<< " ";
        }
        std::cout << std::endl;
    }
}