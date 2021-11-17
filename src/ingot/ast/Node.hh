#pragma once

#include <ingot/parser/location.hh>

namespace ingot::ast
{
    class Node {
        parser::location m_location;

    public:
        void setLocation(parser::location location);
        const parser::location& getLocation() const;
    };
} // namespace ingot::ast
