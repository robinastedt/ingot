#include "ArgumentReference.hh"

namespace ingot::ast
{
    ArgumentReference::ArgumentReference(std::string name)
    : m_name(std::move(name))
    , m_type(nullptr)
    , m_scope(nullptr)
    , m_index(-1) {}

    const Type&
    ArgumentReference::getType() const {
        return *m_type;
    }

    const std::string&
    ArgumentReference::getName() const {
        return m_name;
    }

    size_t
    ArgumentReference::getIndex() const {
        return m_index;
    }

    void
    ArgumentReference::resolve(const Type& type, const Function& scope, size_t index) {
        m_type = &type;
        m_scope = &scope;
        m_index = index;
    }

    bool
    ArgumentReference::operator==(const ArgumentReference& other) const {
        return m_name == other.m_name;
    }

    std::ostream& operator<<(std::ostream& str, const ArgumentReference& arg) {
        return str << arg.m_name;
    }

} // namespace ingot::ast
