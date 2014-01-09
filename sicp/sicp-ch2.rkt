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

(define (entry tree) (car tree))
(define (left-branch tree) (cadr tree))
(define (right-branch tree) (caddr tree))
(define (make-tree entry left right)
  (list entry left right))

(define (tree->list-1 tree)
  (if (null? tree)
      '()
      (append (tree->list-1 (left-branch tree))
              (cons (entry tree)
                    (tree->list-1 (right-branch tree))))))

(define (tree->list-2 tree)
  (define (copy-to-list tree result-list)
    (if (null? tree)
        result-list
        (copy-to-list (left-branch tree)
                      (cons (entry tree)
                            (copy-to-list (right-branch tree) 
                                          result-list)))))
  (copy-to-list tree '()))

(define tree1 (make-tree 7 (make-tree 3 (make-tree 1 '() '()) 
                                        (make-tree 5 '() '()))
                          (make-tree 9 '()
                                     (make-tree 11 '() '()))))

(define tree2 (make-tree 3 (make-tree 1 '() '())
                         (make-tree 7 (make-tree 5 '() '())
                                    (make-tree 9 '()
                                               (make-tree 11 '() '())))))

(define tree3 (make-tree 5 (make-tree 3 (make-tree 1 '() '()) 
                                        '())
                          (make-tree 9 (make-tree 7 '() '())
                                     (make-tree 11 '() '()))))

(define tree4 (make-tree 7 (make-tree 5 (make-tree 3 (make-tree 1 '()
                                                                '()) 
                                                   '())
                                      '())
                         '()))

(define tree5 (make-tree 1 '()
                         (make-tree 3 '()
                                    (make-tree 5 '() 
                                               (make-tree 7 '()
                                                          '())))))

(define (list->tree elements)
  (car (partial-tree elements (length elements))))

(define (partial-tree elts n)
  (if (= n 0)
      (cons '() elts)
      (let ((left-size (quotient (- n 1) 2)))
        (let ((left-result (partial-tree elts left-size)))
          (let ((left-tree (car left-result))
                (non-left-elts (cdr left-result))
                (right-size (- n (+ left-size 1))))
            (let ((this-entry (car non-left-elts))
                  (right-result (partial-tree (cdr non-left-elts) right-size)))
              (let ((right-tree (car right-result))
                    (remaining-elts (cdr right-result)))
                (cons (make-tree this-entry left-tree right-tree)
                      remaining-elts))))))))

(define tree->list tree->list-2)

(define (union-tree-set set1 set2)
  (list->tree (union-set (tree->list set1) (tree->list set2))))