#pragma once

#include <iostream>

class Exception : public std::runtime_error
{
public:
    Exception(const std::string& errorString) : std::runtime_error(errorString)
    {}
};
