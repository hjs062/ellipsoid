//
//  main.cpp
//  test
//
//  Created by hjs on 3/19/18.
//  Copyright © 2018 com.hjs. All rights reserved.
//

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>

#include <glm/vec3.hpp> // glm::vec3
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective

#include "../../include/ellipsoid.hpp"

static const char* vertex_shader_text =
"uniform mat4 MVP;\n"
"attribute vec3 vCol;\n"
"attribute vec3 vPos;\n"
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 1.0);\n"
"    color = vCol;\n"
"}\n";

static const char* fragment_shader_text =
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_FragColor = vec4(color, 1.0);\n"
"}\n";

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main(void)
{
    JS::Geo::Ellipsoid e(4, 3, 2);
    
    std::vector<glm::vec3> vecPosition;
    e.getPositionsBySampling(vecPosition, 5, 6);
    
    GLFWwindow* window;
    GLuint vertex_buffer, vertex_buffer_axis, vertex_shader, fragment_shader, program;
    GLint mvp_location, vpos_location, vcol_location;
    
    glfwSetErrorCallback(error_callback);
    
    if (!glfwInit())
        exit(EXIT_FAILURE);
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    
    window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    glfwSetKeyCallback(window, key_callback);
    
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glfwSwapInterval(1);
    
    // NOTE: OpenGL error checks have been omitted for brevity
    
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vecPosition[0]) * vecPosition.size(), vecPosition.data(), GL_STATIC_DRAW);
    
    const float vertex_axis[] =
    {
        0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        9.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 9.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 9.0f, 0.0f, 0.0f, 1.0f
    };
    glGenBuffers(1, &vertex_buffer_axis);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_axis);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_axis), vertex_axis, GL_STATIC_DRAW);
    
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertex_shader);
    
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);
    
    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    
    mvp_location = glGetUniformLocation(program, "MVP");
    vpos_location = glGetAttribLocation(program, "vPos");
    vcol_location = glGetAttribLocation(program, "vCol");
    
    glUseProgram(program);
    
    const glm::mat4 & matView = glm::lookAt(glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    glEnableVertexAttribArray(vpos_location);
    
    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;
        
        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;
        
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
        
        const glm::mat4 & matProjection = glm::perspective(glm::radians(45.0f), ratio, 0.1f, 100.f);
        const glm::mat4 & matVP = matProjection * matView;
        
        // ellipsoid
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        const glm::mat4 & matModelEllipsoid = glm::rotate(glm::mat4(), (float) glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
        const glm::mat4 & matMVPEllipsoid = matProjection * matView * matModelEllipsoid;
        
        glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
        
        glDisableVertexAttribArray(vcol_location);
        glVertexAttrib3f(vcol_location, 1.0f, 1.0f, 1.0f);
        
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) &matMVPEllipsoid[0][0]);
        glDrawArrays(GL_LINE_STRIP, 0, vecPosition.size());
        
        // axis
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_axis);
        glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE, 24, (void*) 0);
        
        glEnableVertexAttribArray(vcol_location);
        glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,  24, (void*)12);
        
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) &matVP[0][0]);
        glDrawArrays(GL_LINES, 0, 6);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwDestroyWindow(window);
    
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
