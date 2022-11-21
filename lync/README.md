# lync

lync is the compile for the lyn language.
Right now it performs the following passes:
1. Parse input
2. scopify: Assigns ids to every variable and resolves lexical
   scoping. A more common name for this pass is alpha conversion.
3. typecheck: Typechecks the program using a Hindley-Milner style type
   system.
4. genanf: Converts the typechecked AST into an intermediate
   representation resembling A-normal form.
5. genasm: Converts the intermediate representation to textual
   assembly, suitable to be passed to an assembler to yield executable
   code.

Note that the code is of horrible quality at the moment due to the
lack of register allocation, primitive functions and any kind of
optimization.
