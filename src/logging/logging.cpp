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

#include "logging.hpp"
#include <cstdlib>
#include <iostream>
#include <optional>
#include <string>
#include <variant>
#include <vector>

void log_tokens(const std::vector<Token*>& tokens) {
    for (Token* token : tokens) {
        std::cout << "Token:" << token->line_number << ":" << token->column_number << ":("
                  << to_string(token->token_type);
        if (token->value.has_value()) {
            std::cout << ", " << token->value.value();
        }
        std::cout << ")\n";
    }
    std::cout << std::endl;
}

void log_program(const Program& program) {
    struct TermVisitor {
        static void print_term(const std::string term_name, const std::string value) {
            std::cout << "\e[1;34mNodeTerm" << term_name << "\e[0;90m, \e[32m" << value
                      << " \e[90m";
        }
        void operator()(const NodeTermInteger* term) {
            print_term("Integer", term->integer->value.value());
        }
        void operator()(const NodeTermIdentifier* identifier) {
            print_term("Identifier", identifier->identifer->value.value());
        }
    };

    struct StatementVisitor {
        static void print_statement(const std::string statement_name, const NodeTerm* node_term) {
            std::cout << "\e[1;31mNodeStatement" << statement_name << "\e[0;90m( ";
            std::visit(TermVisitor{}, node_term->node);
            std::cout << "\e[90m)\e[0m\n";
        }

        void operator()(const NodeStatementExit* exit) const {
            print_statement("Exit", exit->term);
        }
        void operator()(const NodeStatementLet* let) const { print_statement("Let", let->term); }
        void operator()(const NodeStatementConst* _const) const {
            print_statement("Const", _const->term);
        }
        void operator()(const NodeStatementAssign* assign) const {
            print_statement("Assign", assign->term);
        }
        void operator()(const NodeStatementPrint* print) const {
            print_statement("Print", print->term);
        }
    };
    for (NodeStatement* statement : program.statements) {
        std::visit(StatementVisitor{}, statement->node);
    }
    std::cout << std::endl;
}
