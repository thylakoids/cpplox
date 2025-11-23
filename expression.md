## Expression Grammar

### Basic Grammar Rules
expression     → literal
               | unary
               | binary
               | grouping ;

literal        → NUMBER | STRING | "true" | "false" | "nil" ;
grouping       → "(" expression ")" ;
unary          → ( "-" | "!" ) expression ;
binary         → expression operator expression ;
operator       → "==" | "!=" | "<" | "<=" | ">" | ">="
               | "+"  | "-"  | "*" | "/" ;

### Operator Precedence and Associativity
Name       | Operators   | Associates
-----------|-------------|------------
Equality   | == !=      | Left
Comparison | > >= < <=  | Left
Term       | - +        | Left
Factor     | / *        | Left
Unary      | ! -        | Right
Call       | ()         | Left

### Stratified Grammar (by precedence)
expression     → equality
equality       → comparison ( ( "!=" | "==" ) comparison )*
comparison     → term ( ( ">" | ">=" | "<" | "<=" ) term )*
term           → factor ( ( "-" | "+" ) factor )*
factor         → unary ( ( "/" | "*" ) unary )*
unary          → ( "!" | "-" ) unary | call
call           → primary ( "(" arguments? ")" | "." IDENTIFIER )*
arguments      → expression ( "," expression )*
primary        → NUMBER | STRING | "true" | "false" | "nil"
               | "(" expression ")" | IDENTIFIER | "this"
               | "super" "." IDENTIFIER

### Key Points:
1. Each level only matches expressions at its precedence level or higher
