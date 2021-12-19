#ifndef MINECRAFT_CLONE_EBO_H
#define MINECRAFT_CLONE_EBO_H

#include "core.h"

class EBO {
    public:
        GLuint ID, count;
        
        EBO(const GLuint *indices, const GLuint count);
        
        void bind() const;
        void unbind() const;
        
        inline GLuint getCount() const { return count; };
        
        static void free(EBO *ebo);
};

#endif
