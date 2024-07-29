#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <vector>

#include <GLFW/glfw3.h>
#include <gl/GL.h>

struct Primitive {
    std::vector<float> vertices; // координати вершин примітива
    float red, green, blue; // колір примітива
};

std::vector<Primitive> primitives; // контейнер для зберігання примітивів

float click_x = -1, click_y = -1;
float current_red = 1.0f, current_green = 0.0f, current_blue = 0.0f;

void drawSquare(float x, float y, float red, float green, float blue) {
    float square_size = 50.0f;

    glColor3f(red, green, blue);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + square_size, y);
    glVertex2f(x + square_size, y + square_size);
    glVertex2f(x, y + square_size);
    glEnd();
}

void render_loop() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPointSize(10);
    glLineWidth(2.5);

    // Малювання всіх збережених примітивів
    for (const auto& primitive : primitives) {
        drawSquare(primitive.vertices[0], primitive.vertices[1], primitive.red, primitive.green, primitive.blue);
    }

    // Малювання примітива, якщо був клік
    if (click_x != -1 && click_y != -1) {
        drawSquare(click_x, click_y, current_red, current_green, current_blue);
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        // Визначаємо центр квадрата в місці кліку курсором
        float square_size = 50.0f; // Розмір квадрата
        click_x = (float)xpos - square_size / 2.0f; // Зсуваємо центр на половину розміру квадрата
        click_y = (float)800 - ypos - square_size / 2.0f; // Зсуваємо центр на половину розміру квадрата
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        // Додавання нового примітива в контейнер при натисканні правої кнопки миші
        if (click_x != -1 && click_y != -1) {
            Primitive new_primitive;
            new_primitive.vertices = { click_x, click_y };
            new_primitive.red = current_red;
            new_primitive.green = current_green;
            new_primitive.blue = current_blue;
            primitives.push_back(new_primitive);
            click_x = -1;
            click_y = -1;
        }
    }
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_C: // Зміна кольору на кнопці 'c'
            current_red = static_cast<float>(rand()) / RAND_MAX;
            current_green = static_cast<float>(rand()) / RAND_MAX;
            current_blue = static_cast<float>(rand()) / RAND_MAX;
            break;
        case GLFW_KEY_UP: // Зміна координати верхньої лівої вершини примітива вгору
            if (click_y != -1)
                click_y += 10.0f;
            break;
        case GLFW_KEY_DOWN: // Зміна координати верхньої лівої вершини примітива вниз
            if (click_y != -1)
                click_y -= 10.0f;
            break;
        case GLFW_KEY_RIGHT: // Зміна координати верхньої лівої вершини примітива вправо
            if (click_x != -1)
                click_x += 10.0f;
            break;
        case GLFW_KEY_LEFT: // Зміна координати верхньої лівої вершини примітива вліво
            if (click_x != -1)
                click_x -= 10.0f;
            break;
        case GLFW_KEY_P: // Видалення останнього примітива кнопкою 'P'
            if (!primitives.empty())
                primitives.pop_back();
            break;
        default:
            break;
        }
    }
}

int main() {
    GLFWwindow* window;

    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    window = glfwCreateWindow(1000, 800, "Lab1", NULL, NULL);

    if (!window) {
        std::cout << "Failed to create window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, key_callback);

    glfwSwapInterval(1);

    glViewport(0, 0, 1000, 800);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0.0, 1000.0, 0.0, 800.0, 0.0, 1.0);

    while (!glfwWindowShouldClose(window)) {
        render_loop();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}