; Standard library

; Constants
(var #nil #())
(var #true 1)
(var #false 0)

; Function definition, formals and body as parameters
(var #func
  (lambda #(f b)
    #(var (head f)
      (lambda (tail f) b))))

; When operator
(func #(when x y)
  #(if (== x true)
    #(y)
    #(nil)))

; First and second element of a list
(func #(fst ls) #(eval (head ls)))
(func #(snd ls) #(eval (head (tail ls))))

; List length
(func #(len l)
  #(if (== l nil)
    #(0)
    #(+ 1 (len (tail l)))))

; Return the last element
(func #(last l)
  #(if (== (len l) 1)
    #(eval l)
    #(last (tail l))))

; List contains element
(func #(contains l e)
  #(if (== l nil)
    #(false)
    #(if (== (fst l) e)
      #(true)
      #(contains (tail l) e))))

; List index
(func #(index l i)
  #(if (== i 0)
    #(fst l)
    #(index (tail l) (- i 1))))

; Define a block
(func #(block & l)
  #(if (== l nil)
    #(nil)
    #(last l)))

; Filter by function
(func #(filter f l)
  #(if (== l nil)
    #(nil)
    #(join
      (if (f (fst l))
        #(head l)
        #(nil))
      (filter f (tail l)))))
