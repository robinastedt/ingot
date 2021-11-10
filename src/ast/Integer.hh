#pragma once

#include <ast/Type.hh>

#include <cstdint>
#include <iostream>

namespace ingot::ast
{
    class Integer {
        int64_t m_value;

        friend std::ostream& operator<<(std::ostream&, const Integer&);
    
    public:
        Integer(int64_t value);
        int64_t getValue() const;
        static Type getType();
    };

    std::ostream& operator<<(std::ostream& str, const Integer& integer);
} // namespace ingot::ast

