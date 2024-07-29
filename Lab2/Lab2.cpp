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
    float size; // розмір примітива
};

std::vector<Primitive> primitives; // контейнер для зберігання примітивів

float click_x = -1, click_y = -1;
float current_red = 1.0f, current_green = 0.0f, current_blue = 0.0f;
float current_size = 50.0f; // початковий розмір квадрата
bool xor_mode = false; // режим XOR
int grid_size = 50; // розмір сітки (значення за замовчуванням)
bool outline_only_mode = false; // режим тільки контур фігури
bool grid_enabled = true; // чи ввімкнена сітка

void drawSquare(float x, float y, float size, float red, float green, float blue, bool outline_only_mode) {
    if (xor_mode) {
        glEnable(GL_COLOR_LOGIC_OP);
        glLogicOp(GL_XOR);
    }

    if (outline_only_mode) {
        glColor3f(1.0f - red, 1.0f - green, 1.0f - blue); // обернені кольори для контуру
        glLineWidth(2.0f);
        glBegin(GL_LINE_LOOP);
        glVertex2f(x, y);
        glVertex2f(x + size, y);
        glVertex2f(x + size, y + size);
        glVertex2f(x, y + size);
        glEnd();
    }
    else {
        glColor3f(red, green, blue);
        glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + size, y);
        glVertex2f(x + size, y + size);
        glVertex2f(x, y + size);
        glEnd();
    }

    if (xor_mode) {
        glDisable(GL_COLOR_LOGIC_OP);
    }
}

void render_loop() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPointSize(10);
    glLineWidth(2.5);

    // Відображення сітки умовних пікселів, якщо вона увімкнена
    if (grid_enabled) {
        glColor3f(0.5f, 0.5f, 0.5f);
        for (int i = 0; i <= 1000; i += grid_size) {
            glBegin(GL_LINES);
            glVertex2f(i, 0);
            glVertex2f(i, 800);
            glEnd();
            glBegin(GL_LINES);
            glVertex2f(0, i);
            glVertex2f(1000, i);
            glEnd();
        }
    }

    // Малювання всіх збережених примітивів
    for (const auto& primitive : primitives) {
        drawSquare(primitive.vertices[0], primitive.vertices[1], primitive.size, primitive.red, primitive.green, primitive.blue, outline_only_mode);
    }

    // Малювання примітива, якщо був клік
    if (click_x != -1 && click_y != -1) {
        drawSquare(click_x, click_y, current_size, current_red, current_green, current_blue, outline_only_mode);
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        // Визначаємо центр квадрата в місці кліку курсором
        click_x = (float)xpos - current_size / 2.0f; // Зсуваємо центр на половину розміру квадрата
        click_y = (float)800 - ypos - current_size / 2.0f; // Зсуваємо центр на половину розміру квадрата
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        // Додавання нового примітива в контейнер при натисканні правої кнопки миші
        if (click_x != -1 && click_y != -1) {
            Primitive new_primitive;
            new_primitive.vertices = { click_x, click_y };
            new_primitive.red = current_red;
            new_primitive.green = current_green;
            new_primitive.blue = current_blue;
            new_primitive.size = current_size;
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
        case GLFW_KEY_M: // Збільшення розміру останнього примітива кнопкою 'm'
            current_size += 10.0f;
            break;
        case GLFW_KEY_N: // Зменшення розміру останнього примітива кнопкою 'n'
            current_size = std::max(current_size - 10.0f, 10.0f);
            break;
        case GLFW_KEY_COMMA: // Включення режиму XOR кнопкою '>'
            xor_mode = true;
            break;
        case GLFW_KEY_PERIOD: // Виключення режиму XOR кнопкою '<'
            xor_mode = false;
            break;
        case GLFW_KEY_SEMICOLON: // Включення режиму тільки контур фігури кнопкою ';'
            outline_only_mode = true;
            break;
        case GLFW_KEY_APOSTROPHE: // Вимкнення режиму тільки контур фігури кнопкою '''
            outline_only_mode = false;
            break;
        case GLFW_KEY_BACKSLASH: // Забирання сітки на кнопку '\'
            grid_enabled = !grid_enabled;
            break;
        case GLFW_KEY_LEFT_BRACKET: // Зменшення розміру сітки на кнопку '['
            if (grid_size > 1)
                grid_size -= 1;
            break;
        case GLFW_KEY_RIGHT_BRACKET: // Збільшення розміру сітки на кнопку ']'
            grid_size += 1;
            break;
        case GLFW_KEY_1: // Зміна розміру сітки на 25
            grid_size = 25;
            break;
        case GLFW_KEY_2: // Зміна розміру сітки на 50
            grid_size = 50;
            break;
        case GLFW_KEY_3: // Зміна розміру сітки на 100
            grid_size = 100;
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
