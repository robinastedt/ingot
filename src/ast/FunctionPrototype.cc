#include "FunctionPrototype.hh"

namespace ingot::ast
{
    FunctionPrototype::FunctionPrototype(std::string name)
        : m_name(std::move(name)) {}
    

    std::ostream& operator<<(std::ostream& str, const FunctionPrototype& functionPrototype) {
        return str << functionPrototype.m_name;
    }
} // namespace ingot::ast
