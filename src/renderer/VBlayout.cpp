#include "renderer/VBlayout.h"

template<>
void VBlayout::push<float>(unsigned int count) {
    elements.push_back({ GL_FLOAT, count, GL_FALSE });
    stride += VBelement::getSize(GL_FLOAT) * count;
}

template<>
void VBlayout::push<int>(unsigned int count) {
    elements.push_back({ GL_INT, count, GL_FALSE });
    stride += VBelement::getSize(GL_INT) * count;
}

template<>
void VBlayout::push<unsigned char>(unsigned int count) {
    elements.push_back({ GL_UNSIGNED_BYTE, count, GL_FALSE });
    stride += VBelement::getSize(GL_UNSIGNED_BYTE) * count;
}
