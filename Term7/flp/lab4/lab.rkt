#lang racket

;;  Функция, инкрементирующая каждый чётный элемент списка
(define (inc-even lst)
  (map (lambda (x)
    (if (even? x)
      (+ x 1)
      x))
    lst))

(displayln (string-append "Инкрементированные чётные: " (format "~a" (inc-even '(1 -2 3 4 5 6)))))

;;  Функция, удаляющая все элементы, совпадающие с заданным образцом (pattern)
(define (remove-pattern lst pattern)
  (filter (lambda (x) (not (= x pattern))) lst))

(displayln (string-append "Без элементов = 2: " (format "~a" (remove-pattern '(1 2 3 2 4 2 5) 2))))

