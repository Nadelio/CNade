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
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int setUpShader(const char* vertSource, const char* fragSource);
WindowData buildWindow();
unsigned int setUpTexture();

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
vec3 cameraPos = { 0.0f, 0.0f, 3.0f };
vec3 cameraFront = { 0.0f, 0.0f, -1.0f };
vec3 cameraUp = { 0.0f, 1.0f, 0.0f };

bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

//shaders
const char* vertexShaderSource = "#version 330 core\n" //vert // basic shader
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

const char* vertexShaderSource2 = "#version 330 core\n" //vert // transform shader
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

const char* vertexShaderSource3 = "#version 330 core\n" // vert // projection shader
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
                                  "   TexCoord = aTexCoord;\n"
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

const char* fragmentShaderSource6 = "#version 330 core\n" //frag // blended textures shader
                                    "out vec4 FragColor;\n"
                                    "in vec2 TexCoord;\n"
                                    "uniform sampler2D texture1;\n"
                                    "uniform sampler2D texture2;\n"
                                    "void main()\n"
                                    "{\n"
                                    "    FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);\n"
                                    "}\n";

const char* fragmentShaderSource7 = "#version 330 core\n" //frag // texture shader
									"out vec4 FragColor;\n"
									"in vec2 TexCoord;\n"
									"uniform sampler2D texture1;\n"
									"void main()\n"
									"{\n"
									"    FragColor = texture(texture1, TexCoord) * vec4(1.0);\n"
									"}\n";

