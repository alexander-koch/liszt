# Liszt

Liszt is a small, lightweight lisp based programming language.
This language is still in "pre-alpha" stage.

## Features

For now, liszt has a limited feature set:

- S-Expressions
- Q-Expressions
- Variable declarations
- Function declarations

## Syntax

Comments start with a semicolon, quotes with a number sign.
An anonymous function is declared using the `lambda` operator.
Variables can either be global or local.
Use `var` for global variables, `local` for locals.
You can import files using the `import` function.
Conditions can be created using the `if` function.

Builtin functions:

- list
- head
- tail
- eval
- local
- var
- lambda
- import
- if
- print

Variable declarations:
```lisp
(var #x 5)
(local #answer 42)
```

Functions:
```lisp
; Declare function main with parameters x and y

(var #main (lambda #(x y) #(* x y)))

; or using std.lisp
(fun #(main x y) #(* x y))
```

Imports:
```lisp
; Will search for std.lisp and math.lisp
(import #std)
(import #math)
```

Conditions:
```lisp
(if (== 0 5)
	#(print true)
	#(print false)
)
```

## License

Copyright (c) Alexander Koch 2016
