### TODO

* Variable declaration

### Map

* Lexer
    * Char stream -> token stream
* Parser
    * Token Stream -> AST + Semantic Analysis
* Codegen
    * AST + Semantic Analysis -> CodeBuffer with instructions
* CodeBuffer
    * Allocate and free JIT memory
    * Copy code, manage JIT write protection
    * Return proc function pointer after invalidating instruction cache

### Notes

* Parser registers variables on assignment
* Parser expects variable assignment before usage
* Ast node for assignment stores rhs root node in left
* Codegen is stateless
