#include "Integer.hh"

namespace ingot::ast
{
    Integer::Integer(int64_t value)
    : m_value(value) {}

    int64_t
    Integer::getValue() const {
        return m_value;
    }

    Type
    Integer::getType() {
        return {"int"};
    }

    std::ostream& operator<<(std::ostream& str, const Integer& integer) {
        return str << integer.m_value;
    }
} // namespace ingot::ast

