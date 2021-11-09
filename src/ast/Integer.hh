#pragma once

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
    };

    std::ostream& operator<<(std::ostream& str, const Integer& integer);
} // namespace ingot::ast

