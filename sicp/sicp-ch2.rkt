#lang racket

(define (last-pair l)
  (if (null? (cdr l))
      (car l)
      (last-pair (cdr l))))

(define (reverse l)
  (define (rev l rl)
    (if (null? l)
        rl
        (rev (cdr l) (cons (car l) rl))
        ))
  (rev l '()))

(define (deep-reverse tree)
  (dp-rev tree '()))

(define (dp-rev tree rtree)
  (cond ((null? tree) rtree)
        ((pair? (car tree)) (dp-rev (cdr tree) (cons (deep-reverse (car tree)) rtree)))
        (else (dp-rev (cdr tree) (cons (car tree) rtree)))))

(define x (list 1 2 3))
(define y (list 4 5 6))

(define zero (lambda (f) (lambda (x) x)))

(define (add-1 n)
  (lambda (f) (lambda (x) (f ((n f) x)))))

(define one 
  (lambda (f) (lambda (x) (f x))))

(define two
  (lambda (f) (lambda (x) (f (f x)))))

;(define (+ n m)
;  (lambda (f) (lambda (x) (n ((m f) x)))))

(define (map proc items)
  (if (null? items)
      '()
      (cons (proc (car items)) 
            (map proc (cdr items)))))

(define (filter predicate sequence)
  (cond ((null? sequence) '())
        ((predicate (car sequence)) 
         (cons (car sequence) (filter predicate (cdr sequence))))
        (else (filter predicate (cdr sequence)))))

;incorrect accumulate (accumulate cons '() '(1 2 4 6 7 98))
;(define (accumulate op initial sequence)
;  (if (null? sequence)
;      initial
;     (accumulate op (op initial (car sequence)) (cdr sequence))))

(define (accumulate op initial sequence)
  (if (null? sequence)
      initial
      (op (car sequence) (accumulate op initial (cdr sequence)))))

(define (horner-eval x coefficient-sequence)
  (accumulate (lambda (this-coeff higher-terms) (+ this-coeff (* higher-terms x)))
              0
              coefficient-sequence))
;initial for transform, not only linear op like plus

(define (enumerate-interval low high)
  (if (> low high)
      '()
      (cons low (enumerate-interval (+ low 1) high))))

(define (flatmap proc seq)
  (accumulate append '() (map proc seq)))

(define (unique-pairs n)
  (flatmap (lambda (i)
             (map (lambda (j) (list i j)) (enumerate-interval 1 (- i 1))))
           (enumerate-interval 1 n)))

(define (augend e)
  (if (null? (cdr (cddr e)))
      (caddr e)
      (cons '+ (cddr e))
      ))

(define (element-of-set? x set)
  (cond ((null? set) false)
        ((equal? (car set) x) true)
        (else (element-of-set? x (cdr set)))))

(define (adjoin-set x set)
  (if (element-of-set? x set) 
      set
      (cons x set)))

(define (intersection-set set1 set2)
  (cond ((or (null? set2) (null? set1)) '())
        ((element-of-set? (car set1) set2) 
         (cons (car set1) 
               (intersection-set (cdr set1) set2)))
        (else (intersection-set (cdr set1) set2))))

(define (union-set set1 set2)
  (cond ((null? set1) set2)
        ((element-of-set? (car set1) set2) (union-set (cdr set1) set2))
        (else (cons (car set1) (union-set (cdr set1) set2)))))

;adjoin-ordered-set
(define (adjoin-ordered-set x set)
  (cond ((null? set) (list x))
        ((= x (car set)) set)
        ((< x (car set)) (cons x set))
        (else (cons (car set) 
                    (adjoin-ordered-set x (cdr set))))))