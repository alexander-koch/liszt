; Standard library

; Constants
(def 'true 1)
(def 'false 0)
(def 'pi 3.1415926)

; Function definition formals and body as parameters
(def 'fun (lambda '(f b) '(def (head f) (lambda (tail f) b))))

; First and second element of a list
(fun '(fst ls) '(head ls))
(fun '(snd ls) '(head (tail ls)))
