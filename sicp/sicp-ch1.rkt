#lang racket
(define (square n)
  (* n n))

(define (smallest-divisor n)
  (find-divisor n 2))

(define (find-divisor n test-divisor)
  (cond ((> (square test-divisor) n) n)
        ((divides? test-divisor n) test-divisor)
        (else (find-divisor n (+ test-divisor 1)))))

(define (divides? a b)
  (= (remainder b a) 0))

(define (prime? n)
  (= n (smallest-divisor n)))

(define (timed-prime-test n)
  (newline)
  (display n)
  (start-prime-test n (current-inexact-milliseconds)))

(define (start-prime-test n start-time)
  (cond ((prime? n)
      (report-prime (- (current-inexact-milliseconds) start-time)))))

(define (report-prime elapsed-time)
  (display " *** ")
  (display elapsed-time))


(define (accumulate combiner null-value term a next b)
  (if (> a b) 
      null-value 
      (accumulate combiner (combiner null-value (term a)) term (next a) next b)
  ))

;(define (combiner x y)
;  (* x y)
;  )

;(define (term x)
;  (/ (* (- x 1) (+ x 1)) (* x x))
;  )

;(define (next x)
;  (+ x 2)
;  )

(define (p-pi n)
  (* 4.0 (accumulate 
          (lambda (x y) (* x y)) 
          1 
          (lambda (x) (/ (* (- x 1) (+ x 1)) (* x x))) 
          (+ (* 2 1) 1) 
          (lambda (x) (+ x 2)) 
          (+ (* 2 n) 1)))
  )

(define tolerance 0.00000000001)

(define (fixed-point f first-guess)
  (define (close-enough a b)
    (< (abs (- a b)) tolerance))
  (define (try guess)
    (let ((next (f guess)))
      (if (close-enough guess next) 
          next
          (try next)
          )))
  (try first-guess))