int main(int argc, char* argv[]){

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

    float plane1verts[] = {
        //---position--------/----color--------/texture coordinates
        //X------Y------Z----/R-----G-----B----/X-----Y----/
        -1.25f, -1.0f, -1.25f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // 0
         1.25f, -1.0f, -1.25f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, // 1
         1.25f, -1.0f,  1.25f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // 2
        -1.25f, -1.0f,  1.25f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // 3
    };

    unsigned int planeIndices[] = {
		0, 1, 3, // 1st triangle
		1, 2, 3, // 2nd triangle
	};

    float cube1verts[] = {
      //---position--------/----color--------/texture coordinates
      //X------Y------Z----/R-----G-----B----/X-----Y----/
      
        // back face
        0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // 0 // RED // top right
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // 1 // YELLOW // bottom right
       -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, // 2 // GREEN // bottom left
       -0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // 3 // BLUE // top left

        // front face
       -0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, // 4
        0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, // 5
        0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // 6
       -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // 7

        // left face
       -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // 8
       -0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // 9
       -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, // 10
       -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, // 11

        // right face // working on color correction
        0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // 12
        0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // 13
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, // 14
        0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // 15

        // bottom face
       -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // 16
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, // 17
        0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // 18
       -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // 19

        // top face
       -0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // 20
        0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // 21
        0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, // 22
       -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, // 23
    };

    unsigned int cubeIndices[] = {
        //back face
         0,  1,  3, // 1st triangle
         1,  2,  3, // 2nd triangle

        //front face
         4,  5,  6, // 3rd triangle
         6,  7,  4, // 4th triangle

        //left face
         8,  9, 11, // 5th triangle
         9, 10, 11, // 6th triangle

        //right face
        12, 13, 15, // 7th triangle
        13, 14, 15, // 8th triangle

        //bottom face
        16, 17, 19, // 9th triangle
        17, 18, 19, // 10th triangle

        //top face
        20, 21, 23, // 11th triangle
        21, 22, 23, // 12th triangle
    };

    vec3 cubePositions[] = {
        { 0.0f,  0.0f,  0.0f },
        { 2.0f,  5.0f, -15.0f },
        { -1.5f, -2.2f, -2.5f },
        { -3.8f, -2.0f, -12.3f },
        { 2.4f, -0.4f, -3.5f },
        { -1.7f,  3.0f, -7.5f },
        { 1.3f, -2.0f, -2.5f },
        { 1.5f,  2.0f, -2.5f },
        { 1.5f,  0.2f, -1.5f },
        { -1.3f,  1.0f, -1.5f }
    };

    unsigned int VBOs[2], VAOs[2], EBOs[2];
    glGenVertexArrays(2, VAOs);
    glGenBuffers(2, VBOs);
    glGenBuffers(2, EBOs);

    // SETUP TEXTURES
    unsigned int texture = setUpTexture();
    

    // SETUP RENDERING
    
    // CUBE
    glBindVertexArray(VAOs[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube1verts), cube1verts, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // PLANE
    glBindVertexArray(VAOs[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(plane1verts), plane1verts, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(planeIndices), planeIndices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); glLineWidth(2.0f); // Wireframe mode
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); glLineWidth(2.0f); // Fill mode
    glEnable(GL_DEPTH_TEST);

    // RENDER LOOP
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /*--------------------------------------------------------------------------------------*/

        // SETUP CAMERA/FOV

        // initialize view and projection matrices
        mat4 view = GLM_MAT4_IDENTITY_INIT; // camera
        mat4 projection = GLM_MAT4_IDENTITY_INIT; // FOV

        // do view and projection transforms
        vec3 cameraDirection;
        glm_vec3_add(cameraPos, cameraFront, cameraDirection);
        glm_lookat(cameraPos, cameraDirection, cameraUp, view);
        glm_perspective(glm_rad(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f, projection); // FOV, aspect ratio, near Z, far Z, projection matrix

        /*--------------------------------------------------------------------------------------*/

        // DRAW TEXTURED CUBE IN PERSPECTIVE

        // setup cube textures, shaders, VAO, and EBO
        glBindVertexArray(VAOs[0]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[0]);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUseProgram(modelShader);

        // assign view and projection matrices to shader
        unsigned int viewLoc1 = glGetUniformLocation(modelShader, "view");
        unsigned int projectionLoc1 = glGetUniformLocation(modelShader, "projection");
        glUniformMatrix4fv(viewLoc1, 1, GL_FALSE, (const GLfloat*)view);
        glUniformMatrix4fv(projectionLoc1, 1, GL_FALSE, (const GLfloat*)projection);

        for (int i = 0; i < (sizeof(cubePositions)/sizeof(vec3)); i++) {
            // initialize model matrix
            mat4 model1 = GLM_MAT4_IDENTITY_INIT;

            // do model matrix transforms
            glm_translate(model1, cubePositions[i]);
            glm_rotate(model1, glm_rad((float)glfwGetTime() * -10.0f * i), (vec3) { 1.0f, 0.5f, 0.0f });

            // assign model matrix to shader
            unsigned int modelLoc1 = glGetUniformLocation(modelShader, "model");
            glUniformMatrix4fv(modelLoc1, 1, GL_FALSE, (const GLfloat*)model1);

            // draw cube
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }

        /*--------------------------------------------------------------------------------------*/

        // DRAW PLANE IN PERSPECTIVE

        // setup plane textures, shaders, VAO, and EBO
        glBindVertexArray(VAOs[1]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[1]);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUseProgram(modelShader);

        // initialize model matrix
        mat4 model2 = GLM_MAT4_IDENTITY_INIT;

        // do model matrix transforms
        glm_translate(model2, (vec3) { 0.0f, 0.0f, 0.0f });
        glm_rotate(model2, glm_rad((float)glfwGetTime() * 10.0f), (vec3) { 0.0f, 1.0f, 0.0f });
        glm_scale(model2, (vec3) { 1.0f, 1.0f, 1.0f });

        // assign model matrix to shader
        unsigned int modelLoc2 = glGetUniformLocation(modelShader, "model");
        glUniformMatrix4fv(modelLoc2, 1, GL_FALSE, (const GLfloat*)model2);

        // assign view and projection matrices to shader
        unsigned int viewLoc2 = glGetUniformLocation(modelShader, "view");
        unsigned int projectionLoc2 = glGetUniformLocation(modelShader, "projection");
        glUniformMatrix4fv(viewLoc2, 1, GL_FALSE, (const GLfloat*)view);
        glUniformMatrix4fv(projectionLoc2, 1, GL_FALSE, (const GLfloat*)projection);

        // draw plane
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
    glDeleteProgram(textureTransformShader);
    glDeleteProgram(modelShader);

    // CLEAR ALL RESOURCES AND STOP OpenGL
    glfwTerminate();
    return 0;
}

// CHANGE WINDOW SIZE AND VIEWPORT
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = (float)xposIn;
    float ypos = (float)yposIn;

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    vec3 front;
    front[0] = (float)cos(glm_rad(yaw)) * (float)cos(glm_rad(pitch));
    front[1] = (float)sin(glm_rad(pitch));
    front[2] = (float)sin(glm_rad(yaw)) * (float)cos(glm_rad(pitch));
    glm_normalize(front);
    cameraFront[0] = front[0];
    cameraFront[1] = front[1];
    cameraFront[2] = front[2];
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    float cameraSpeed = 2.5f;// *deltaTime;
    vec3 tempVec; // Temporary vector for intermediate calculations

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        glm_vec3_scale(cameraFront, cameraSpeed, tempVec); // Scale cameraFront by cameraSpeed
        glm_vec3_add(cameraPos, tempVec, cameraPos); // Add to cameraPos
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        glm_vec3_scale(cameraFront, cameraSpeed, tempVec); // Scale cameraFront by cameraSpeed
        glm_vec3_sub(cameraPos, tempVec, cameraPos); // Subtract from cameraPos
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        vec3 crossProduct;
        glm_vec3_cross(cameraFront, cameraUp, crossProduct); // Cross product of cameraFront and cameraUp
        glm_normalize(crossProduct); // Normalize the result
        glm_vec3_scale(crossProduct, cameraSpeed, tempVec); // Scale by cameraSpeed
        glm_vec3_sub(cameraPos, tempVec, cameraPos); // Subtract from cameraPos
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        vec3 crossProduct;
        glm_vec3_cross(cameraFront, cameraUp, crossProduct); // Cross product of cameraFront and cameraUp
        glm_normalize(crossProduct); // Normalize the result
        glm_vec3_scale(crossProduct, cameraSpeed, tempVec); // Scale by cameraSpeed
        glm_vec3_add(cameraPos, tempVec, cameraPos); // Add to cameraPos
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
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    return windowData;
}

unsigned int setUpTexture(){
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(1);
    unsigned char* data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);
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