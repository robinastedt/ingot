#include "Integer.hh"

namespace ingot::ast
{
    Integer::Integer(int64_t value)
    : m_value(value) {}

    std::ostream& operator<<(std::ostream& str, const Integer& integer) {
        return str << integer.m_value;
    }
} // namespace ingot::ast

