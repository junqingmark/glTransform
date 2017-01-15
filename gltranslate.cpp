#include <iostream>
#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <X11/Xlib.h>
#include <cstdlib>
#include "glMatrixUtil.h"
#include <signal.h>
#include <sys/types.h>
#include <X11/keysym.h>
#include <unistd.h>
#include <cstdio>

using namespace std;

EGLSurface eglSurface;
EGLDisplay eglDisplay;
int quit;
GLuint fbo;
GLuint fbo_tex;
GLuint tex;
char* pixel = NULL;
GLint uniformLoc;

/*
GLfloat vVertices[] = {-0.5f, 0.5f, 0.0f,
0.5f, 0.5f, 0.0f,
0.5f, -0.5f, 0.0f,
-0.5f, 0.5f, 0.0f,
-0.5f, -0.5f, 0.0f,
0.5f, -0.5f, 0.0f
};
GLfloat vColors[] = {1.0f, 0.0f, 0.0f,
0.0f, 1.0f, 0.0f,
0.0f, 0.0f, 1.0f,
1.0f, 0.0f, 0.0f,
0.0f, 1.0f, 0.0f,
0.0f, 0.0f, 1.0f};
*/
GLfloat vVertices[] = {
    -0.25f, 0.25f, 0.0f,
    0.25f, 0.25f, 0.0f,
    0.25f, -0.25f, 0.0f,
    -0.25f, 0.25f, 0.0f,
    -0.25f, -0.25f, 0.0f,
    0.25f, -0.25f, 0.0f
};
/*
GLfloat vCoords[] = {
0.0f, 0.0f,
1.0f, 0.0f,
1.0f, 1.0f,
0.0f, 0.0f,
0.0f, 1.0f,
1.0f, 1.0f
};
*/

GLfloat vCoords[] = {
    0.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 0.0f,

    0.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 0.0f
};

GLuint LoadShader(const char *shaderSrc, GLenum type)
{
    GLuint shader;
    GLint compiled;

    // Create the shader object
    shader = glCreateShader(type);
    if (shader == 0)
        return 0;

    // Load the shader source
    glShaderSource(shader, 1, &shaderSrc, NULL);
    // Compile the shader
    glCompileShader(shader);

    // Check the compile status
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled)
    {
        GLint infoLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen > 1)
        {
            char* infoLog = static_cast<char*>(malloc(sizeof(char) * infoLen));
            glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
            cout << "Error compiling shader: " << infoLog << endl;

            free(infoLog);
        }

        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

int GLESInit()
{
    GLchar vShaderStr[] =
        "attribute vec3 vVertices;\n"
        "uniform mat4 mvp;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = mvp * vec4(vVertices, 1.0f);\n"
        "}\n";


    GLchar fShaderStr[] =
        "precision mediump float;\n"
        "void main()\n"
        "{\n"
        "    gl_FragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);\n"
        "}\n";

    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint programObject;
    GLint linked;

    // Load the vertex/fragment shaders
    vertexShader = LoadShader(vShaderStr, GL_VERTEX_SHADER);
    fragmentShader = LoadShader(fShaderStr, GL_FRAGMENT_SHADER);

    // Create the program object
    programObject = glCreateProgram();
    if (programObject == 0)
        return 0;
    glAttachShader(programObject, vertexShader);
    glAttachShader(programObject, fragmentShader);

    // Bind vPosition to attribute 0
    glBindAttribLocation(programObject, 0, "vVertices");

    // Link the program
    glLinkProgram(programObject);

    // Check the link status
    glGetProgramiv(programObject, GL_LINK_STATUS, &linked);
    if (!linked)
    {
        GLint infoLen = 0;
        glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen > 1)
        {
            char* infoLog = static_cast<char*>(malloc(sizeof(char) * infoLen));
            glGetProgramInfoLog(programObject, infoLen, NULL, infoLog);
            cout << "Error linking program: " << infoLog << endl;
            free(infoLog);
        }
        glDeleteProgram(programObject);
        return 0;
    }
    else
    {
        glUseProgram(programObject);
    }

    uniformLoc = glGetUniformLocation(programObject, "mvp");
    printf("the uniform is %d\n", uniformLoc);

    return 1;

}

