#ifndef MUNCHKIN_RENDERER_UNIFORM_BUFFER_HPP_
#define MUNCHKIN_RENDERER_UNIFORM_BUFFER_HPP_

#include <glad/glad.h>
#include <cstddef>

namespace munchkin {

namespace renderer {

class UniformBuffer {
public:
    UniformBuffer(std::size_t binding, std::size_t initial_size = 0, GLenum mode = GL_STATIC_DRAW);
    ~UniformBuffer();

    unsigned int handle();

    std::size_t get_binding() const;

    static void bind(UniformBuffer& buf);

    // Assumes the buffer has been bound
    void write_data(float const* data, std::size_t byte_size, std::size_t byte_offset = 0);

private:
    unsigned int ubo = 0;
    std::size_t binding = 0;
};

} // namespace renderer

} // namespace munchkin

#endif