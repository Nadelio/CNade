#define _CRT_SECURE_NO_WARNINGS
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include "stb/stb_image.h"
#include <cglm/cglm.h>

typedef struct WindowData {
	GLFWwindow* window;
	int status;
} WindowData;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
unsigned int setUpShader(const char* vertSource, const char* fragSource);
WindowData buildWindow();
unsigned int setUpTexture();

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//shaders
const char* vertexShaderSource = "#version 330 core\n" //vert
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "layout (location = 1) in vec3 aColor;\n"
                                 "layout (location = 2) in vec2 aTexCoord;\n"
                                 "out vec3 vertexColor;\n"
                                 "out vec2 TexCoord;\n"
                                 "out vec3 vertexPos;\n"
                                 "void main()\n"
                                 "{\n"
                                 "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                 "   vertexColor = aColor;\n"
                                 "   vertexPos = aPos;\n"
                                 "   TexCoord = aTexCoord;\n"
                                 "}\0";

const char* vertexShaderSource2 = "#version 330 core\n" //vert
								  "layout (location = 0) in vec3 aPos;\n"
								  "layout (location = 1) in vec3 aColor;\n"
							 	  "layout (location = 2) in vec2 aTexCoord;\n"
								  "out vec3 vertexColor;\n"
								  "out vec2 TexCoord;\n"
                                  "uniform mat4 transform;\n"
							 	  "void main()\n"
								  "{\n"
								  "   gl_Position = transform * vec4(aPos, 1.0);\n"
								  "   vertexColor = aColor;\n"
								  "   TexCoord = aTexCoord;\n"
								  "}\0";

const char* vertexShaderSource3 = "#version 330 core\n" // vert
                                  "layout(location = 0) in vec3 aPos;\n"
                                  "layout (location = 1) in vec3 aColor;\n"
                                  "layout(location = 2) in vec2 aTexCoord;\n"
                                  "out vec3 vertexColor;\n"
                                  "out vec2 TexCoord;\n"
                                  "out vec3 vertexPos;\n"
                                  "uniform mat4 model;\n"
                                  "uniform mat4 view;\n"
                                  "uniform mat4 projection;\n"
                                  "void main()\n"
                                  "{\n"
                                  "   vertexColor = aColor; \n"
                                  "   vertexPos = aPos;\n"
                                  "   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
                                  "   TexCoord = vec2(aTexCoord.x, aTexCoord.y);\n"
                                  "}\n";

const char* fragmentShaderSource1 = "#version 330 core\n" //frag // orange shader
                                    "out vec4 FragColor;\n"
                                    "void main()\n"
                                    "{\n"
                                    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                    "}\n\0";

const char* fragmentShaderSource2 = "#version 330 core\n" //frag // yellow shader
									"out vec4 FragColor;\n"
									"void main()\n"
									"{\n"
									"   FragColor = vec4(1.0f, 0.7f, 0.0f, 1.0f);\n"
									"}\n\0";

const char* fragmentShaderSource3 = "#version 330 core\n" //frag // vertex color shader
									"out vec4 FragColor;\n"
                                    "in vec3 vertexColor;\n"
									"void main()\n"
									"{\n"
									"   FragColor = vec4(vertexColor, 1.0);\n"
									"}\n\0";

const char* fragmentShaderSource4 = "#version 330 core\n" //frag // texture RGB shader
                                    "out vec4 FragColor;\n"
                                    "in vec3 vertexColor;\n"
                                    "in vec2 TexCoord;\n"
                                    "uniform sampler2D ourTexture;\n"
                                    "void main()\n"
                                    "{\n"
                                    "   FragColor = texture(ourTexture, TexCoord) * vec4(vertexColor, 1.0);\n"
        							"}\n\0";

const char* fragmentShaderSource5 = "#version 330 core\n" //frag // texture UV shader
									 "out vec4 FragColor;\n"
								   	 "in vec3 vertexColor;\n"
									 "in vec2 TexCoord;\n"
                                     "in vec3 vertexPos;\n"
									 "uniform sampler2D ourTexture;\n"
									 "void main()\n"
									 "{\n"
									 "   FragColor = vec4(vertexPos.x + 1.0, vertexPos.y + 1.0, vertexPos.z, 1.0);\n"
									 "}\n\0";

const char* fragmentShaderSource6 = "#version 330 core\n" //frag // dual texture shader
                                    "out vec4 FragColor;\n"
                                    "in vec2 TexCoord;\n"
                                    "uniform sampler2D texture1;\n"
                                    "uniform sampler2D texture2;\n"
                                    "void main()\n"
                                    "{\n"
                                    "    FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);\n"
                                    "}\n";

