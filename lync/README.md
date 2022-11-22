# lync

lync is the compiler for the lyn language.
Right now it performs the following passes:
1. Parse input
2. alpha_convert: Applies alpha conversion to the parse tree to not let
   the following passes worry about lexical scoping.
3. typecheck: Typechecks the program using a Hindley-Milner style type
   system.
4. genanf: Converts the typechecked AST into an intermediate
   representation resembling A-normal form.
5. genasm: Converts the intermediate representation to textual
   assembly, suitable to be passed to an assembler to yield executable
   code.

Note that the generated code is of horrible quality at the moment due
to the lack of register allocation, primitive functions and any kind
of optimization.
