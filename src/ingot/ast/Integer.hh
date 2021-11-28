#pragma once

#include <ingot/ast/Type.hh>

#include <cstdint>
#include <iostream>

namespace ingot::ast
{
    class Integer : public Node {
        int64_t m_value;  // TODO: support arbitrary size
        Type m_type;

        friend std::ostream& operator<<(std::ostream&, const Integer&);
    
    public:
        Integer(int64_t value); // TODO: Handle more sizes
        int64_t getValue() const;
        const Type& getType() const;
        void setSize(size_t size);
    };

    std::ostream& operator<<(std::ostream& str, const Integer& integer);
} // namespace ingot::ast

