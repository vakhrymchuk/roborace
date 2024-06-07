#pragma once

#include <string.h>

class Param
{
public:
    explicit Param(
        int value,
        String name = "",
        String group = "default") : value(value),
                                    name(std::move(name)),
                                    group(std::move(group)) {}

    int value;
    String name;
    String group;
};
