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

#include "tokenizer.hpp"

#include <cctype>
#include <cstdlib>
#include <sstream>
#include <utility>

[[nodiscard]] std::string to_string(TokenType token_type) {
    switch (token_type) {
    case TokenType::integer:
        return "integer";
    case TokenType::exit:
        return "exit";
    case TokenType::let:
        return "let";
    case TokenType::_const:
        return "const";
    case TokenType::print:
        return "print";
    case TokenType::identifier:
        return "identifier";
    case TokenType::open_brace:
        return "`(`";
    case TokenType::close_brace:
        return "`)`";
    case TokenType::equals:
        return "`=`";
    case TokenType::semi_colon:
        return "`;`";
    default:
        ErrorHandler().info("Unimplimented to_string for given TokenType");
        return "";
    }
}

Tokenizer::Tokenizer(const std::string source_contents)
    : m_contents(std::move(source_contents)), m_allocator(1024 * 1024 * 4), m_error_handler() {}

[[nodiscard]] std::vector<Token*> Tokenizer::tokenize() {
    std::string buffer;

    while (peek().has_value()) {
        if (std::isalpha(peek().value()) || peek().value() == '_') {
            buffer.push_back(consume());
            while (peek().has_value() && (std::isalpha(peek().value()) || peek().value() == '_')) {
                buffer.push_back(consume());
            }
            size_t buffer_length = buffer.length();
            if (buffer == "exit") {
                push(TokenType::exit, m_line_number, m_column_number);
                buffer.clear();
            } else if (buffer == "let") {
                push(TokenType::let, m_line_number, m_column_number);
                buffer.clear();
            } else if (buffer == "const") {
                push(TokenType::_const, m_line_number, m_column_number);
                buffer.clear();
            } else if (buffer == "print") {
                push(TokenType::print, m_line_number, m_column_number);
                buffer.clear();
            } else {
                push(TokenType::identifier, m_line_number, m_column_number, buffer);
                buffer.clear();
            }
            m_column_number += buffer_length;
        } else if (std::isdigit(peek().value())) {
            buffer.push_back(consume());
            size_t buffer_length = buffer.length();
            while (peek().has_value() && std::isdigit(peek().value())) {
                buffer.push_back(consume());
            }
            push(TokenType::integer, m_line_number, m_column_number, buffer);
            m_column_number += buffer_length;
            buffer.clear();
        } else if (peek().value() == '(') {
            consume();
            push(TokenType::open_brace, m_line_number, m_column_number);
            m_column_number++;
        } else if (peek().value() == ')') {
            consume();
            push(TokenType::close_brace, m_line_number, m_column_number);
            m_column_number++;
        } else if (peek().value() == ';') {
            consume();
            push(TokenType::semi_colon, m_line_number, m_column_number);
            m_column_number++;
        } else if (peek().value() == '=') {
            consume();
            push(TokenType::equals, m_line_number, m_column_number);
            m_column_number++;
        } else if (peek().value() == ' ') {
            consume();
            m_column_number++;
        } else if (peek().value() == '\n') {
            consume();
            m_line_number++;
            m_column_number = 1;
        } else {
            std::stringstream ss;
            ss << "Invalid Token `" << peek().value() << "`";
            m_error_handler.error(ss.str());
            exit(EXIT_FAILURE);
        }
    }

    return m_tokens;
}

[[nodiscard]] std::optional<char> Tokenizer::peek(const size_t offest) {
    if (m_index + offest >= m_contents.length()) {
        return {};
    }
    return m_contents.at(m_index + offest);
}

char Tokenizer::consume() { return m_contents.at(m_index++); }

template <typename... Args> void Tokenizer::push(Args&&... args) {
    auto token = m_allocator.alloc<Token>(std::forward<Args>(args)...);
    m_tokens.push_back(token);
}
