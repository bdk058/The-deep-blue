#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <vector>

// ===== WINDOW =====
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// ===== MOUSE ROTATE =====
bool isDragging = false;
bool firstMouse = true;
float lastX, lastY;
float rotX = 0.0f;
float rotY = 0.0f;

// ===== CAMERA FIXED =====
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);

// ===== MOUSE BUTTON =====
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            isDragging = true;
        }
        else if (action == GLFW_RELEASE) {
            isDragging = false;
            firstMouse = true;
        }
    }
}

// ===== MOUSE MOVE =====
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (!isDragging) return;

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float dx = xpos - lastX;
    float dy = ypos - lastY;

    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.3f;
    rotY += dx * sensitivity;
    rotX += dy * sensitivity;
}

// ===== SHADER =====
unsigned int createShader() {
    const char* vertexSrc = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        uniform mat4 MVP;
        void main() {
            gl_Position = MVP * vec4(aPos, 1.0);
        }
    )";

    const char* fragmentSrc = R"(
        #version 330 core
        out vec4 FragColor;
        void main() {
            FragColor = vec4(1.0, 0.5, 0.2, 1.0);
        }
    )";

    unsigned int v = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(v, 1, &vertexSrc, NULL);
    glCompileShader(v);

    unsigned int f = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(f, 1, &fragmentSrc, NULL);
    glCompileShader(f);

    unsigned int shader = glCreateProgram();
    glAttachShader(shader, v);
    glAttachShader(shader, f);
    glLinkProgram(shader);

    glDeleteShader(v);
    glDeleteShader(f);

    return shader;
}

// ===== LOAD STL =====
void loadSTL(const std::string& path, std::vector<float>& vertices) {
    std::ifstream file(path, std::ios::binary);

    if (!file) {
        std::cout << "Cannot open STL file!\n";
        return;
    }

    char header[80];
    file.read(header, 80);

    unsigned int triangleCount;
    file.read(reinterpret_cast<char*>(&triangleCount), 4);

    std::cout << "Triangle count: " << triangleCount << std::endl;

    for (unsigned int i = 0; i < triangleCount; i++) {
        float normal[3];
        file.read(reinterpret_cast<char*>(normal), 12);

        float v[9];
        file.read(reinterpret_cast<char*>(v), 36);

        for (int j = 0; j < 9; j++) {
            vertices.push_back(v[j]);
        }

        char attr[2];
        file.read(attr, 2);
    }

    std::cout << "Vertices loaded: " << vertices.size()/3 << std::endl;
}

// ===== MAIN =====
int main() {
    glfwInit();

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Deep Sea Room", NULL, NULL);
    glfwMakeContextCurrent(window);

    // CALLBACK
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    gladLoadGL();
    glEnable(GL_DEPTH_TEST);

    unsigned int shader = createShader();

    // ===== LOAD MODEL =====
    std::vector<float> vertices;
    loadSTL("../assets/house_demo-Body.stl", vertices);

    if (vertices.empty()) {
        std::cout << "ERROR: STL not loaded!\n";
        return -1;
    }

    // ===== VAO VBO =====
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // ===== LOOP =====
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.0f, 0.05f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader);

        // ===== MODEL =====
        glm::mat4 model = glm::mat4(1.0f);

        // fix STL orientation
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1,0,0));

        // rotate by mouse
        model = glm::rotate(model, glm::radians(rotX), glm::vec3(1,0,0));
        model = glm::rotate(model, glm::radians(rotY), glm::vec3(0,1,0));

        // scale
        model = glm::scale(model, glm::vec3(0.01f));

        glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, glm::vec3(0,1,0));

        glm::mat4 proj = glm::perspective(glm::radians(45.0f),
            (float)SCR_WIDTH / SCR_HEIGHT,
            0.1f, 100.0f);

        glm::mat4 MVP = proj * view * model;

        glUniformMatrix4fv(glGetUniformLocation(shader, "MVP"),
            1, GL_FALSE, glm::value_ptr(MVP));

        glBindVertexArray(VAO);

        glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
