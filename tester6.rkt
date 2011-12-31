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
;       90.30899870323904 ;; 90.30899870323904 == (signal-level #(32768))
       83.11858286715683
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

;;;;;;;;;;;;;;;;;;;;;;;;;;;;white noise generator ;;;;;;;;;;;;;;;;;;

(define (read-dat-file filename)
  (let ([file (open-input-file filename #:mode 'binary)]
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
    (* (expt 10 (/ (- ERL) 20)) K val)
;;    val
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
  ;(print (generate-impulse-response))
  
  (vector-map round (filter signal (generate-impulse-response)))
  
  )

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
;(define _audio_in (make-ctype _cvector i _int16 ))
;(define _audio_out (_cvector i _int16 ))

(define lib (ffi-lib "/home/lexa/develop/rnlms/librnlms.so"))
  
(define c-rnlms-init-struct (get-ffi-obj "rnlms_init" lib (_fun (data alpha betta err-buf-len filter-len) :: (data : _pointer) (alpha : _num) (betta : _num) (err-buf-len : _size_t) (filter-len : _size_t) -> _int)))

(define c-rnlms-process (get-ffi-obj "rnlms_process" lib (_fun (hnd x_arr y_arr err_out size) :: (hnd : _pointer) (x_arr : _pointer) (y_arr : _pointer) (err_out : _pointer) (size : _size_t) -> _int)))

(define c-sizeof-rnlms (get-ffi-obj "sizeof_rnlms" lib (_fun (P filter-len) :: (P : _size_t) (filter-len : _size_t) -> _size_t)))

(define _rnlms_options (_bitmask '(OPT_INHIBIT_ADAPTATION = 1 OPT_DISABLE_NONLINEAR_PROCESSING = 2)))

(define c-rnlms-set-options (get-ffi-obj "rnlms_set_options" lib (_fun (mem options) :: (mem : _pointer) (options : _rnlms_options) -> _int)))
(define c-rnlms-show-debug (get-ffi-obj "rnlms_show_debug" lib (_fun (mem) :: (mem : _pointer) -> _void)))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;testing-algo;;;;;;;;;;;;;;;;;;;;;;;;;;;;

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
                   (when (not (= (vector-length far) (vector-length near)))
                     (error "length of far and near must be the same"))
                   
                   (let* ([len (vector-length far)]
                          [err-out-mem (malloc _int16 len )])
                     (c-rnlms-process filter-mem (vector->mem far _int16) (vector->mem near _int16) err-out-mem len)
                     (mem->vector err-out-mem _int16 len))
                   )
                 (define/public (debug-info)
                   (c-rnlms-show-debug filter-mem)
                   )
                 ))
                 

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;g165-1;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define (make-g165-1 input-level delay-time adaptation-time ERL K filter-params)
  (let*  ([R-in (read-dat-file (format "g165/filtered_noise_~a.dat" input-level))]
;          [S-in (read-dat-file (format "g165/echo_~a_~a.dat" input-level delay-time))]
          [S-in (make-echo-answer R-in 0 delay-time ERL K)]
          [filter (apply make-object rnlms% filter-params)]
          [adaptation-signal (send filter process (vector-take S-in adaptation-time) (vector-take R-in adaptation-time))])

    (send filter set-options 'OPT_INHIBIT_ADAPTATION)
    (let ([residual-signal (send filter process (vector-drop S-in adaptation-time) (vector-drop R-in adaptation-time))])
      
      (with-output-to-file "test.dat"
        #:mode 'text
        #:exists 'replace
        (lambda ()
;          (vector-map  displayln adaptation-signal)
 ;         (vector-map  displayln residual-signal)
          (vector-map  displayln S-in)
          ))
      (signal-level residual-signal)
      )))

;(displayln "g165 - 1")
;(make-g165-1 10 256 40000 800 10 (list 0.45 0.00000001 300 512))

;; (for/list ([filter-len '(128 512 1024 1024)]
;;            [alpha '(0.095 0.45 0.9 0.39)])
;;           (for/list ([l '(10 15 20 25 30)])
;;                     (make-g165-1 l filter-len 40000 (list alpha 0.00000001 300 filter-len))))



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;g165-2;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


(define (make-g165-2 input-level delay-time filter-params)
  (let* ([R-in (read-dat-file (format "g165/filtered_noise_~a.dat" input-level))] ;;near
         [S-in (read-dat-file (format "g165/echo_~a_~a.dat" input-level delay-time))  ] ;; far
         [len (vector-length R-in)]
         [N (read-dat-file  "g165/filtered_noise_10.dat")] ;;-10 согласно требованиям теста
         [first-sample-len 2000]
         [sampling 8000]
         [filter (apply make-object rnlms% filter-params)])

    (send filter set-options 'OPT_INHIBIT_ADAPTATION)

    (send filter process (vector-take (vector-map + S-in N) first-sample-len) (vector-take R-in first-sample-len))
    
    (send filter set-options '())
    
    (send filter process (vector-take (vector-drop S-in first-sample-len) (/ sampling 2)) (vector-take (vector-drop R-in first-sample-len) (/ sampling 2))) 
    
    (send filter set-options 'OPT_INHIBIT_ADAPTATION)

    (let ([combined-loss
           (- (- input-level)
              (signal-level
               (send filter process (vector-take (vector-drop S-in (+ (/ sampling 2) first-sample-len)) sampling) (vector-take (vector-drop R-in (+ (/ sampling 2) first-sample-len))  sampling))))])
      combined-loss
    )))

;; g165-2
;; (displayln "g165 - 2")
;; (for/list ([filter-len '(128 512 1024 1024)]
;;            [alpha '(0.095 0.45 0.9 0.39)])
;;           (for/list ([l '(15 20 25 30)])
;;                     (make-g165-2 l filter-len (list alpha 0.00000001 300 filter-len))))


(define (vector-range vec start len)
  (vector-take (vector-drop vec start) len)
  )


(define (make-g165-4 input-level delay-time filter-params)
  (let* ([R-in (read-dat-file (format "g165/filtered_noise_~a.dat" input-level))] ;;near
         [S-in (read-dat-file (format "g165/echo_~a_~a.dat" input-level delay-time))] ;; far
         [len (vector-length R-in)]
         [sampling 8000]
         [filter (apply make-object rnlms% filter-params)])

    (send filter process (vector-range S-in 0 (* 5 sampling)) (vector-range R-in 0 (* 5 sampling))) ;;5 секунд адаптации
    
    (send filter process (make-vector (* sampling 120) 0) (make-vector (* sampling 120) 0)) ;;2 минуты тишины

    (send filter set-options 'OPT_INHIBIT_ADAPTATION)

    (send filter process (vector-range S-in (* 5 sampling) (* 3 sampling) ) (vector-range R-in (* 5 sampling) (* 3 sampling))) ;;3 секунды подаём сигнал на не адаптирующийся фильтр

    ;;меряем остаточный уровень
    (signal-level
     (send filter process (vector-range S-in (* 8 sampling) (* 2 sampling) ) (vector-range R-in (* 8 sampling) (* 2 sampling)))
     )))


;(make-g165-3 10 1024 (list 0.9 0.00000001 300 1024))

;;g165-4
;; (displayln "g165 - 4")
;; (for/list ([filter-len '(128 512 1024 1024)]
;;            [alpha '(0.095 0.45 0.9 0.39)])
;;           (for/list ([l '(10 15 20 25 30)])
;;                     (make-g165-4 l filter-len (list alpha 0.00000001 300 filter-len))))

;; (make-g165-3 10 128 '(0.095 1e-08 300 128))
;; (make-g165-3 20 128 '(0.095 1e-08 300 128))
;; (make-g165-3 10 512 '(0.45 1e-08 300 512))
;; (make-g165-3 20 512 '(0.45 1e-08 300 512))
;(make-g165-3 10 1024 '(0.4 1e-08 300 1024))
;; (make-g165-3 20 1024 '(0.9 1e-08 300 1024))


(define (make-g165-5 input-level delay-time filter-params)
  (let* ([R-in (read-dat-file (format "g165/filtered_noise_~a.dat" input-level))] ;;near
         [S-in (read-dat-file (format "g165/echo_~a_~a.dat" input-level delay-time))] ;; far
         [len (vector-length R-in)]
         [sampling 8000]
         [filter (apply make-object rnlms% filter-params)])

    (send filter process (vector-range S-in 0 (* 5 sampling)) (vector-range R-in 0 (* 5 sampling))) ;;5 секунд адаптации

    (send filter process (make-vector (/ sampling 2) 0) (vector-range R-in (* 5 sampling) (/ sampling 2))) ;;0.5 секунды отключён эхотракт

    (send filter set-options 'OPT_INHIBIT_ADAPTATION)

    (send filter process (make-vector sampling 0) (vector-range R-in (* 55/10 sampling) sampling)) ;;1 секунды без адаптации, чтоб всё просралось

    (signal-level (send filter process (make-vector (* sampling 2) 0) (vector-range R-in (* 65/10 sampling) (* sampling 2)))) ;;2 секунды меряем уровень
    ))
    

;;g165-5
;; (displayln "g165 - 5")
;; (for/list ([filter-len '(128 512 1024 1024)]
;;            [alpha '(0.095 0.45 0.9 0.39)])
          
;;           (for/list ([l '(10 15 20 25 30)])
;;                     (make-g165-5 l filter-len (list alpha 0.00000001 300 filter-len))))



(let* ([R-in (read-dat-file "echo-in.dat" )] ;;near
       [S-in (read-dat-file "echo-out.dat" )] ;; far
       [len (vector-length R-in)]
       [filter (apply make-object rnlms% (list 0.4 0.00000001 300 512))]
       [err-signal (send filter process (vector-take R-in 80000) (vector-take S-in 80000))])
  
  (with-output-to-file "test.dat"
    #:mode 'text
    #:exists 'replace
    (lambda ()
      (vector-map  displayln err-signal)
      ))
  (send filter debug-info)
  )
