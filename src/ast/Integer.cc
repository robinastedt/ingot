#include "Integer.hh"

namespace ingot::ast
{
    Integer::Integer(int64_t value)
    : m_value(value) {}

    int64_t
    Integer::getValue() const {
        return m_value;
    }

    std::ostream& operator<<(std::ostream& str, const Integer& integer) {
        return str << integer.m_value;
    }
} // namespace ingot::ast

