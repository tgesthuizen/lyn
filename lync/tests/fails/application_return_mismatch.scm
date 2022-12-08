(declare g (-> int bool))
(define f (lambda () (g (g 0))))
