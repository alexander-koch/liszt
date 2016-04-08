; Math standard library

; Constants
(var #pi 3.1415926)
(var #e 2.7182818)

; Functions
(fun #(square x) #(* x x))
(fun #(inc x) #(+ x 1))
(fun #(dec x) #(- x 1))

(fun #(deg2rad x) #(* x (/ pi 180)))
(fun #(rad2deg x) #(* x (/ 180 pi)))
