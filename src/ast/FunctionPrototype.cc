#include "FunctionPrototype.hh"

namespace ingot::ast
{
    FunctionPrototype::FunctionPrototype(std::string name)
    : m_name(std::move(name)) {}
    

    const std::string&
    FunctionPrototype::getName() const {
        return m_name;
    }

    bool
    FunctionPrototype::operator<(const FunctionPrototype& rhs) const {
        return m_name < rhs.m_name;
    }

    std::ostream& operator<<(std::ostream& str, const FunctionPrototype& functionPrototype) {
        return str << functionPrototype.m_name;
    }
} // namespace ingot::ast
