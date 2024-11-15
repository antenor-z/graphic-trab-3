#include "disk.h"

#ifdef _WIN32
#include <glad/glad.h>
#elif __linux__
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#else
#include <OpenGL/gl3.h>
#endif
#include <cmath>
#include <vector>
#include <iostream>
#include <vector>
#include <memory>

DiskPtr Disk::Make(int nslice) {
    return DiskPtr(new Disk(nslice));
}

Disk::Disk(int nslice) : m_nslice(nslice), m_vao(0) {
    std::vector<float> vertices;
    std::vector<float> texcoord;
    std::vector<unsigned int> inc;

    // vertices.push_back(0.0f);  // Center vertex
    // vertices.push_back(0.0f);
    // texcoord.push_back(0.5f); // Center tex coord
    // texcoord.push_back(0.5f);

    float angleStep = 2.0f * M_PI / m_nslice;
    for (int i = 0; i <= m_nslice; i++) {
        float angle = i * angleStep;
        float x = cos(angle);
        float y = sin(angle);
        vertices.push_back(x);
        vertices.push_back(y);
        
        texcoord.push_back(0.5f + 0.5f * cos(angle));
        texcoord.push_back(0.5f + 0.5f * sin(angle));
    }

    for (int i = 1; i <= m_nslice; i++) {
        inc.push_back(0);
        inc.push_back(i);
        inc.push_back(i + 1);
    }

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    GLuint vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);  // position
    glEnableVertexAttribArray(0);

    GLuint texCoordBuffer;
    glGenBuffers(1, &texCoordBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
    glBufferData(GL_ARRAY_BUFFER, texcoord.size() * sizeof(float), texcoord.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);  // texture coordinates
    glEnableVertexAttribArray(1);

    GLuint indexBuffer;
    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, inc.size() * sizeof(unsigned int), inc.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
}

Disk::~Disk() {
    glDeleteVertexArrays(1, &m_vao);
}

void Disk::Draw(StatePtr) {
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, 3 * m_nslice, GL_UNSIGNED_INT, 0);
    //glBindVertexArray(0);
}
