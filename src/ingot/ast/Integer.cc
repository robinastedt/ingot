#include "Integer.hh"

namespace ingot::ast
{
    Integer::Integer(int64_t value)
    : Node()
    , m_value(value)
    , m_type(Type::integer(0)) {}

    int64_t
    Integer::getValue() const {
        return m_value;
    }

    const Type&
    Integer::getType() const {
        return m_type;
    }

    void
    Integer::setSize(size_t size) {
        m_type.setSize(size);
    }

    std::ostream& operator<<(std::ostream& str, const Integer& integer) {
        return str << integer.m_value;
    }
} // namespace ingot::ast

