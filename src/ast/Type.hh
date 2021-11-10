#pragma once

#include <string>

namespace ingot::ast
{
    class Type {
        std::string m_name;

        friend std::ostream& operator<<(std::ostream& str, const Type& type);

    public:
        Type() = default;
        Type(std::string name);
        bool operator==(const Type& rhs) const;
        bool operator!=(const Type& rhs) const;
        const std::string& getName() const;
    };

    std::ostream& operator<<(std::ostream& str, const Type& type);
} // namespace ingot::ast
