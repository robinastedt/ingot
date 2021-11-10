#include "Type.hh"

namespace ingot::ast
{
    Type::Type(std::string name)
    : m_name(std::move(name)) {}

    bool
    Type::operator!=(const Type& rhs) const {
        return m_name != rhs.m_name;
    }

    std::ostream& operator<<(std::ostream& str, const Type& type) {
        return str << type.m_name;
    }
} // namespace ingot::ast
