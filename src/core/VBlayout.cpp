#include "core/VBlayout.h"

template<typename T>
void VBlayout::push(unsigned int count) {
}

template<>
void VBlayout::push<float>(unsigned int count) {
    elements.push_back({ GL_FLOAT, count, GL_FALSE });
    stride += VBelement::getSize(GL_FLOAT) * count;
}

template<>
void VBlayout::push<unsigned int>(unsigned int count) {
    elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE });
    stride += VBelement::getSize(GL_UNSIGNED_INT) * count;
}

template<>
void VBlayout::push<unsigned char>(unsigned int count) {
    elements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE });
    stride += VBelement::getSize(GL_UNSIGNED_BYTE) * count;
}
