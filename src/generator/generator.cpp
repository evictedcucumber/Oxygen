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

#include "generator.hpp"
#include "../parser/parser.hpp"
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <variant>

void Generator::push(const std::string& reg) {
    m_output << "\tpush " << reg << std::endl;
    m_stack_size++;
}

void Generator::pop(const std::string& reg) {
    m_output << "\tpop " << reg << std::endl;
    m_stack_size--;
}

void Generator::gen_term(const NodeTerm* term) {
    struct TermVisitor {
        Generator& generator;

        void operator()(const NodeTermInteger* integer) const {
            generator.m_output << "\tmov rax, " << integer->integer->value.value() << std::endl;
            generator.push("rax");
        }
        void operator()(const NodeTermIdentifier* identifier) const {
            const auto identity = std::ranges::find_if(
                std::as_const(generator.m_stack), [&](const Variable& variable) {
                    return variable.indentifier == identifier->identifer->value.value();
                });

            if (identity == generator.m_stack.cend()) {
                std::cerr << "[Generator Error] Undeclared identifier `"
                          << identifier->identifer->value.value() << "`" << std::endl;
                exit(EXIT_FAILURE);
            }

            std::stringstream offset;
            offset << "QWORD [rsp + " << (generator.m_stack_size - identity->location - 1) * 8
                   << "]";
            generator.push(offset.str());
        }
    };

    TermVisitor visitor{.generator = *this};
    std::visit(visitor, term->node);
}

void Generator::gen_statement(const NodeStatement* statement) {
    struct StatementVisitor {
        Generator& generator;

        void operator()(const NodeStatementExit* exit) const {
            generator.m_output << "\t;;exit" << std::endl;
            generator.gen_term(exit->term);
            generator.m_output << "\tmov rax, 60" << std::endl;
            generator.pop("rdi");
            generator.m_output << "\tsyscall" << std::endl;
            generator.m_output << "\t;;/exit" << std::endl;
        }
        void operator()(const NodeStatementLet* let) const {
            generator.m_output << "\t;;let " << let->identifer->value.value() << std::endl;
            if (std::ranges::find_if(
                    std::as_const(generator.m_stack), [&](const Variable& variable) {
                        return variable.indentifier == let->identifer->value.value();
                    }) != generator.m_stack.cend()) {
                std::cerr << "[Generator Error] Identifier `" << let->identifer->value.value()
                          << "` already in use" << std::endl;
                exit(EXIT_FAILURE);
            }
            generator.m_stack.push_back({.indentifier = let->identifer->value.value(),
                                         .is_const = false,
                                         .location = generator.m_stack_size});
            generator.gen_term(let->term);
            generator.m_output << "\t;;/let " << let->identifer->value.value() << std::endl;
        }
        void operator()(const NodeStatementConst* _const) const {
            generator.m_output << "\t;;const " << _const->identifer->value.value() << std::endl;
            if (std::ranges::find_if(
                    std::as_const(generator.m_stack), [&](const Variable& variable) {
                        return variable.indentifier == _const->identifer->value.value();
                    }) != generator.m_stack.cend()) {
                std::cerr << "[Generator Error] Identifier `" << _const->identifer->value.value()
                          << "` already in use" << std::endl;
                exit(EXIT_FAILURE);
            }
            generator.m_stack.push_back({.indentifier = _const->identifer->value.value(),
                                         .is_const = true,
                                         .location = generator.m_stack_size});
            generator.gen_term(_const->term);
            generator.m_output << "\t;;/const " << _const->identifer->value.value() << std::endl;
        }
        void operator()(const NodeStatementAssign* assign) const {
            generator.m_output << "\t;;assign " << assign->identifer->value.value() << std::endl;
            const auto identifier = std::ranges::find_if(
                std::as_const(generator.m_stack), [&](const Variable& variable) {
                    return variable.indentifier == assign->identifer->value.value();
                });
            if (identifier == generator.m_stack.cend()) {
                std::cerr << "[Generator Error] Undeclared identifier `"
                          << assign->identifer->value.value() << "`" << std::endl;
                exit(EXIT_FAILURE);
            }
            if (identifier->is_const == true) {
                std::cerr << "[Generator Error] Can not reassign const `" << identifier->indentifier
                          << "`" << std::endl;
                exit(EXIT_FAILURE);
            }
            generator.gen_term(assign->term);
            generator.pop("rax");
            generator.m_output << "\tmov [rsp + "
                               << (generator.m_stack_size - identifier->location - 1) * 8
                               << "], rax" << std::endl;
            generator.m_output << "\t;;/assign " << assign->identifer->value.value() << std::endl;
        }
        void operator()(const NodeStatementPrint* print) const {
            generator.m_output << "\t;;print" << std::endl;
            generator.gen_term(print->term);
            generator.m_output << "\tmov rdi, num_fmt" << std::endl;
            generator.pop("rsi");
            generator.m_output << "\txor eax, eax" << std::endl;
            generator.m_output << "\tcall printf" << std::endl;
            generator.m_output << "\t;;/print" << std::endl;
        }
    };

    StatementVisitor visitor{.generator = *this};
    std::visit(visitor, statement->node);
}

[[nodiscard]] std::string Generator::generate() {
    m_output << "section .data" << std::endl;
    m_output << "\tnum_fmt db \"%d\", 10, 0" << std::endl;
    m_output << std::endl;

    m_output << "section .text" << std::endl;
    m_output << "\textern printf" << std::endl;
    m_output << "\tglobal main" << std::endl;
    m_output << std::endl;

    m_output << "main:" << std::endl;
    for (const NodeStatement* node_statement : m_program.statements) {
        gen_statement(node_statement);
    }
    m_output << std::endl;

    m_output << "\tmov rax, 60 ;; exit syscall" << std::endl;
    m_output << "\txor rdi, rdi" << std::endl;
    m_output << "\tsyscall" << std::endl;
    m_output << std::endl;

    m_output << "section .note.GNU-stack noalloc noexec nowrite progbits" << std::endl;

    return m_output.str();
}
