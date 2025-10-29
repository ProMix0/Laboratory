#lang racket

(define (delete-repetitions lst)
  (define (helper lst elem)
    (cond
      [(null? lst) lst]
      [(null? elem) (cons (car lst) (helper (cdr lst) (car lst)))]
      [(= (car lst) elem) (helper (cdr lst) elem)]
      [else (helper lst null)]))
  (helper lst null))

(displayln (string-append "Повторяющиеся элементы удалены: " (format "~a" (delete-repetitions '(1 1 1 1  2 2 2 2  3 3 3 3  4 4 4 4 4 44 )))))

