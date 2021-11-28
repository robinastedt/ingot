# Function overloading
Do we want this?

# Improved syntax for integer literals
* We want to use a integer literal and specify other size than 64 bit.
* Could be useful for i8 without using characters, if we want 8 bit numbers.
* Same with 16 or 32 bit etc.

# Syntactic sugar for i8 literals
eg 'a', 'b'.

# Syntax for list literals
* Make list an expression type.
* Support list literals, eg [1, 2, 3].
* Replace String expression with List of i8 in AST.

# Improve identifiers, functions first class
* Unify identifier references, arguments and references to global scope functions, as well as passing functions as value
* class ArgumentReference should instead be IdentifierReference
* A FunctionCall should hold a IdentifierReference instead of a string directly
* Make Function an Expression and FunctionType a Type
* When resolving IdentifierReference first look for local scope arguments and then if not shadowed, look at global functions

# Handle memory clean up for lists
Current implementation is garbage and leaks everything.