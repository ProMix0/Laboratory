#lang racket

;;  Функция для вычисления суммы чётных чисел от 2 до n
(define (sum-even n)
  (define (helper i acc)
    (if (> i n)
        acc
        (helper (+ i 2) (+ acc i))))
  (if (< n 2)
      0
      (helper 2 0)))

(displayln (string-append "Сумма чётных чисел: " (number->string (sum-even 17))))

;;  Функция для вставки элемента в список на указанную позицию
(define (insert-at lst elem pos)
  (cond
    [(<= pos 0) (cons elem lst)]
    [(null? lst) (list elem)]
    [else (cons (car lst) (insert-at (cdr lst) elem (- pos 1)))]))

(displayln (string-append "Список после вставки: " (format "~a" (insert-at '(1 2 3 4) 99 2))))

;;  Функция для подсчёта числа элементов списка
(define (my-length lst)
  (define (helper lst acc)
    (if (null? lst)
        acc
        (helper (cdr lst) (+ acc 1))))
  (helper lst 0))

(displayln (string-append "Количество элементов: " (number->string (my-length '(a b c d e)))))

