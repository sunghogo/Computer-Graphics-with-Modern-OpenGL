#include <stdio.h>
#include <string.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Window dimensions
const GLint WIDTH = 800, HEIGHT = 600;

GLuint VAO, VBO, shader;

// Vertex Shader (C-style)
static const char* vShader = "                                          \n\
#version 330                                                            \n\
                                                                        \n\
layout (location = 0) in vec3 pos;                                      \n\
                                                                        \n\
void main()                                                             \n\
{                                                                       \n\
    gl_Position = vec4(0.5 * pos.x, 0.5 * pos.y, pos.z, 1.0);           \n\
}                                                                       \n\
";

// Fragment Shader (C++11 Raw Strings)
static const char* fShader = R"(
#version 330

out vec4 colour;

void main()
{
    colour = vec4(1.0, 0.0, 0.0, 1.0);
}
)";

void createTriangle() {
    GLfloat vertices[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        0.0f, 1.0f, 0.0f
    };

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

            glVertexAttribPointer(
                0,              // location = 0 in the shader
                3,              // 3 floats per vertex (x, y, z)
                GL_FLOAT,       // each component is a float
                GL_FALSE,       // don't normalize (floats don't need it)
                0,              // tightly packed (no extra bytes between)
                0               // offset is 0 (start of buffer)
            );
            glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

void addShader(GLuint theProgram, const char* shaderCode, GLenum shaderType) {
    GLuint theShader = glCreateShader(shaderType);

    const GLchar* theCode[1];
    theCode[0] = shaderCode;

    GLint codeLength[1];
    codeLength[0] = (GLint) strlen(shaderCode);

    // Setup shader source and compile
    glShaderSource(theShader, 1, theCode, codeLength);
    glCompileShader(theShader);

    // Error checking
    GLint result = 0;
    GLchar eLog[1024] = { 0 };

    // Verify shader linking to current OpenGL context
    glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
    if (!result) 
    {
        glGetShaderInfoLog(theShader, sizeof(eLog), NULL, eLog);
        printf("Error compiling the %d shader: '%s'\n", shaderType, eLog);
        return;
    }

    // Attach Shader
    glAttachShader(theProgram, theShader);
    return;
}

void compileShaders() {
    shader = glCreateProgram();

    if (!shader) 
    {
        printf("Error creating shader program!");
        return;
    }

    // Add vertex and fragment shaders
    addShader(shader, vShader, GL_VERTEX_SHADER);
    addShader(shader, fShader, GL_FRAGMENT_SHADER);

    // Error checking
    GLint result = 0;
    GLchar eLog[1024] = { 0 };

    // Verify shader linking to current OpenGL context
    glLinkProgram(shader);
    glGetProgramiv(shader, GL_LINK_STATUS, &result);
    if (!result) 
    {
        glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
        printf("Error linking program: '%s'\n", eLog);
        return;
    }

    // Validate shader in current OpenGL context
    glBindVertexArray(VAO);
        glValidateProgram(shader);
        glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
        if (!result) 
        {
            glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
            printf("Error validating program: '%s'\n", eLog);
            return;
        }
    glBindVertexArray(0);
}

int main() {
    // Initialize GLFW
    if (!glfwInit())
    {
        printf("GLFW Initialization Failed\n");
        glfwTerminate();
        return 1;
    }

    // Setup GLFW window properties
    // OpenGL version 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    
    // Core profile = No Backwards Compatibility
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Allow forward compatibility
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow *mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Test Window", NULL, NULL);
    if (!mainWindow) {
        printf("GLFW Window creation failed!\n");
        glfwTerminate();
        return 1;
    }

    // Get Buffer size information
    int bufferWidth, bufferHeight;
    glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

    // Set context for GLEW to use
    glfwMakeContextCurrent(mainWindow);

    // Allow modern extension features
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK) {
        printf("GlEW Initialization failed!\n");
        glfwDestroyWindow(mainWindow);
        glfwTerminate();
        return 1;
    }

    // Create Viewport
    glViewport(0, 0, bufferWidth, bufferHeight);

    // Create and color triangle
    createTriangle();
    compileShaders();

    // Loop until window closed
    while (!glfwWindowShouldClose(mainWindow)) {
        // Get + Handle user input events
        glfwPollEvents();

        // Clear window
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //rgba
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Use shader(s)
        glUseProgram(shader);
            glBindVertexArray(VAO);
                glDrawArrays(GL_TRIANGLES, 0, 3);
            glBindVertexArray(0);
        glUseProgram(0);

        glfwSwapBuffers(mainWindow);
    }

    return 0;
}
