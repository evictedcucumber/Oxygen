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

#include "error_handling.hpp"
#include <iostream>
#include <sstream>

std::string WHITE = "\e[0m";
std::string BOLD_RED = "\e[1;31m";
std::string BOLD_GREEN = "\e[1;32m";
std::string BOLD_YELLOW = "\e[1;33m";
std::string BOLD_BLUE = "\e[1;34m";

const std::string has_line_number(const int line_number) {
    std::stringstream ss;
    if (line_number > 0) {
        ss << BOLD_YELLOW << ":" << line_number << ":";
    }
    ss << " ";
    return ss.str();
}

void ErrorHandler::info(const std::string message, const int line_number) {
    std::cout << BOLD_BLUE << "[Info]" << has_line_number(line_number) << WHITE << message
              << std::endl;
}

void ErrorHandler::error(const std::string message, const int line_number) {
    std::cout << BOLD_RED << "[Error]" << has_line_number(line_number) << WHITE << message
              << std::endl;
}

void ErrorHandler::error_expected(const std::string message, const int line_number) {
    std::cerr << BOLD_RED << "[Error]" << has_line_number(line_number) << WHITE << "Expected "
              << BOLD_GREEN << message << std::endl;
}
