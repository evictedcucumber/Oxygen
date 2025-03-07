#pragma once

#include <cstddef>
#include <memory>
#include <utility>

#ifndef ARENA_ALLOCATOR_HPP
#define ARENA_ALLOCATOR_HPP

class ArenaAllocator {
  public:
    ArenaAllocator(const size_t max_num_bytes)
        : m_size(std::move(max_num_bytes)), m_buffer(new std::byte[m_size]), m_offset(m_buffer) {}

    ~ArenaAllocator() { delete[] m_buffer; }

    template <typename T, typename... Args> [[nodiscard]] T* alloc(Args&&... args) {
        size_t remaining_num_bytes = m_size - static_cast<size_t>(m_offset - m_buffer);
        auto pointer = static_cast<void*>(m_offset);
        const auto aligned_address =
            std::align(alignof(T), sizeof(T), pointer, remaining_num_bytes);
        if (aligned_address == nullptr) {
            throw std::bad_alloc{};
        }
        m_offset = static_cast<std::byte*>(aligned_address) + sizeof(T);
        const auto allocated_memory = static_cast<T*>(aligned_address);
        return new (allocated_memory) T{std::forward<Args>(args)...};
    }

  private:
    size_t m_size;
    std::byte* m_buffer;
    std::byte* m_offset;
};

#endif
