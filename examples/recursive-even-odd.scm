(define run
  (lambda (n)
    (let rec ((odd? (lambda (x) (if (= x 0) false (even? (- x 1)))))
	      (even? (lambda (x) (if (= x 0) true (odd? (- x 1))))))
      (even? n))))