EGLBoolean InitializeWindow(EGLNativeWindowType surface_id, EGLNativeDisplayType display_id)
{
    const EGLint configAttributes[] =
    {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_DEPTH_SIZE, 24,
        EGL_NONE
    };

    const EGLint contextAttributes[] =
    {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };

    eglDisplay = eglGetDisplay(display_id);
    if (eglDisplay == EGL_NO_DISPLAY)
    {

        cout << "Fail to get the native display!" << endl;
        return EGL_FALSE;
    }

    EGLint major, minor;
    //EGLBoolean eglInitialize(EGLDisplay display, EGLint* majorVersion, EGLint* minorVersion);  
    if (!eglInitialize(eglDisplay, &major, &minor))
    {

        cout << "Fail to initialize the EGL !" << endl;
        return EGL_FALSE;
    }
    else
    {

        cout << "The version of EGL is: " << major << " . " << minor << endl;
    }

    EGLConfig config;
    EGLint ConfigNum;
    if (!eglChooseConfig(eglDisplay, configAttributes, &config, 1, &ConfigNum))
    {
        return EGL_FALSE;
    }

    //EGLSurface eglCreateWindowSurface(EGLDisplay dpy, EGLConfig config,
    //              EGLNativeWindowType win,
    //              const EGLint *attrib_list);


    eglSurface = eglCreateWindowSurface(eglDisplay, config, surface_id, NULL);
    if (eglSurface == EGL_NO_SURFACE)
    {
        return EGL_FALSE;
    }

    //eglCreateContext(EGLDisplay dpy, EGLConfig config,
    //            EGLContext share_context,
    //            const EGLint *attrib_list);
    EGLContext context = eglCreateContext(eglDisplay, config, EGL_NO_CONTEXT, contextAttributes);
    if (context == EGL_NO_CONTEXT)
    {
        return EGL_FALSE;
    }

    //eglMakeCurrent(EGLDisplay dpy, EGLSurface draw,
    //          EGLSurface read, EGLContext ctx);
    if (!eglMakeCurrent(eglDisplay, eglSurface, eglSurface, context))
    {
        return EGL_FALSE;
    }

    return EGL_TRUE;
}


void drawscene(int width, int height, glMatrix* matrix)
{
    glViewport(0, 0, width, height);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnableVertexAttribArray(0);
    

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vVertices);

    glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, &(matrix->m[0][0]));

    glDrawArrays(GL_TRIANGLES, 0, 6);
}


int main(int argc, char* argv[])
{
    KeySym   escape;
    Display *x_display;
    glMatrix matrix;
    muInit(&matrix);

    int width = 800;
    int height = 600;
    x_display = XOpenDisplay(NULL);
    int screen = DefaultScreen(x_display);

    Window win = XCreateSimpleWindow(x_display, RootWindow(x_display, screen), 0, 0, /*DisplayWidth(x_display, screen)*/width, /*DisplayHeight(x_display, screen)*/height, 0, 0, WhitePixel(x_display, screen));

    XSelectInput(x_display, win, ExposureMask | KeyPressMask | KeyReleaseMask);
    escape = XKeysymToKeycode(x_display, XK_Escape);

    XMapWindow(x_display, win);

    if (!InitializeWindow(win, x_display))
    {

        cout << "Intiallize the EGL window Fail!" << endl;
        return 1;
    }

    if (GLESInit() == 0)
    {
        cout << "Fail to Init GLES!" << endl;
    }


    quit = 0;
    XEvent event;

    while (!quit)
    {
        printf("in the loop\n");
        
        XNextEvent(x_display, &event);
        if (event.type == KeyPress)
        {
            cout << "===KeyPress===" << endl;
            if (event.xkey.keycode == escape)
            {
                quit = 1;
            }

        }
        
        //muTranslate(&matrix, 0.1f, 0.0f, 0.0f);
        //muScale(&matrix, 1.2f, 1.0f, 1.0f);
        muRotate(&matrix, 10.0f, 1.0f, 0.0f, 0.0f);
        printMatrix(&matrix);
        drawscene(width, height, &matrix);
        eglSwapBuffers(eglDisplay, eglSurface);
        
        sleep(1);
    }


    return 1;
}
