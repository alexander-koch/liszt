; Math standard library

; Constants
(var #pi 3.1415926)
(var #tau (* 2 pi))
(var #e 2.7182818)

; Functions
(func #(sqr x) #(* x x))
(func #(inc x) #(+ x 1))
(func #(dec x) #(- x 1))
(func #(neg x) #(- x))
(func #(abs x)
  #(if (< x 0)
    #(* x (- 1))
	#(x)))

(func #(exp n) #(pow e n))

(func #(fact n)
  #(if (<= n 1)
    #(1)
    #(* n (fact (- n 1)))))

(func #(min x y)
  #(if (< x y)
    #(x)
    #(y)))

(func #(max x y)
  #(if (> x y)
    #(x)
    #(y)))

(func #(toRadian x) #(* x (/ pi 180)))
(func #(toDegree x) #(* x (/ 180 pi)))

(func #(minusp x)
  #(if (< x 0)
    #(true)
	#(false)))

(func #(plusp x)
  #(if (> x 0)
    #(true)
    #(false)))

(func #(sum-digits x)
    #(if (== x 0)
      #(0)
      #(+ (% x 10) (sum-digits (/ (- x (% x 10)) 10)))))

; Vector math
(func #(vec2 x y) #(list x y))
(func #(vec3 x y z) #(list x y z))

(func #(dot x y)
  #(if (== x nil)
    #(0)
    #(+ (* (fst x) (fst y)) (dot (tail x) (tail y)))))
