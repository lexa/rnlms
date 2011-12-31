#lang racket

(require ffi/unsafe)
(require ffi/cvector)




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;utilites
(define (signal-level signal . optional-args)
  (let ([start (if (null? optional-args) 0 (car optional-args))]
        [len (if (null? optional-args) (cvector-length signal) (cadr optional-args))])
        
    (- (* 10 (lg (add1
                  (/ (let loop ([i (sub1 len)] [sum 0])
                       (if (< i 0)
                           sum
                           (loop (sub1 i) (+ sum (sqr (cvector-ref signal (+ start i)))))))
                     len))))
;       90.30899870323904 ;; 90.30899870323904 == (signal-level #(32768))
       83.11858286715683
       )))

(define (lg  num)
  (/ (log num) (log 10)))







(define mi (list
            (vector -436   -829   -2797  -4208  -17968 -11215 46150  34480  -10427 9049   -1309  -6320  390     -8191   -1751   -6051   -3796   -4055 -3948  -2557  -3372  -1808  -2259  -1300  -1098 -618 -340 -61  323  419   745    716    946    880    1014   976  1033  1091  1053  1042  794   831  899   716   390   313   304   304  73   -119  -109  -176  -359  -407 -512  -580  -704  -618  -685 -791 -772 -820 -839 -724)))



(define (make-echo-answer signal model echo-delay ERL K)

  (define (m i)
    (let ([impulse-response (list-ref mi model)])
      (if (>= (vector-length impulse-response) i)
          (vector-ref impulse-response i)
          0)
    ))
  
  (define (g val)
;;    (* (expt 10 (/ (- ERL) 20)) K val)
    val
    )

  (define (generate-impulse-response )
    
    (let* ([impulse-response (list-ref mi model)]
           [rez (make-vector (+ (vector-length impulse-response) echo-delay) 0)])      
      (vector-copy! rez echo-delay (vector-map g impulse-response))
      rez
      ))

  (define (filter-out impulse-response signal)
    (let ([len (vector-length impulse-response)])
    (do ([i 0 (add1 i)]
         [sum 0 (+ sum (* (vector-ref impulse-response (- len i 1)) (vector-ref signal i)))]
         )
        ((>= i len) sum)
      )))
  
  (define (vector-window vec begin len)
    (let ([rez (make-vector len 0)])
      (vector-copy! rez 0 vec begin (min (+ begin len) (vector-length vec)))
      rez
      ))
  
  (define (filter signal impulse-response)
    (let ([rez (make-vector (vector-length signal))])
      (do ([b 0 (add1 b)])
        ((>= b (vector-length signal)))
        
       (vector-set! rez b (filter-out impulse-response (vector-window signal b (vector-length impulse-response))))
      )
      rez
      )
    )
  (print (generate-impulse-response))
  
  (filter  signal  (generate-impulse-response))
  )

;; (let ([tmp (make-vector 300 1)])
;;   (make-echo-answer tmp 0 00 5 0.000001))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;c-rnlms ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(define _size_t (make-ctype _ulong #f #f))
(define _num _float)
(define _signal (make-ctype _cvector #f #f))

(define lib (ffi-lib "/home/lexa/develop/rnlms/librnlms.so"))

(define _rnlms_options (_bitmask '(OPT_INHIBIT_ADAPTATION = 1 OPT_DISABLE_NONLINEAR_PROCESSING = 2)))
(define c-rnlms-set-options (get-ffi-obj "rnlms_set_options" lib (_fun (mem options) :: (mem : _pointer) (options : _rnlms_options) -> _int)))

(define c-rnlms-init-struct (get-ffi-obj "rnlms_init" lib (_fun (data alpha betta err-buf-len filter-len) :: (data : _pointer) (alpha : _num) (betta : _num) (err-buf-len : _size_t) (filter-len : _size_t) -> _int)))

(define c-sizeof-rnlms (get-ffi-obj "sizeof_rnlms" lib (_fun (P filter-len) :: (P : _size_t) (filter-len : _size_t) -> _size_t)))

(define c-rnlms-process (get-ffi-obj "rnlms_process" lib (_fun (hnd x_arr y_arr err_out size) :: (hnd : _pointer) (x_arr : _signal) (y_arr : _signal) (err_out : _signal) (size : _size_t) -> _int)))

(define rnlms% (class object%
                 
                 (init alpha betta P filter-len)
                 (super-new)
                 
                 (define filter-mem
                   (let ([mem (malloc 'raw (c-sizeof-rnlms P filter-len))])
                     (if (= (c-rnlms-init-struct mem alpha betta P filter-len) 0)
                         mem
                         (error "error in init")
                         )))

                 (define/public (set-options opts)
                   (c-rnlms-set-options filter-mem opts)
                   )

                 (define/public (process far near)
                   (when (not (= (cvector-length far) (cvector-length near)))
                     (error "length of far and near must be the same"))
                   
                   (let* ([len (cvector-length far)]
                          [err-out-mem (malloc _int16 len )])
                     (c-rnlms-process filter-mem far near  err-out-mem len)
                     err-out-mem 
                   ))))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;file with data ;;;;;;;;;;;;;;;;;;;;;;;;;;;;
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

(define (read-dat-file filename)
  (let ([file (open-input-file filename #:mode 'binary)]
         [data (make-vector  (/ (file-size filename) 2))])
    (let loop ([i 0])
      (let ([num (read-bytes 2 file)])
        (when (not (eof-object? num))
          (begin
            (vector-set! data i (bytes->int num))
            (loop (add1 i))))))
    
    (close-input-port file)
    data
    ))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;g165-1;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define (make-g165-1 input-level delay-time adaptation-time ERL k filter-params)
  (let*  ([R-in (read-dat-file (format "g165/filtered_noise_~a.dat" input-level))]
          [S-in (make-echo-answer R-in 0 delay-time ERL k)]
          [filter (apply make-object rnlms% filter-params)]
          [adaptation-signal (send filter process (vector-take S-in adaptation-time) (vector-take R-in adaptation-time))])

    (send filter set-options 'OPT_INHIBIT_ADAPTATION)
    (let ([residual-signal (send filter process (vector-drop S-in adaptation-time) (vector-drop R-in adaptation-time))])
      
      (with-output-to-file "test.dat"
        #:mode 'text
        #:exists 'replace
        (lambda ()
          (vector-map  displayln adaptation-signal)
          (vector-map  displayln residual-signal)
          ))
      (signal-level residual-signal)
      )))

;(make-g165-1 10 200 8000 10 0.0000001 (list 0.45 0.00000001 300 512))

(define input-level 10)
(define ERL 10)
(define k 0.000001)
(let*  ([R-in (read-dat-file (format "g165/filtered_noise_~a.dat" input-level))]
        [S-in (make-echo-answer R-in 0 200 ERL k)]
        [filter (apply make-object rnlms% (list 0.45 0.00000001 300 512))]
        [adaptation-signal (send filter process (vector-take S-in 8000) (vector-take R-in 8000))])
  S-in
  )
