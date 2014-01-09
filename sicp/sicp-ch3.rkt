#lang racket

(define ones (stream-cons 1 ones))

(define (print-n seq n)
    (cond ((> n 0) (begin (display (stream-first seq)) (display "\n") (print-n (stream-rest seq) (- n 1))))))

(define (add-streams s1 s2)
  (stream-cons (+ (stream-first s1) (stream-first s2)) (add-streams (stream-rest s1) (stream-rest s2))))

(define integers (stream-cons 1 (add-streams ones integers)))

(define (partial-sum seq)
  (define pss 
    (stream-cons (stream-first seq) (add-streams (stream-rest seq) pss)))
  pss)

(define (scale-stream seq factor)
  (stream-map (lambda (x) (* x factor)) seq))

(define (average x y)
  (/ (+ x y) 2))

(define (sqrt-improve guess x)
  (average guess (/ x guess)))

(define (sqrt-stream x)
  (define guesses
    (stream-cons 1.0
                 (stream-map (lambda (guess)
                               (sqrt-improve guess x))
                             guesses)))
  guesses)

(define (pi-summands n)
  (stream-cons (/ 1.0 n)
               (stream-map - (pi-summands (+ n 2)))))

(define pi-stream
  (scale-stream (partial-sum (pi-summands 1.0)) 4))

(define (ln2-summands n)
  (stream-cons (/ 1.0 n)
               (stream-map - (ln2-summands (+ n 1)))))

(define ln2-stream
  (partial-sum (ln2-summands 1.0)))

;sequence accelerator
(define (euler-transform s)
  (let ((s0 (stream-ref s 0))
        (s1 (stream-ref s 1))
        (s2 (stream-ref s 2)))
    (stream-cons (- s2 
                    (/ (* (- s2 s1) (- s2 s1)) 
                       (+ (+ s0 (* -2 s1)) s2))) 
                 (euler-transform (stream-rest s)))))

(define (make-tableau transform s)
  (stream-cons s
               (make-tableau transform
                             (transform s))))

(define (accelerated-sequence transform s)
  (stream-map stream-first
              (make-tableau transform s)))

(define (merge s1 s2)
  (cond ((stream-empty? s1) s2)
        ((stream-empty? s2) s1)
        (else
         (let ((s1car (stream-first s1))
               (s2car (stream-first s2)))
           (cond ((< s1car s2car)
                  (stream-cons s1car (merge (stream-rest s1) s2)))
                 ((< s2car s1car)
                  (stream-cons s2car (merge s1 (stream-rest s2))))
                 (else
                  (stream-cons s1car (merge (stream-rest s1) (stream-rest s2)))))))))

(define Ss (stream-cons 1 (merge (scale-stream Ss 5) (merge (scale-stream Ss 2) (scale-stream Ss 3)))))

(define (sumexp3 x y)
  (+ (* x (* x x)) (* y (* y y))))
 
(define (merge-weighted s1 s2 weight)
  (cond ((stream-empty? s1) s2)
        ((stream-empty? s2) s1)
        (else
         (let ((s1car (weight (stream-first (stream-first s1)) (stream-first (stream-rest (stream-first s1)))))
               (s2car (weight (stream-first (stream-first s2)) (stream-first (stream-rest (stream-first s2))))))
           (cond ((< s1car s2car)
                  (stream-cons (stream-first s1) (merge-weighted (stream-rest s1) s2 weight)))
                 ((< s2car s1car)
                  (stream-cons (stream-first s2) (merge-weighted s1 (stream-rest s2) weight)))
                 (else
                  (stream-cons (stream-first s1) (stream-cons (stream-first s2) (merge-weighted (stream-rest s1) (stream-rest s2) weight)))))))))

(define (weighted-pairs s t weight)
  (stream-cons
   (list (stream-first s) (stream-first t))
   (merge-weighted 
    (stream-map (lambda (x) (list (stream-first s) x)) (stream-rest t))
    (weighted-pairs (stream-rest s) (stream-rest t) weight) weight)))

(define (uniq-stream s weight)
  (define (uniq-stream-with-last s weight last)
    (cond ((= (weight last) (weight (stream-first s))) (uniq-stream-with-last (stream-rest s) weight last))
          (else (stream-cons (stream-first s) (uniq-stream-with-last (stream-rest s) weight (stream-first s))))))
  (stream-cons (stream-first s) (uniq-stream-with-last (stream-rest s) weight (stream-first s))))

(define (pair-sumexp3 p)
  (sumexp3 (stream-first p) (stream-first (stream-rest p))))

(define (get-dup s weight)
  (define (get-dup-with-state-machine s weight state last)
    (cond ((and (= state 1) (= (weight (stream-first s)) (weight last))) 
           (stream-cons (cons (weight last) last) (get-dup-with-state-machine (stream-rest s) weight 2 (stream-first s))))
          ((and (= state 1) (not (= (weight (stream-first s)) (weight last))))
           (get-dup-with-state-machine (stream-rest s) weight 1 (stream-first s)))
          ((and (= state 2) (= (weight (stream-first s)) (weight last)))
           (stream-cons (cons (weight last) last) (get-dup-with-state-machine (stream-rest s) weight 2 (stream-first s))))
          ((and (= state 2) (not (= (weight (stream-first s)) (weight last))))
           (stream-cons (cons (weight last) last) (get-dup-with-state-machine (stream-rest s) weight 1 (stream-first s))))))
  (get-dup-with-state-machine (stream-rest s) weight 1 (stream-first s)))