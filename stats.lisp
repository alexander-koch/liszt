; Stochastics library

; Binomial coefficient
; Formula: n! / (k! * (n - k)!
(fun #(ncr np k)
  #(/ (fact np) (* (fact k) (fact (- np k)))))

; Binomial probability distribution
; Formula: bin(n,p,k) = (n k) * p^k * (1-p)^(n-k)
(fun #(binompdf n p k)
  #(* (ncr n k) (* (pow p k) (pow (- 1 p) (- n k)))))

; Normal distribution function
(fun #(normpdf x m s)
  #(* (/ 1 (* s (sqrt tau))
    (exp (neg (/
        (pow (- x m) 2)
	    (* 2 (pow s 2))))))))
		
(var #a1 0.254829592)
(var #a2 (- 0.284496736))
(var #a3 1.421413741)
(var #a4 (- 1.453152027))
(var #a5 1.061405429)
(var #p1 0.3275911)
(var #sqr2 (sqrt 2.0))

; ERF, error function
(fun #(erf x)
  #(block
    (var #y (/ 1.0 (+ 1.0 (* p1 (abs x)))))
    (var #z (- 1.0 (* y (exp (- (* x x)))
      (+ a1 (* y (+ a2 (* y (+ a3 (* y (+ a4 (* y a5)))))))))))
    (if (plusp x)
      #(z)
      #(- z))))

; Cumulative density function for a standard gaussian bell curve
(fun #(phi x)
  #(* 0.5 (+ 1.0 (erf (/ x sqr2)))))

; Cumulative normal distribution function
(fun #(normcdf x m s)
  #(* (/ 1 2) (+ 1 (erf (/ (- x m) (sqrt (* 2 (sqr s))))))))
