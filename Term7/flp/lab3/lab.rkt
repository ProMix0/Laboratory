#lang racket

;;  Функция для вычисления суммы чётных чисел от 2 до n
(define (my-sum-even n)
  (cond
    [(< n 2) 0]
    [(= 1 (remainder n 2)) (my-sum-even (- n 1))]
    [else (+ n (my-sum-even (- n 2)))]))

(displayln (string-append "Сумма чётных чисел: " (number->string (my-sum-even 17))))

;;  Функция для вставки элемента в список на указанную позицию
(define (insert-at lst elem pos)
  (cond
    [(<= pos 0) (cons elem lst)]
    [(null? lst) (list elem)]
    [else (cons (car lst) (insert-at (cdr lst) elem (- pos 1)))]))

(displayln (string-append "Список после вставки: " (format "~a" (insert-at '(1 2 3 4) 99 2))))

;;  Функция для подсчёта числа элементов списка
(define (my-length lst elem)
  (cond
    [(null? lst) 0]
    [(= elem (car lst)) (my-length (cdr lst) elem)]
    [else (+ 1 (my-length (cdr lst) elem))]))

(displayln (string-append "Количество элементов: " (number->string (my-length '(1 2 3 4 5) 4))))

