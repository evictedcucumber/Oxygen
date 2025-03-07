/* Copyright (C) 2025 ethan-mcconkey

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>. */

#pragma once

#include "../../include/arena_allocator.hpp"
#include "../error_handling/error_handling.hpp"
#include <optional>
#include <string>
#include <vector>

#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

enum class TokenType {
    exit,
    let,
    _const,
    print,
    identifier,
    open_brace,
    close_brace,
    equals,
    semi_colon,
    integer
};

struct Token {
    TokenType token_type;
    int line_number;
    int column_number;
    std::optional<std::string> value;
};

[[nodiscard]] extern std::string to_string(TokenType);

class Tokenizer {
  public:
    explicit Tokenizer(const std::string);

    [[nodiscard]] std::vector<Token*> tokenize();

  private:
    ArenaAllocator m_allocator;
    ErrorHandler m_error_handler;

    int m_line_number = 1;
    int m_column_number = 1;

    [[nodiscard]] std::optional<char> peek(const size_t = 0);
    char consume();
    template <typename... Args> void push(Args&&...);

    const std::string m_contents;
    std::vector<Token*> m_tokens;
    size_t m_index = 0;
};

#endif
