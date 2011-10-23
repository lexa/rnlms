#lang racket
(require ffi/unsafe)
(require ffi/cvector)


;;;;;;;;;;;;;;;;;;;;;;;;;;;;utilites;;;;;;;;;;;;;;;;;;;;;;;;;;;;


(define (lg  num)
  (/ (log num) (log 10)))

;; (define (signal-level signal)
;;   (* 10 (lg (add1 (/ (foldl + 0 (map sqr signal)) (length  signal))))))

;; (define-syntax-rule (add! x y)
;;   (set! x (+ x y)))
  
(define (signal-level signal . optional-args)
  (let ([start (if (null? optional-args) 0 (car optional-args))]
        [len (if (null? optional-args) (vector-length signal) (cadr optional-args))])
        
    (- (* 10 (lg (add1
                  (/ (let loop ([i (sub1 len)] [sum 0])
                       (if (< i 0)
                           sum
                           (loop (sub1 i) (+ sum (sqr (vector-ref signal (+ start i)))))))
                     len))))
       90.30899870323904 ;; 90.30899870323904 == (signal-level #(32768))
       )))

;; (define (signal-level signal . optional-args)

;;   (- (* 10 (lg (add1
;;                 (/ (let loop ([i (sub1 (vector-length signal))] [sum 0])
;;                      (if (< i 0)
;;                          sum
;;                          (loop (sub1 i) (+ sum (sqr (vector-ref signal i))))))
;;                    (vector-length signal)))))
;;      90.30899870323904 ;; 90.30899870323904 == (signal-level #(32768))
;;      ))

(define (bytes->int bytes)
  (if (= (bytes-length bytes) 2)
      (let ([buf (malloc _int16 'raw)])
        (ptr-set! buf _uint8 (bytes-ref bytes 0))
        (ptr-set! buf _uint8 1 (bytes-ref bytes 1))
        
        (let ([ret-val (ptr-ref buf _int16)])
          (free buf)
          ret-val)
        )
      (error 'bytes->float "invalid len of bytes")
      ))
  
                 
(define (bytes->float bytes)
  (if (= (bytes-length bytes) 2)
      (let ([buf (malloc _int16 'raw)])
        (ptr-set! buf _uint8 (bytes-ref bytes 0))
        (ptr-set! buf _uint8 1 (bytes-ref bytes 1))
        
        (let ([ret-val (real->double-flonum (ptr-ref buf _int16))])
          (free buf)
          ret-val)
        )
      (error 'bytes->float "invalid len of bytes")
      ))

(define (signal->levels signal step window)
  (let ([ret (make-vector (- (quotient (- (vector-length signal) window) step) -1))])
    (let loop ([start 0] [i 0])      
      (when (<= (+ start window) (vector-length signal))
        (begin
          (vector-set! ret i (signal-level signal start window))
          (loop (+ start step) (add1 i)))
        ))
    ret
    ))
  
;;;;;;;;;;;;;;;;;;;;;;;;;;;;white noise generator ;;;;;;;;;;;;;;;;;;

(define (generate-white-noise level)
  (let* ([filename (string-append "g165/filtered_noise_" (number->string (- level)) ".dat")]
         [file (open-input-file filename #:mode 'binary)]
         [data (make-vector (/ (file-size filename) 2))])
    (let loop ([i 0])
      (let ([num (read-bytes 2 file)])
        (when (not (eof-object? num))
          (begin
            (bytes->int num)
            (vector-set! data i (bytes->int num))
            (loop (add1 i))))))
    
    (close-input-port file)
    data
    ))

(define (delay-signal signal delay-time)
  (let ([ret (make-vector (+ (vector-length signal) delay-time) 0)])
    (vector-copy! ret delay-time signal)
    ret))

(define (attenuate signal level)
  (let ([coeff (expt 10 (/ level 20))])
    (vector-map (lambda (x) (inexact->exact (round (* x coeff)))) signal)))
  
  
;;;;;;;;;;;;;;;;;;;;;;;;;;;;test-generic;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;; (define test-interface (interface () get-test-data check-test-results))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;test-g165;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;; (define test-g165-1% (class* object%
;;                       (test-interface)
;;                       (super-new)
;;                       (init input-level delay-time echo-loss)
                      
;;                       (define R-in (generate-white-noise input-level))
;;                       (define S-in (delay-signal (attenuate R-in echo-loss) delay-time))

;;                       ;;возвращает R_in и S_in
;;                       (define/public (get-test-data)
;;                         (values R-in S-in))
;;                       (define/public (check-test-results R-out S-out)
;;                         ;;TODO
;;                         #t
;;                         )))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;c-rnlms ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define _size_t (make-ctype _ulong #f #f))
(define _num _float)
;(define _audio_in (_cvector i _int16 ))
;(define _audio_out (_cvector i _int16 ))

(define lib (ffi-lib "/home/lexa/develop/rnlms/librnlms.so"))
  
(define c-rnlms-init-struct (get-ffi-obj "rnlms_init_struct" lib (_fun (data betta delta memory-factor filter-len) :: (data : _pointer) (betta : _num)(delta : _num) (memory-factor : _num) (filter-len : _size_t) -> _int)))

(define c-rnlms-process(get-ffi-obj "rnlms_process" lib (_fun (hnd x_arr y_arr err_out size) :: (hnd : _pointer) (x_arr : _pointer) (y_arr : _pointer) (err_out : _pointer) (size : _size_t) -> _int)))

(define c-sizeof-rnlms (get-ffi-obj "sizeof_rnlms" lib (_fun (filter-len) :: (filter-len : _size_t) -> _size_t)))

(define _rnlms_options (_bitmask '(OPT_INHIBIT_ADAPTATION = 1 OPT_DISABLE_NONLINEAR_PROCESSING = 2)))

(define c-rnlms-set-options (get-ffi-obj "rnlms_set_options" lib (_fun (mem options) :: (mem : _pointer) (options : _rnlms_options) -> _int)))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;testing-algo;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define (vector->mem vector type)
  (let ([mem (malloc type (vector-length vector))])
    (let loop ([i 0])
      (when (< i (vector-length vector))
        (begin
          (ptr-set! mem type i  (vector-ref vector i) )
          (loop (add1 i)))))
    mem
    ))

(define (mem->vector mem type len)
  (let ([ret (make-vector  len)])
    (let loop ([i 0])
      (if (< i len)
          (begin
            (vector-set! ret i (ptr-ref mem type i))
            (loop (add1 i)))
          ret
  ))))
 
(define (print-mem mem-array type len filename)
  (with-output-to-file
      filename
      #:mode 'text
      #:exists 'replace
      (lambda ()
        (let loop ([i 0])
          (when (< i len)
            (begin (printf "~a~%" (ptr-ref mem-array type i)) (loop (add1 i))))))))


(define (find-reverce-vector vector val)
  (let loop ([i (sub1 (vector-length vector))])
    (if (or (<= i 0) (equal? val (vector-ref vector i)))
        i
        (loop (sub1 i))))
  )
  
(define (find-thresold vector thresold)
  (find-reverce-vector 
   (vector-map (lambda (x) (< x thresold)) vector)
   #f  ))
  
(define (make-g165-1 input-level delay-time echo-loss required-level filter-params)
  (let* ([R-in (generate-white-noise input-level)]
         [filter-len (last filter-params)]
         [S-in (delay-signal (attenuate R-in echo-loss) delay-time)]
         [filter-mem (malloc (c-sizeof-rnlms filter-len) )]
         [err-out-mem (malloc _int16 (vector-length R-in) )])
    
    (apply c-rnlms-init-struct filter-mem  filter-params)

;    (c-rnlms-set-options filter-mem 'OPT_INHIBIT_ADAPTATION)
    
    (c-rnlms-process filter-mem (vector->mem R-in _int16) (vector->mem S-in _int16) err-out-mem (vector-length R-in))

;    (free filter-mem)
   (print-mem err-out-mem _int16 (vector-length R-in) "./err_out.txt")
   
;    (print (mem->vector err-out-mem _int16 (vector-length R-in)))
    (find-thresold (signal->levels (mem->vector err-out-mem _int16 (vector-length R-in)) 100 400) required-level)
;    (signal->levels (mem->vector err-out-mem _int16 (vector-length R-in)) 100 400)
    ))


(print (make-g165-1 -30 190 -6 -48 '(1.0 0.3 0.9999 2000)))


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;g165-2;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



(define (make-g165-2 input-level delay-time echo-loss required-level filter-params)
  (let* ([R-in (generate-white-noise input-level)]
         [filter-len (last filter-params)]
         [S-in (delay-signal (attenuate R-in echo-loss) delay-time)]
         [filter-mem (malloc (c-sizeof-rnlms filter-len) )]
         [err-out-mem (malloc _int16 (vector-length R-in) )])

    (c-rnlms-set-options filter-mem 'OPT_INHIBIT_ADAPTATION)

    
    
  ))