int main(int argc, char* argv[])
{
    // SETUP WINDOW
    WindowData windowBuildResult = buildWindow();

    if(windowBuildResult.status == -1) {
		return -1;
	}

    GLFWwindow* window = windowBuildResult.window;

    // GLAD: LOAD ALL OpenGL FUNCTION POINTERS
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialize GLAD\n");
        return -1;
    }

    // SET UP SHADERS AND TEXTURES
    unsigned int orangeShader = setUpShader(vertexShaderSource, fragmentShaderSource1);
    unsigned int yellowShader = setUpShader(vertexShaderSource, fragmentShaderSource2);
    unsigned int RGBShader = setUpShader(vertexShaderSource, fragmentShaderSource3);
    unsigned int textureRGBShader = setUpShader(vertexShaderSource, fragmentShaderSource4);
    unsigned int textureUVShader = setUpShader(vertexShaderSource, fragmentShaderSource5);
    unsigned int textureTransformShader = setUpShader(vertexShaderSource2, fragmentShaderSource4);
    unsigned int modelShader = setUpShader(vertexShaderSource3, fragmentShaderSource4);

    // SET UP VERTEX DATA AND VBOs/VAOs/EBO

    float quad1verts[] = {
        //---position---// //----color----// // texture coordinates
        0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // 0 // RED
        0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, // 1 // YELLOW
       -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, // 2 // BLUE
       -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // 3 // GREEN
    };

    float quad2verts[] = {
        //---position---// //----color----// // texture coordinates
        1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // 0 // YELLOW
        1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, // 1 // RED
       -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // 2 // BLACK
       -1.0f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // 3 // GREEN
    };

    unsigned int indices[] = {
        0, 1, 3, // 1st triangle
        1, 2, 3, // 2nd triangle
    };

    unsigned int VBOs[2], VAOs[2], EBOs[2];
    glGenVertexArrays(2, VAOs);
    glGenBuffers(2, VBOs);
    glGenBuffers(2, EBOs);

    // SETUP TEXTURES
    unsigned int texture = setUpTexture();
    

    // SETUP RENDERING
    
    // 1ST QUAD
    glBindVertexArray(VAOs[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad1verts), quad1verts, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // 2ND QUAD
    glBindVertexArray(VAOs[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad2verts), quad2verts, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // To draw in triangle as a wireframe:
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // RENDER LOOP
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);

        // DRAW BACKGROUND QUAD
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[1]);
        glBindTexture(GL_TEXTURE_2D, texture);

        glUseProgram(textureUVShader);
        glBindVertexArray(VAOs[1]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // DRAW TEXTURED QUAD IN PERSPECTIVE
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[0]);
        glBindTexture(GL_TEXTURE_2D, texture);

        glUseProgram(modelShader);

        mat4 model = GLM_MAT4_IDENTITY_INIT;
        mat4 view = GLM_MAT4_IDENTITY_INIT;
        mat4 projection = GLM_MAT4_IDENTITY_INIT;
        glm_rotate(model, glm_rad(-55.0f), (vec3) { 1.0f, 0.0f, 0.0f });
        glm_translate(view, (vec3){0.0f, 0.0f, -3.0f});
        glm_perspective(glm_rad(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f, projection);

        unsigned int modelLoc = glGetUniformLocation(modelShader, "model");
        unsigned int viewLoc = glGetUniformLocation(modelShader, "view");
        unsigned int projectionLoc = glGetUniformLocation(modelShader, "projection");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (const GLfloat*)model);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (const GLfloat*) view);
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, (const GLfloat*) projection);

        glBindVertexArray(VAOs[0]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // SWAP BUFFERS AND CHECK FOR USER INPUTS
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // DE-ALLOCATE RESOURCES
    glDeleteVertexArrays(2, VAOs);
    glDeleteBuffers(2, VBOs);
    glDeleteBuffers(2, EBOs);
    glDeleteProgram(yellowShader);
    glDeleteProgram(orangeShader);
    glDeleteProgram(RGBShader);
    glDeleteProgram(textureRGBShader);
    glDeleteProgram(textureUVShader);

    // CLEAR ALL RESOURCES AND STOP OpenGL
    glfwTerminate();
    return 0;
}

// CHANGE WINDOW SIZE AND VIEWPORT
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, 1);
    }
}

unsigned int setUpShader(const char* vertSource, const char* fragSource) {
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertSource, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
    }

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
    }

    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

WindowData buildWindow() {

    // INIT GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // CHANGE SETTINGS BASED ON OS
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // BUILD WINDOW
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    WindowData windowData = { window, 0 };
    if (window == NULL)
    {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        windowData.status = -1;
        return windowData;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    return windowData;
}

unsigned int setUpTexture(){
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char* data = stbi_load("container.jpg", & width, & height, & nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        printf("ERROR::TEXTURE::LOAD::FAILED\n");
    }
    stbi_image_free(data);
    return texture;
}