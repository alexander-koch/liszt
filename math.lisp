; Math standard library

; Constants
(var #pi 3.1415926)
(var #tau (* 2 pi))
(var #e 2.7182818)

; Functions
(fun #(sq x) #(* x x))
(fun #(inc x) #(+ x 1))
(fun #(dec x) #(- x 1))
(fun #(neg x) #(- x))
(fun #(abs x)
  #(if (< x 0)
    #(* x (- 1))
	#(x)))

(fun #(pow x n)
  #(if (> n 1)
    #(* x (pow x (- n 1)))
    #(x)))

(fun #(fact n)
  #(if (<= n 1)
    #(1)
    #(* n (fact (- n 1)))))

(fun #(toRadian x) #(* x (/ pi 180)))
(fun #(toDegree x) #(* x (/ 180 pi)))

; Vector math
(fun #(vec2 x y) #(list x y))
(fun #(vec3 x y z) #(list x y z))

(fun #(dot x y)
  #(if (== x nil)
    #(0)
    #(+ (* (fst x) (fst y)) (dot (tail x) (tail y)))))
