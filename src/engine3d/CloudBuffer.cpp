#include "engine3d/CloudBuffer.h"

#include <utility>

void CloudBuffer::init() {
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vboA_);
    glGenBuffers(1, &vboB_);
    bindAttribs();
}

void CloudBuffer::destroy() {
    glDeleteBuffers(1, &vboA_);
    glDeleteBuffers(1, &vboB_);
    glDeleteVertexArrays(1, &vao_);
    vao_ = vboA_ = vboB_ = 0;
}

// Attribute layout per point: x y z | density sign   (5 floats)
//   locations 0,1 read buffer A (current)   locations 2,3 read buffer B (morph target)
void CloudBuffer::bindAttribs() {
    const GLsizei stride = OrbitalCloud::kFloatsPerPoint * sizeof(float);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vboA_);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glBindBuffer(GL_ARRAY_BUFFER, vboB_);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glBindVertexArray(0);
}

void CloudBuffer::upload(GLuint vbo, const OrbitalCloud& cloud) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, cloud.data.size() * sizeof(float), cloud.data.data(), GL_STATIC_DRAW);
}

void CloudBuffer::setImmediate(const OrbitalCloud& cloud) {
    upload(vboA_, cloud);
    upload(vboB_, cloud);
    count_ = cloud.count();
    morphing_ = false;
}

bool CloudBuffer::beginMorph(const OrbitalCloud& cloud) {
    if (cloud.count() != count_) { setImmediate(cloud); return false; }
    upload(vboB_, cloud);
    morphing_ = true;
    return true;
}

void CloudBuffer::finishMorph() {
    if (!morphing_) return;
    std::swap(vboA_, vboB_);     // the target becomes the current cloud
    bindAttribs();               // re-point the VAO at the swapped buffers
    morphing_ = false;
}

void CloudBuffer::draw() const {
    glBindVertexArray(vao_);
    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(count_));
    glBindVertexArray(0);
}
