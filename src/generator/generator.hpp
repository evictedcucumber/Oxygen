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

#include "../parser/parser.hpp"
#include <sstream>
#include <string>

#ifndef GENERATOR_HPP
#define GENERATOR_HPP

class Generator {
  public:
    explicit Generator(const Program program) : m_program(std::move(program)) {}

    [[nodiscard]] std::string generate();

  private:
    const Program m_program;
    std::stringstream m_output;
    size_t index = 0;
    size_t m_unique_index = 0;

    struct Variable {
        std::string indentifier;
        bool is_const;
        size_t location;
    };

    std::vector<Variable> m_stack{};
    size_t m_stack_size = 0;

    void push(const std::string&);
    void pop(const std::string&);

    void gen_term(const NodeTerm*);
    void gen_statement(const NodeStatement*);
};

#endif
