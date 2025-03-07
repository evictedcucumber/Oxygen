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

#include "parser.hpp"
#include <cstdlib>
#include <iostream>
#include <optional>
#include <vector>

[[nodiscard]] std::optional<Token*> Parser::peek(const size_t offset) {
    if (index + offset >= m_tokens.size()) {
        return {};
    }

    return m_tokens.at(index + offset);
}

Token* Parser::consume() { return m_tokens.at(index++); }

Token* Parser::try_consume(TokenType token_type) {
    if (!peek().has_value()) {
        m_error_handler.error_expected(to_string(token_type), peek(-1).value()->line_number);
        exit(EXIT_FAILURE);
    }
    if (peek().value()->token_type != token_type) {
        m_error_handler.error_expected(to_string(token_type), peek().value()->line_number);
        exit(EXIT_FAILURE);
    }

    return consume();
}

[[nodiscard]] NodeTerm* Parser::parse_term() {
    if (peek().has_value() && peek().value()->token_type == TokenType::integer) {
        auto node_term_integer = m_allocator.alloc<NodeTermInteger>(consume());
        return m_allocator.alloc<NodeTerm>(node_term_integer);
    } else if (peek().has_value() && peek().value()->token_type == TokenType::identifier) {
        auto node_term_identifier = m_allocator.alloc<NodeTermIdentifier>(consume());
        return m_allocator.alloc<NodeTerm>(node_term_identifier);
    }

    std::cerr << "[Parser Error] Unable to parse term" << std::endl;
    exit(EXIT_FAILURE);
}

[[nodiscard]] NodeStatement* Parser::parse_statement() {
    if (peek().has_value() && peek().value()->token_type == TokenType::exit &&
        peek(1).has_value() && peek(1).value()->token_type == TokenType::open_brace) {
        consume();
        consume();

        auto node_term = parse_term();

        try_consume(TokenType::close_brace);
        try_consume(TokenType::semi_colon);

        auto node_statement_exit = m_allocator.alloc<NodeStatementExit>(node_term);
        return m_allocator.alloc<NodeStatement>(node_statement_exit);
    } else if (peek().has_value() && peek().value()->token_type == TokenType::let) {
        consume();

        Token* identifier = try_consume(TokenType::identifier);

        try_consume(TokenType::equals);

        auto node_term = parse_term();

        try_consume(TokenType::semi_colon);

        auto node_statement_let = m_allocator.alloc<NodeStatementLet>(identifier, node_term);
        return m_allocator.alloc<NodeStatement>(node_statement_let);
    } else if (peek().has_value() && peek().value()->token_type == TokenType::_const) {
        consume();

        Token* identifier = try_consume(TokenType::identifier);

        try_consume(TokenType::equals);

        auto node_term = parse_term();

        try_consume(TokenType::semi_colon);

        auto node_statement_const = m_allocator.alloc<NodeStatementConst>(identifier, node_term);
        return m_allocator.alloc<NodeStatement>(node_statement_const);
    } else if (peek().has_value() && peek().value()->token_type == TokenType::identifier &&
               peek(1).has_value() && peek(1).value()->token_type == TokenType::equals) {
        Token* identifier = consume();

        consume();

        auto node_term = parse_term();

        try_consume(TokenType::semi_colon);

        auto node_statement_assign = m_allocator.alloc<NodeStatementAssign>(identifier, node_term);
        return m_allocator.alloc<NodeStatement>(node_statement_assign);
    } else if (peek().has_value() && peek().value()->token_type == TokenType::print &&
               peek(1).has_value() && peek(1).value()->token_type == TokenType::open_brace) {
        consume();
        consume();

        auto node_term = parse_term();

        try_consume(TokenType::close_brace);
        try_consume(TokenType::semi_colon);

        auto node_statement_print = m_allocator.alloc<NodeStatementPrint>(node_term);
        return m_allocator.alloc<NodeStatement>(node_statement_print);
    }

    std::cerr << "[Parser Error] Unable to parse statement" << std::endl;
    exit(EXIT_FAILURE);
}

[[nodiscard]] Program Parser::parse() {
    std::vector<NodeStatement*> statements;

    while (peek().has_value()) {
        if (NodeStatement* statement = parse_statement()) {
            statements.push_back(statement);
        }
    }

    return Program{.statements = statements};
}
