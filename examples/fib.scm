(define fib-helper
  (lambda (a b n)
    (if (= n 0)
	a
	(fib-helper
	 b
	 (+ a b)
	 (- n 1)))))

(define fib
  (lambda (n)
    (fib-helper 1 1 n)))
