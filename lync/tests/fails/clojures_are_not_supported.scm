(define this-is-fine
  (lambda (a)
    (let ((this-is-not (lambda (b) (+ a b))))
      0)))
