#include "TerrainApplication.h"
#include <ituGL/geometry/VertexAttribute.h>
#define STB_PERLIN_IMPLEMENTATION
#include <stb_perlin.h>
#include <vector>
#include <cmath>
#include <iostream>

// Helper structures. Declared here only for this exercise
struct Vector2
{
    Vector2() : Vector2(0.f, 0.f) {}
    Vector2(float x, float y) : x(x), y(y) {}
    float x, y;
};

struct Vector3
{
    Vector3() : Vector3(0.f,0.f,0.f) {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
    float x, y, z;

    Vector3 Normalize() const
    {
        float length = std::sqrt(1 + x * x + y * y);
        return Vector3(x / length, y / length, z / length);
    }
};

// (todo) 01.8: Declare an struct with the vertex format



TerrainApplication::TerrainApplication()
    : Application(1024, 1024, "Terrain demo"), m_gridX(32), m_gridY(32), m_shaderProgram(0)
{
}

void TerrainApplication::Initialize()
{
    Application::Initialize();

    // Build shaders and store in m_shaderProgram
    BuildShaders();


    std::vector<Vector3> vertices;
    std::vector<unsigned int> indices;
    std::vector<Vector2> verticesTexture;

    float x_scale = 1.0f / m_gridX;
    float y_scale = 1.0f / m_gridY;

    for (int i = 0; i <= m_gridX; ++i) {
        float x = i * x_scale - 0.5f;
        float y = -0.5f;
        float z = stb_perlin_fbm_noise3(x * 2, y * 2, 0.0f, 1.9f, 0.5f, 8) * 0.5f;
        Vector3 bottom = Vector3(x, y, z);

        vertices.push_back(bottom);
        verticesTexture.push_back(Vector2(i, 0));
    }

    for (int i = 1; i <= m_gridY; ++i) {
        for (int j = 1; j <= m_gridX; ++j) {
            if (j == 1) {
                float x1 = -0.5f;
                float y1 = i * y_scale - 0.5f;
                float z1 = stb_perlin_fbm_noise3(x1 * 2, y1 * 2, 0.0f, 1.9f, 0.5f, 8) * 0.5f;
                Vector3 left = Vector3(x1, y1, z1);

                vertices.push_back(left);
                verticesTexture.push_back(Vector2(j-1, i));
            }

            float x = j * x_scale - 0.5f;
            float y = i * y_scale - 0.5f;
            float z = stb_perlin_fbm_noise3(x * 2, y * 2, 0.0f, 1.9f, 0.5f, 8) * 0.5f;
            Vector3 right = Vector3(x, y, z);
            
            vertices.push_back(right);
            verticesTexture.push_back(Vector2(j, i));

            //Triangle one
            int bottom_left = ((i - 1) * (m_gridY + 1)) + j - 1;
            int bottom_right = ((i - 1) * (m_gridY + 1)) + j;
            int top_left = bottom_left + m_gridY + 1;
            int top_right = bottom_right + m_gridY + 1;
            indices.push_back(bottom_left);
            indices.push_back(bottom_right);
            indices.push_back(top_left);

            //Triangle one
            indices.push_back(bottom_right);
            indices.push_back(top_left);
            indices.push_back(top_right);

        }
    }

    m_vao.Bind();

    m_vbo.Bind();
    m_vbo.AllocateData(vertices.size() * (sizeof(Vector3) + sizeof(Vector2)));
    m_vbo.UpdateData(std::span(vertices), 0);
    m_vbo.UpdateData(std::span(verticesTexture), vertices.size() * sizeof(Vector3));


    m_ebo.Bind();
    m_ebo.AllocateData<unsigned int>(std::span(indices));

    VertexAttribute position(Data::Type::Float, 3);
    m_vao.SetAttribute(0, position, 0);

    VertexAttribute position2(Data::Type::Float, 2);
    m_vao.SetAttribute(1, position2, vertices.size() * sizeof(Vector3));

    VertexBufferObject::Unbind();
    VertexArrayObject::Unbind();
    ElementBufferObject::Unbind();

    // Enables wireframe
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

}

void TerrainApplication::Update()
{
    Application::Update();

    UpdateOutputMode();
}

void TerrainApplication::Render()
{
    Application::Render();

    // Clear color and depth
    GetDevice().Clear(true, Color(0.0f, 0.0f, 0.0f, 1.0f), true, 1.0f);

    // Set shader to be used
    glUseProgram(m_shaderProgram);

    m_vao.Bind();

    //glDrawArrays(GL_TRIANGLES, 0, 6 * m_gridX * m_gridY);
    glDrawElements(GL_TRIANGLES, 6 * m_gridX * m_gridY, GL_UNSIGNED_INT, 0);

}

void TerrainApplication::Cleanup()
{
    Application::Cleanup();
}


void TerrainApplication::BuildShaders()
{
    const char* vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec2 aTexCoord;\n"
        "layout (location = 2) in vec3 aColor;\n"
        "layout (location = 3) in vec3 aNormal;\n"
        "uniform mat4 Matrix = mat4(1);\n"
        "out vec2 texCoord;\n"
        "out vec3 color;\n"
        "out vec3 normal;\n"
        "void main()\n"
        "{\n"
        "   texCoord = aTexCoord;\n"
        "   color = aColor;\n"
        "   normal = aNormal;\n"
        "   gl_Position = Matrix * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";
    const char* fragmentShaderSource = "#version 330 core\n"
        "uniform uint Mode = 0u;\n"
        "in vec2 texCoord;\n"
        "in vec3 color;\n"
        "in vec3 normal;\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   switch (Mode)\n"
        "   {\n"
        "   default:\n"
        "   case 0u:\n"
        "       FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
        "       break;\n"
        "   case 1u:\n"
        "       FragColor = vec4(fract(texCoord), 0.0f, 1.0f);\n"
        "       break;\n"
        "   case 2u:\n"
        "       FragColor = vec4(color, 1.0f);\n"
        "       break;\n"
        "   case 3u:\n"
        "       FragColor = vec4(normalize(normal), 1.0f);\n"
        "       break;\n"
        "   case 4u:\n"
        "       FragColor = vec4(color * max(dot(normalize(normal), normalize(vec3(1,0,1))), 0.2f), 1.0f);\n"
        "       break;\n"
        "   }\n"
        "}\n\0";

    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    m_shaderProgram = shaderProgram;
}

void TerrainApplication::UpdateOutputMode()
{
    for (int i = 0; i <= 4; ++i)
    {
        if (GetMainWindow().IsKeyPressed(GLFW_KEY_0 + i))
        {
            int modeLocation = glGetUniformLocation(m_shaderProgram, "Mode");
            glUseProgram(m_shaderProgram);
            glUniform1ui(modeLocation, i);
            break;
        }
    }
    if (GetMainWindow().IsKeyPressed(GLFW_KEY_TAB))
    {
        const float projMatrix[16] = { 0, -1.294f, -0.721f, -0.707f, 1.83f, 0, 0, 0, 0, 1.294f, -0.721f, -0.707f, 0, 0, 1.24f, 1.414f };
        int matrixLocation = glGetUniformLocation(m_shaderProgram, "Matrix");
        glUseProgram(m_shaderProgram);
        glUniformMatrix4fv(matrixLocation, 1, false, projMatrix);
    }
}
