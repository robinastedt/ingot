#include "String.hh"

namespace ingot::ast
{
    String::String(std::string value)
    : Node()
    , m_value(std::move(value)) {}

    Type
    String::getType() {
        return Type::list(Type::integer(8));
    }

    const std::string&
    String::getValue() const {
        return m_value;
    }

    std::ostream& operator<<(std::ostream& str, const String& string) {
        return str << "\"" << string.m_value << "\"";
    }
} // namespace ingot::ast
