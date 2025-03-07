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
#include "../tokenizer/tokenizer.hpp"
#include <variant>
#include <vector>

#ifndef PARSER_HPP
#define PARSER_HPP

struct NodeTermInteger {
    Token* integer;
};

struct NodeTermIdentifier {
    Token* identifer;
};

struct NodeTerm {
    std::variant<NodeTermInteger*, NodeTermIdentifier*> node;
};

struct NodeStatementExit {
    NodeTerm* term;
};

struct NodeStatementLet {
    Token* identifer;
    NodeTerm* term;
};

struct NodeStatementConst {
    Token* identifer;
    NodeTerm* term;
};

struct NodeStatementAssign {
    Token* identifer;
    NodeTerm* term;
};

struct NodeStatementPrint {
    NodeTerm* term;
};

struct NodeStatement {
    std::variant<NodeStatementExit*, NodeStatementLet*, NodeStatementConst*, NodeStatementAssign*,
                 NodeStatementPrint*>
        node;
};

struct Program {
    std::vector<NodeStatement*> statements;
};

class Parser {
  public:
    explicit Parser(const std::vector<Token*> tokens)
        : m_tokens(std::move(tokens)), m_allocator(1024 * 1024 * 1024), m_error_handler() {}

    [[nodiscard]] Program parse();

  private:
    ArenaAllocator m_allocator;
    ErrorHandler m_error_handler;

    const std::vector<Token*> m_tokens;
    size_t index = 0;

    [[nodiscard]] std::optional<Token*> peek(const size_t = 0);
    Token* consume();
    Token* try_consume(TokenType);

    [[nodiscard]] NodeTerm* parse_term();
    [[nodiscard]] NodeStatement* parse_statement();
};

#endif
