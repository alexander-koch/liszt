; Standard library

; Constants
(var #nil #())
(var #true 1)
(var #false 0)

; Function varinition formals and body as parameters
(var #fun (lambda #(f b)
	#(var (head f) (lambda (tail f) b))
))

; First and second element of a list
(fun #(fst ls) #(eval (head ls)))
(fun #(snd ls) #(eval (head (tail ls))))

; List operations
(fun #(len l)
	#(if (== l nil)
		#(0)
		#(+ 1 (len (tail l)))
	)
)

(fun #(contains l e)
	#(if (== l nil)
		#(false)
		#(if (== (fst l) e)
			#(true)
			#(contains (tail l) e)
		)
	)
)
