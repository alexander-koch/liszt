; Math standard library

; Constants
(var #pi 3.1415926)
(var #tau (* 2 pi))
(var #e 2.7182818)

; Functions
(fun #(sqr x) #(* x x))
(fun #(inc x) #(+ x 1))
(fun #(dec x) #(- x 1))
(fun #(neg x) #(- x))
(fun #(abs x)
  #(if (< x 0)
    #(* x (- 1))
	#(x)))

(fun #(exp n) #(pow e n))

(fun #(fact n)
  #(if (<= n 1)
    #(1)
    #(* n (fact (- n 1)))))

(fun #(min x y)
  #(if (< x y)
    #(x)
    #(y)))

(fun #(max x y)
  #(if (> x y)
    #(x)
    #(y)))

(fun #(toRadian x) #(* x (/ pi 180)))
(fun #(toDegree x) #(* x (/ 180 pi)))

(fun #(minusp x)
  #(if (< x 0)
    #(true)
	#(false)))

(fun #(plusp x)
  #(if (> x 0)
    #(true)
    #(false)))

(fun #(sum-digits x)
    #(if (== x 0)
      #(0)
      #(+ (% x 10) (sum-digits (/ (- x (% x 10)) 10)))))

; Vector math
(fun #(vec2 x y) #(list x y))
(fun #(vec3 x y z) #(list x y z))

(fun #(dot x y)
  #(if (== x nil)
    #(0)
    #(+ (* (fst x) (fst y)) (dot (tail x) (tail y)))))
