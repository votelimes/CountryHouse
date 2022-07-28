#define STB_IMAGE_IMPLEMENTATION
#define aiProcessPreset_TargetRealtime_Fast

#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/normal.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h"
#include "camera.h"
#include "model.h"
#include "mesh.h"
#include "dayLight.h"
#include <iostream>


// Объявление функций
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// Константы
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Камера
Camera camera(glm::vec3(0.0f, 10.0f, 50.0f));
Camera camera2(glm::vec3(0.0f, 10.0f, 50.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// Тайминги
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Класс обсл. день-ночь
DayLight dayLight;

// Другое
bool flashlightEnabled = false;
float fps = 0.0f;

int main()
{
    // glfw: инициализация и конфигурирование
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    // glfw: создание окна
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Country House", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Сообщаем GLFW, чтобы он захватил наш курсор
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // // glad: загрузка всех указателей на OpenGL-функции
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Сообщаем stb_image.h, чтобы он перевернул загруженные текстуры относительно y-оси (до загрузки модели)
    stbi_set_flip_vertically_on_load(true);

    // Конфигурирование глобального состояния OpenGL (отбрасывает невидимые камере вершины до фрагментного шейдера)
    glEnable(GL_DEPTH_TEST);

    // Компилирование нашей шейдерной программы
    Shader coreProgram("shaders//default.vs", "shaders//default.fs");
    Shader sunProgram("shaders//sun.vs", "shaders//sun.fs");

    // Загрузка моделей
    Model houseDay("resources//house.obj");
    
    Model sun("resources//sun.obj");

    // Координаты точечных источников света
    glm::vec3 pointLightPositions[3] = {
        glm::vec3(-3.80567f, 0.347255f, 1.46787f),
        glm::vec3(0.965005f, 1.84552f, 5.74475f),
        glm::vec3(1.185005f, 1.84552f, 5.74475f)
    };

    // Оконный свет 1
    coreProgram.use();
    coreProgram.setVec3("pointLights[0].position", pointLightPositions[0]);
    coreProgram.setFloat("pointLights[0].constant", dayLight.window1Light.constant);
    coreProgram.setFloat("pointLights[0].linear", dayLight.window1Light.linear);
    coreProgram.setFloat("pointLights[0].quadratic", dayLight.window1Light.quadratic);
    
    // Оконный свет 2
    coreProgram.setVec3("pointLights[1].position", pointLightPositions[1]);
    coreProgram.setFloat("pointLights[1].constant", dayLight.window2Light.constant);
    coreProgram.setFloat("pointLights[1].linear", dayLight.window2Light.linear);
    coreProgram.setFloat("pointLights[1].quadratic", dayLight.window2Light.quadratic);
    
    // Оконный свет 3
    coreProgram.setVec3("pointLights[2].position", pointLightPositions[2]);
    coreProgram.setFloat("pointLights[2].constant", dayLight.window3Light.constant);
    coreProgram.setFloat("pointLights[2].linear", dayLight.window3Light.linear);
    coreProgram.setFloat("pointLights[2].quadratic", dayLight.window3Light.quadratic);

    // Фонарь
    coreProgram.setFloat("flashlight.constant", dayLight.flashlight.constant);
    coreProgram.setFloat("flashlight.linear", dayLight.flashlight.linear);
    coreProgram.setFloat("flashlight.quadratic", dayLight.flashlight.quadratic);   
    coreProgram.setFloat("flashlight.cutOff", dayLight.flashlight.cutOff);
    coreProgram.setFloat("flashlight.outerCutOff", dayLight.flashlight.outerCutOff);

    // Солнце
    coreProgram.setFloat("sunLight.constant", dayLight.sunLight.constant);
    coreProgram.setFloat("sunLight.linear", dayLight.sunLight.linear);
    coreProgram.setFloat("sunLight.quadratic", dayLight.sunLight.quadratic);   
    coreProgram.setFloat("sunLight.cutOff", dayLight.sunLight.cutOff);
    coreProgram.setFloat("sunLight.outerCutOff", dayLight.sunLight.outerCutOff);

    // Цикл рендеринга
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        glClearColor(dayLight.bgColor.r, dayLight.bgColor.g, dayLight.bgColor.b, 1.0f);
        
        // Рендеринг
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projectionMatrix = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 200.0f);
        glm::mat4 viewMatrix = camera.GetViewMatrix();
        
        // Логическая часть работы со временем для каждого кадра
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        fps = 1.0f / deltaTime;

        dayLight.flashlightEnabled = flashlightEnabled;
        dayLight.proceedAmbient(deltaTime, camera.Position);
        
        // Активация другой программы (для отрисовки небесных тел)
        sunProgram.use();
        sunProgram.setMat4("projection", projectionMatrix);
        sunProgram.setMat4("view", viewMatrix);

        dayLight.proceedSunMoon(deltaTime);

        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(dayLight.sunX, dayLight.sunY, dayLight.sunZ));
        // Солнце
        sunProgram.setMat4("model", modelMatrix);
        sun.Draw(sunProgram);
        // Луна
        modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(dayLight.moonX, dayLight.moonY, dayLight.moonZ));
        sunProgram.setMat4("model", modelMatrix);
        sun.Draw(sunProgram);

        // Обработка ввода
        processInput(window);

        // Убеждаемся, что активировали шейдер прежде, чем настраивать uniform-переменные/объекты_рисования
        coreProgram.use();

        // Преобразования Вида/Проекции
        coreProgram.setMat4("projection", projectionMatrix);
        coreProgram.setMat4("view", viewMatrix);

        // Рендеринг загруженной модели
        modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.0f, 0.0f)); // смещаем вниз чтобы быть в центре сцены
        modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f, 1.0f, 1.0f));	// объект слишком большой для нашей сцены, поэтому немного уменьшим его
        
        coreProgram.setVec3("viewPos", camera.Position);
        
        coreProgram.setMat4("model", modelMatrix);

        // Туман
        coreProgram.setVec3("SkyColor", dayLight.bgColor.r, dayLight.bgColor.g, dayLight.bgColor.b);
		
        // Туман
        coreProgram.setFloat("density", dayLight.fog);

        // Точечный источник света №1
        coreProgram.setVec3("pointLights[0].ambient", dayLight.window1Light.ambient);
        coreProgram.setVec3("pointLights[0].diffuse", dayLight.window1Light.diffuse);
        coreProgram.setVec3("pointLights[0].specular", dayLight.window1Light.specular);
		
        // Точечный источник света №2
        coreProgram.setVec3("pointLights[1].ambient", dayLight.window2Light.ambient);
        coreProgram.setVec3("pointLights[1].diffuse", dayLight.window2Light.diffuse);
        coreProgram.setVec3("pointLights[1].specular", dayLight.window2Light.specular);

        // Точечный источник света №3
        coreProgram.setVec3("pointLights[2].ambient", dayLight.window3Light.ambient);
        coreProgram.setVec3("pointLights[2].diffuse", dayLight.window3Light.diffuse);
        coreProgram.setVec3("pointLights[2].specular", dayLight.window3Light.specular);

        // Фонарь
        coreProgram.setVec3("flashlight.position", camera.Position);
        coreProgram.setVec3("flashlight.direction", camera.Front);
        coreProgram.setVec3("flashlight.ambient", dayLight.flashlight.ambient);
        coreProgram.setVec3("flashlight.diffuse", dayLight.flashlight.diffuse);
        coreProgram.setVec3("flashlight.specular", dayLight.flashlight.specular);

        // Солнце
        coreProgram.setVec3("sunLight.position", dayLight.sunLight.position);
        coreProgram.setVec3("sunLight.direction", dayLight.sunLight.direction);
        coreProgram.setVec3("sunLight.ambient", dayLight.sunLight.ambient);
        coreProgram.setVec3("sunLight.diffuse", dayLight.sunLight.diffuse);
        coreProgram.setVec3("sunLight.specular", dayLight.sunLight.specular);  
        houseDay.Draw(coreProgram);

        // glfw: обмен содержимым front- и back- буферов. Отслеживание событий ввода/вывода (была ли нажата/отпущена кнопка, перемещен курсор мыши и т.п.)
        glfwSwapBuffers(window);
    }

    // glfw: завершение, освобождение всех выделенных ранее GLFW-реcурсов
    glfwTerminate();
    return 0;
}

// Обработка всех событий ввода: запрос GLFW о нажатии/отпускании кнопки мыши в данном кадре и соответствующая обработка данных событий
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    float deltaTimeBias = 0.01;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime + deltaTimeBias);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime + deltaTimeBias);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime + deltaTimeBias);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime + deltaTimeBias);
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)    
        flashlightEnabled = true;
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)    
        flashlightEnabled = false;
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)    
        std::cout << "LOG: FPS: " << fps << std::endl;
}

// glfw: всякий раз, когда изменяются размеры окна (пользователем или операционной системой), вызывается данная callback-функция
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // Убеждаемся, что окно просмотра соответствует новым размерам окна.
    glViewport(0, 0, width, height);
}

// glfw: всякий раз, когда перемещается мышь, вызывается данная callback-функция
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // перевернуто, так как y-координаты идут снизу вверх

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: всякий раз, когда прокручивается колесико мыши, вызывается данная callback-функция
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}