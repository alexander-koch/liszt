; Math standard library

; Constants
(var #pi 3.1415926)
(var #e 2.7182818)

; Functions
(fun #(square x) #(* x x))
(fun #(inc x) #(+ x 1))
(fun #(dec x) #(- x 1))
(fun #(neg x) #(- 0 x))
(fun #(abs x)
	#(if (< x 0)
		#(* x (neg 1))
		#(x)
	)
)

(fun #(deg2rad x) #(* x (/ pi 180)))
(fun #(rad2deg x) #(* x (/ 180 pi)))
