#include "String.hh"

namespace ingot::ast
{
    String::String(std::string value)
    : m_value(std::move(value)) {}

    std::ostream& operator<<(std::ostream& str, const String& string) {
        return str << "\"" << string.m_value << "\"";
    }
} // namespace ingot::ast
