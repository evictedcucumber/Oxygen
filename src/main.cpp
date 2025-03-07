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

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>

#include "error_handling/error_handling.hpp"
#include "generator/generator.hpp"
#include "logging/logging.hpp"
#include "tokenizer/tokenizer.hpp"

int main(int argc, char* argv[]) {
    ErrorHandler error_handler{};

    if (argc != 3) {
        error_handler.error("Incorrect usage of command oxygen [options] <input.o2> <output>");
        exit(EXIT_FAILURE);
    }

    if (std::system("where nasm > /dev/null 2>&1") == 0 ||
        std::system("where gcc > /dev/null 2>&1") == 0) {
        error_handler.error("`nasm` and `gcc` should be installed on system");
        exit(EXIT_FAILURE);
    }

    std::string asm_outfile = std::string(argv[2]) + ".asm";
    std::string o_outfile = std::string(argv[2]) + ".o";
    std::string outfile = std::string(argv[2]);

    std::string source_contents;
    {
        std::stringstream contents_stream;
        std::fstream input(argv[1], std::ios::in);
        contents_stream << input.rdbuf();
        source_contents = contents_stream.str();
    }

    if (source_contents.empty()) {
        error_handler.info(std::move("Empty source file, nothing todo"));
        exit(EXIT_SUCCESS);
    }

    Tokenizer tokenizer(std::move(source_contents));

    std::vector<Token*> tokens = tokenizer.tokenize();

    log_tokens(tokens);

    Parser parser(std::move(tokens));

    Program program = parser.parse();

    log_program(program);

    {
        Generator generator(std::move(program));
        std::fstream output(std::string(asm_outfile), std::ios::out);
        output << generator.generate();
    }

    std::stringstream nasm_command;
    nasm_command << "nasm -felf64 " << asm_outfile << " -o " << o_outfile;
    std::stringstream gcc_command;
    gcc_command << "gcc " << o_outfile << " -o " << outfile << " -no-pie";

    system(nasm_command.str().c_str());
    system(gcc_command.str().c_str());

    return EXIT_SUCCESS;
}
