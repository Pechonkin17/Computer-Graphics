import pygame
from pygame.locals import *
from OpenGL.GL import *
from OpenGL.GLUT import *
from OpenGL.GLU import *
import numpy as np

# Функція для зчитування вершин січення з файлу
def read_vertices(filename):
    vertices = []
    with open(filename, 'r') as file:
        for line in file:
            coords = line.strip().split()
            vertices.append([float(coord) for coord in coords])
    return vertices

# Функція для зчитування траєкторії тиражування з файлу
def read_trajectory(filename):
    trajectory = []
    with open(filename, 'r') as file:
        for line in file:
            coords = line.strip().split()
            trajectory.append([float(coord) for coord in coords])
    return trajectory

# Функція для зчитування параметрів зміни січення з файлу
def read_clipping_parameters(filename):
    with open(filename, 'r') as file:
        parameters = [float(param) for param in file.readline().strip().split()]
    return parameters

# Функція для обчислення нормалей для поверхонь
def calculate_normals(vertices, surfaces):
    normals = []
    for surface in surfaces:
        v0 = np.array(vertices[surface[0]])
        v1 = np.array(vertices[surface[1]])
        v2 = np.array(vertices[surface[2]])
        edge1 = v1 - v0
        edge2 = v2 - v0
        normal = np.cross(edge1, edge2)
        normal /= np.linalg.norm(normal)
        normals.append(normal)
    return normals

# Функція для побудови куба без текстури
def cube(vertices, surfaces):
    glBegin(GL_QUADS)
    for surface in surfaces:
        for vertex_index in surface:
            glVertex3fv(vertices[vertex_index])
    glEnd()

# Функція для побудови куба з текстурою, окремо для кожної сторони
def cube_with_texture(texture_id, vertices, texture_coords, surfaces):
    glBindTexture(GL_TEXTURE_2D, texture_id)
    for i, surface in enumerate(surfaces):
        glBegin(GL_QUADS)
        for j, vertex_index in enumerate(surface):
            glTexCoord2fv(texture_coords[i][j])
            glVertex3fv(vertices[vertex_index])
        glEnd()

# Функція для побудови опуклої фігури з заданими вершинами
def build_convex_shape(vertices):
    glBegin(GL_POLYGON)
    for vertex in vertices:
        glVertex3fv(vertex)
    glEnd()

def main():
    pygame.init()
    display = (800, 600)
    pygame.display.set_mode(display, DOUBLEBUF | OPENGL)
    glEnable(GL_DEPTH_TEST)
    glEnable(GL_LIGHTING)
    glEnable(GL_COLOR_MATERIAL)

    # Зчитування даних з файлів
    vertices = read_vertices('vertices.txt')
    trajectory = read_trajectory('trajectory.txt')
    clipping_parameters = read_clipping_parameters('clipping_parameters.txt')

    # Створення списку поверхонь
    surfaces = [
        [0, 1, 2, 3],
        [3, 2, 7, 6],
        [6, 7, 5, 4],
        [4, 5, 1, 0],
        [1, 5, 7, 2],
        [4, 0, 3, 6]
    ]

    # Визначення текстурних координат
    texture_coords = [
        [[0, 0], [1, 0], [1, 1], [0, 1]],  # Передня поверхня
        [[0, 0], [1, 0], [1, 1], [0, 1]],  # Задня поверхня
        [[0, 0], [1, 0], [1, 1], [0, 1]],  # Ліва поверхня
        [[0, 0], [1, 0], [1, 1], [0, 1]],  # Права поверхня
        [[0, 0], [1, 0], [1, 1], [0, 1]],  # Верхня поверхня
        [[0, 0], [1, 0], [1, 1], [0, 1]],  # Нижня поверхня
    ]

    # Розрахунок нормалей для поверхонь
    normals = calculate_normals(vertices, surfaces)

    # Завантаження текстури
    texture_surface = pygame.image.load("photo.png")
    texture_data = pygame.image.tostring(texture_surface, "RGBA", 1)
    texture_id = glGenTextures(1)
    glBindTexture(GL_TEXTURE_2D, texture_id)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_surface.get_width(), texture_surface.get_height(), 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, texture_data)

    gluPerspective(45, (display[0] / display[1]), 0.1, 50.0)
    glTranslatef(0.0, 0.0, -5)
    smooth_normals = True
    draw_wireframe = False
    draw_normals_flag = False
    draw_texture = True  # Перед входом у цикл відображення включаємо відображення текстури

    glLightfv(GL_LIGHT0, GL_POSITION, (1.0, 1.0, 1.0, 0.0))

    build_convex = False  # Включення/виключення побудови опуклої фігури
    vertices_convex = []  # Список вершин для побудови опуклої фігури

    while True:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                quit()
            elif event.type == pygame.KEYDOWN:
                if event.key == pygame.K_w:
                    glTranslatef(0, 0, 0.2)  # Перемістити об'єкт угору
                elif event.key == pygame.K_s:
                    glTranslatef(0, 0, -0.2)  # Перемістити об'єкт вниз
                elif event.key == pygame.K_a:
                    glTranslatef(-0.2, 0, 0)  # Перемістити об'єкт вліво
                elif event.key == pygame.K_d:
                    glTranslatef(0.2, 0, 0)  # Перемістити об'єкт вправо
                if event.key == pygame.K_UP:
                    glRotatef(10, -2, 0, 0)
                elif event.key == pygame.K_DOWN:
                    glRotatef(10, 2, 0, 0)
                elif event.key == pygame.K_LEFT:
                    glRotatef(10, 0, -2, 0)
                elif event.key == pygame.K_RIGHT:
                    glRotatef(10, 0, 2, 0)
                elif event.key == pygame.K_z:
                    draw_wireframe = not draw_wireframe
                elif event.key == pygame.K_x:
                    draw_normals_flag = not draw_normals_flag
                elif event.key == pygame.K_v:
                    smooth_normals = not smooth_normals
                    if smooth_normals:
                        glShadeModel(GL_SMOOTH)
                    else:
                        glShadeModel(GL_FLAT)
                elif event.key == pygame.K_b:
                    # Код для включення/виключення режиму ортографічної проекції
                    pass  # Додайте сюди свої дії для цієї клавіші
                elif event.key == pygame.K_SPACE:
                    build_convex = not build_convex
                    if build_convex:
                        vertices_convex = [
                            [-1, -1, -1],
                            [-1, 1, -1],
                            [1, 1, -1],
                            [1, -1, -1]
                        ]

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)

        if smooth_normals:
            glEnable(GL_LIGHTING)  # Включення освітлення для згладжених нормалей
            glShadeModel(GL_SMOOTH)
        else:
            glDisable(GL_LIGHTING)  # Вимкнення освітлення для плоских нормалей
            glShadeModel(GL_FLAT)

        if draw_wireframe:
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)
        else:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL)

        if draw_normals_flag:
            glBegin(GL_LINES)
            for i, surface in enumerate(surfaces):
                for vertex_index in surface:
                    center = np.mean([vertices[idx] for idx in surface], axis=0)
                    glVertex3fv(center)
                    normal_length = 10.0
                    normal_endpoint = center + normal_length * normals[i]
                    glVertex3fv(normal_endpoint)
            glEnd()

        if draw_texture:
            glEnable(GL_TEXTURE_2D)
            cube_with_texture(texture_id, vertices, texture_coords, surfaces)
        else:
            glDisable(GL_TEXTURE_2D)
            cube(vertices, surfaces)

        if build_convex:
            build_convex_shape(vertices_convex)

        pygame.display.flip()
        pygame.time.wait(10)

if __name__ == '__main__':
    main()
