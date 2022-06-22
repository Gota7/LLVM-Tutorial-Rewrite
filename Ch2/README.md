# Chapter 2
The parser.

## Changes
1. Separate AST code files in src/ast folder.
2. Make parser object oriented.
3. Parser now stores the parsed AST children for later use.
4. Allow outputting ASTs as debug strings (proto is used in front of function declarations).
5. Main method executes test taking advantage of new AST debug output.