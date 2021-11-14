#pragma once

#include <ast/Type.hh>

#include <ostream>
#include <string>

namespace ingot::ast
{
    class String {
        std::string m_value;

        friend std::ostream& operator<<(std::ostream& str, const String& string);
    public:
        String(std::string value);

        static Type getType();
        const std::string& getValue() const;
    };

    std::ostream& operator<<(std::ostream& str, const String& string);
} // namespace ingot::ast
