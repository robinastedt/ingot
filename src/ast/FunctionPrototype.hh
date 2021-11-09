#pragma once

#include <string>

namespace ingot::ast
{
    class FunctionPrototype {
        std::string m_name;
        // TODO: Arguments
    
        friend std::ostream& operator<<(std::ostream& str, const FunctionPrototype& functionPrototype);
    public:
        FunctionPrototype() = default;
        FunctionPrototype(std::string name);

        const std::string& getName() const;

        bool operator<(const FunctionPrototype& rhs) const;
    };

    std::ostream& operator<<(std::ostream& str, const FunctionPrototype& functionPrototype);
} // namespace ingot::ast
