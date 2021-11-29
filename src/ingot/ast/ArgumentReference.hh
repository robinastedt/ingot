#pragma once

#include <ingot/ast/Node.hh>

#include <string>

namespace ingot::ast
{
    class Type;
    class Function;

    class ArgumentReference : public Node {
        std::string m_name;

        // Updated by semantic resolver
        const Type* m_type;
        const Function* m_scope;
        size_t m_index;

        friend std::ostream& operator<<(std::ostream& str, const ArgumentReference& arg);
    public:
        ArgumentReference(std::string name);
        const std::string& getName() const;

        const Type& getType() const;
        size_t getIndex() const;

        void resolve(const Type& type, const Function& scope, size_t index);

        bool operator==(const ArgumentReference& other) const;
    };

    std::ostream& operator<<(std::ostream& str, const ArgumentReference& arg);
} // namespace ingot::ast
