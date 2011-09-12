#lang racket

(require profile)

(define (go)
  'yep-it-works)

(define digit-map '(0 1)) ;;show howto rearrange bytes when reading

(define (my-read-byte file)
  (let ([d (read-byte file)])
    (if (eof-object? d)
        (raise d #t)
        d
        )))

(define (unsigned->signed unum)
  (if (= 1 (bitwise-and unum 128))
      (- (bitwise-and unum 127))
      (bitwise-and unum 127)))

(define (read-num file)
  (let* ([digits-list (sort 
                           (map (lambda (pos) (cons pos (my-read-byte file))) digit-map)
                           <                        
                     #:key car
                     )]
         [unum     (foldl (lambda (digit num)  (+ (* num 256) (cdr digit))) (bitwise-and (cdr (first digits-list)) 127) (rest digits-list))])
    (print unum)
         (* (- (cdr (first digits-list))) unum)))


(define (append-to-end lst elem)
  (append lst (list elem))
  )

(define (read-to-array file)
  (let ([data '()]
        [counter 0]
        )
    (with-handlers ([eof-object? (lambda(x) data)])
      (let loop ()
        (set! data (append-to-end data (read-num file)))
        (set! counter (+ counter 1))
        (when (< counter 1) (loop)))
      data)))

(define input-file-name "/home/lexa/develop/rnlms/Debug/g165/echo_10_128_small.dat")

(define (main)
  (let* ([in-file  (open-input-file input-file-name #:mode 'binary)]
         [data (read-to-array in-file)])
    (print data)
    (close-input-port in-file)
    ))
    
    ;;[out-file (open-output-file output-file-name  #:exists 'replace #mode 'text)])
    
    
(main)