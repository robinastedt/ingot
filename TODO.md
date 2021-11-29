# Function overloading
Do we want this?

# Improve identifiers, functions first class
* Unify identifier references, arguments and references to global scope functions, as well as passing functions as value
* class ArgumentReference should instead be IdentifierReference
* A FunctionCall should hold a IdentifierReference instead of a string directly
* Make Function an Expression and FunctionType a Type
* When resolving IdentifierReference first look for local scope arguments and then if not shadowed, look at global functions

# Handle memory clean up for lists
Current implementation is garbage and leaks everything.

# Control flow
We are currently not turing complete! (I think)
* if/else function/keyword
* map, reduce, fold, etc on lists. Also needs isEmpty, head, tail, etc.