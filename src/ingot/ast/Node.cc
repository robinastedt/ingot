#include "Node.hh"

namespace ingot::ast
{
    void
    Node::setLocation(parser::location location) {
        m_location = location;
    }

    const parser::location&
    Node::getLocation() const {
        return m_location;
    }
} // namespace ingot::ast
