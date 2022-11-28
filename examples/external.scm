(declare print-int (-> int unit))
(define print-sum
  (lambda (a b)
    (print-int (+ a b))))
