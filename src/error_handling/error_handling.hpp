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

#include <string>

#ifndef ERROR_HANDLING_HPP
#define ERROR_HANDLING_HPP

class ErrorHandler {
  public:
    ErrorHandler() = default;
    ~ErrorHandler() = default;

    void info(const std::string message, const int line_number = 0);
    void error(const std::string message, const int line_number = 0);
    void error_expected(const std::string message, const int line_number = 0);
};

#endif
