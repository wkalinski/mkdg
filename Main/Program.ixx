module;
#define GLFW_DLL
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <filesystem>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
export module Program;

import Utils;
import Texture;



export class Program
{
    const std::vector<float> windowQuad =
    {
        -1.0f, -1.0f, 0.0f, //0
        -1.0f, 1.0f, 0.0f,  //1
        1.0f, 1.0f, 0.0f,   //2
        1.0f, 1.0f, 0.0f,   //2
        1.0f, -1.0f, 0.0f,  //3
        -1.0f, -1.0f, 0.0f  //0
    };

public:
    GLFWwindow* window = 0;

    GLuint quadBuffer = 0;


    GLuint loc_width, loc_height;
    GLuint vao;

    int width, height;

    Texture* imageTexture;

    int localWindowSize = 1;

    void PrintOglInfo(std::ostream& stream = std::cout)
    {
        int oglVerMajor = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
        int oglVerMinor = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
        int oglProfile = glfwGetWindowAttrib(window, GLFW_OPENGL_PROFILE);

        const char* oglProfile_str;
        switch (oglProfile)
        {
        case GLFW_OPENGL_CORE_PROFILE:
            oglProfile_str = "Core";
            break;
        case GLFW_OPENGL_COMPAT_PROFILE:
            oglProfile_str = "Compatibility";
            break;
        default:
            oglProfile_str = "Unknown";
            break;
        }

        stream << "OpenGL version " << oglVerMajor << "." << oglVerMinor << " " << oglProfile_str << std::endl;
    }


    std::vector<std::string> shaderNames = { "ori", "mean", "var", "skew" };
    std::vector<GLuint> shaderPrograms;
    int currentShaderIndex = 0;

    void LoadShaders(std::filesystem::path dir)
    {
        for (int i = 0; i < shaderNames.size(); ++i)
        {
            GLuint vs, fs;
            GLuint shaderProgram = glCreateProgram();
            vs = CreateShaderFromFile((dir / "vertex.vert").string(), GL_VERTEX_SHADER);
            fs = CreateShaderFromFile( (dir / std::string(shaderNames[i] + ".frag")).string(), GL_FRAGMENT_SHADER);
            glAttachShader(shaderProgram, vs);
            glAttachShader(shaderProgram, fs);
            glLinkProgram(shaderProgram);
            shaderPrograms.push_back(shaderProgram);
        }
    }

    void Run(std::vector<std::string> args)
    {
        using path = std::filesystem::path;

        stbi_flip_vertically_on_write(1);
        glfwInit();

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
        window = glfwCreateWindow(100, 100, "MKDG", nullptr, nullptr);

        PrintOglInfo();

        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);
        gladLoadGL();
        glDebugMessageCallback(oglDebugCallback, nullptr);


        glCreateVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glCreateBuffers(1, &quadBuffer);
        glNamedBufferData(quadBuffer, windowQuad.size() * sizeof(float), &windowQuad[0], GL_STATIC_DRAW);

        if (args.size() < 2)
        {
            throw std::runtime_error("Err");
        }

        if (args.size() >= 3)
        {
            localWindowSize = std::stoi(args[2]);
        }

        imageTexture = Texture::LoadTexture(args[1]);
        glBindTexture(GL_TEXTURE_2D, imageTexture->textureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);



        path currentExe = args[0];
        path parentFolder = currentExe.parent_path();
        path shadersDir = parentFolder / "shaders";

        LoadShaders(shadersDir);

        glfwSetWindowSize(window, imageTexture->width, imageTexture->height);
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        GLubyte* data = new GLubyte[width * height * 4];

        bool bWritten = false;

        for (int i = 0; i < shaderNames.size(); ++i)
        {
            currentShaderIndex = i;
            Render();
            glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
            stbi_write_png(std::string(std::string("_out_") + shaderNames[currentShaderIndex] + ".png").c_str(), width, height, 4, data, width * 4);
            bWritten = true;
            glfwSwapBuffers(window);
        }

        Cleanup();
	}

    void Render()
    {
        glClearColor(0.0f, 0.4f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderPrograms[currentShaderIndex]);

        loc_width = glGetUniformLocation(shaderPrograms[currentShaderIndex], "textureWidth");
        loc_height = glGetUniformLocation(shaderPrograms[currentShaderIndex], "textureHeight");
        int loc_win = glGetUniformLocation(shaderPrograms[currentShaderIndex], "localWindowSize");

        glUniform1i(loc_width, width);
        glUniform1i(loc_height, height);


        glUniform1i(loc_win, localWindowSize);

        glBindBuffer(GL_ARRAY_BUFFER, quadBuffer);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
        );
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, imageTexture->textureId);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    void Cleanup()
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    static void APIENTRY oglDebugCallback(GLenum source​, GLenum type​, GLuint id​,GLenum severity​, GLsizei length​, const GLchar* message​, const void* userParam​)
    {
        std::cout << message​ << std::endl;
    }
};