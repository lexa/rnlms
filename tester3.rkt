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

(define (rotate-signal signal rotate-time)
  (vector-append (vector-take-right signal rotate-time) (vector-drop-right signal rotate-time)))  
  

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
  
(define c-rnlms-init-struct (get-ffi-obj "rnlms_init" lib (_fun (data alpha betta err-buf-len filter-len) :: (data : _pointer) (alpha : _num)(betta : _num) (err-buf-len : _size_t) (filter-len : _size_t) -> _int)))

(define c-rnlms-process (get-ffi-obj "rnlms_process" lib (_fun (hnd x_arr y_arr err_out size) :: (hnd : _pointer) (x_arr : _pointer) (y_arr : _pointer) (err_out : _pointer) (size : _size_t) -> _int)))

(define c-sizeof-rnlms (get-ffi-obj "sizeof_rnlms" lib (_fun (P filter-len) :: (P : _size_t) (filter-len : _size_t) -> _size_t)))

(define _rnlms_options (_bitmask '(OPT_INHIBIT_ADAPTATION = 1 OPT_DISABLE_NONLINEAR_PROCESSING = 2)))

(define c-rnlms-set-options (get-ffi-obj "rnlms_set_options" lib (_fun (mem options) :: (mem : _pointer) (options : _rnlms_options) -> _int)))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;testing-algo;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define rnlms% (class object%
                 
                 (init alpha betta P filter-len)
                 (super-new)
                 
                 (define filter-mem
                   (let ([mem (malloc (c-sizeof-rnlms P filter-len) )])
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
                   )))
                 

;;;;;;;;;;;;;;;;;;;;;;;;;;;;object-rnlms;;;;;;;;;;;;;;;;;;;;;;;;;;;;

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


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;g165-1;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define (make-g165-1 input-level delay-time echo-loss required-level filter-params)
  (let* ([R-in (generate-white-noise input-level)] ;;near
;         [S-in (rotate-signal (attenuate S-in echo-loss) delay-time)]
         [S-in (vector-take (delay-signal (attenuate R-in echo-loss) delay-time) (vector-length R-in))  ] ;; far
         [filter (apply make-object rnlms% filter-params)]
         [err-signal (send filter process S-in R-in)])

    (with-output-to-file (format "g165-1 ~a ~a ~a ~a ~a.dat" input-level delay-time echo-loss required-level filter-params)
      #:mode 'text
      #:exists 'replace
      (lambda () (vector-map  displayln (signal->levels err-signal 100 400))))

    
    (if (> required-level (signal-level err-signal))
        (printf "PASS: ~a~%" (signal-level err-signal))
        (printf "FAIL: ~a~%" (signal-level err-signal)))

    (list
     filter-params
     (find-thresold (signal->levels err-signal 100 400) required-level)
     (signal-level err-signal))
    ))





;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;g165-2;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


(define (make-g165-2 input-level delay-time echo-loss required-level filter-params)
  (let* ([R-in (generate-white-noise input-level)]
         [len (vector-length R-in)]
         [echo (vector-take (delay-signal (attenuate R-in echo-loss) delay-time) len)]
         [N (generate-white-noise -10)] ;;-10 согласно требованиям теста
         [first-sample-len 1000]
         [sampling 8000]
         [filter (apply make-object rnlms% filter-params)])

    (send filter set-options 'OPT_INHIBIT_ADAPTATION)

    (send filter process (vector-take (vector-map + echo N) first-sample-len) (vector-take R-in first-sample-len) )
    
    (send filter set-options '())
    
    (send filter process (vector-take (vector-drop echo first-sample-len) (/ sampling 2)) (vector-take (vector-drop R-in first-sample-len) (/ sampling 2))) 
    
    (send filter set-options 'OPT_INHIBIT_ADAPTATION)

    (let ([combined-loss
           (- input-level
              (signal-level
               (send filter process (vector-take (vector-drop echo (+ (/ sampling 2) first-sample-len)) (/ sampling 2)) (vector-take (vector-drop R-in (+ (/ sampling 2) first-sample-len)) (/ sampling 2)))))])
      (if (> combined-loss required-level)
          (printf "PASS: ~a~%" combined-loss)
          (printf "FAIL: ~a~%" combined-loss)))
    ))

;; (for ([mem-factor '(0.95 0.9 0.99 0.999 0.9999 0.99999 0.999999 0.9999999)])
;;      (let ([params (list 1.0 0.3 mem-factor 200)])
;;        (printf "params: ~a~%" params)
;;        (printf "g165-1  ")
;;        (make-g165-1 -30 190 -6 -48 params)
;;        (exit)
;;        (printf "g165-2  ")
;;        (make-g165-2 -20 130 -6 27 params)
;;        ))


(define (make-con-test input-level delay-time echo-loss filter-params)
  (let* ([R-in (generate-white-noise input-level)] ;;near
         [S-in (vector-take (delay-signal (attenuate R-in echo-loss) delay-time) (vector-length R-in))  ] ;; far
         [filter (apply make-object rnlms% filter-params)]
         [err-signal (send filter process S-in R-in)]
         [levels (signal->levels err-signal 10 400)])

;;    (with-output-to-file (format "test_~a_~a_~a_~a.dat" input-level delay-time echo-loss  filter-params)
    (with-output-to-file "test.dat"
      #:mode 'text
      #:exists 'replace
      (lambda () (vector-map  displayln err-signal)))

    (list
     filter-params
     (* 10 (find-thresold levels  (signal-level (vector-take err-signal (/ (vector-length err-signal) 4)))))
     (signal-level err-signal)
     (vector-ref levels (sub1 (vector-length levels)))
     )
    ))

;(print (make-con-test -10 64 -10 '(0.1 0.00000001 65 65)))

;; (for/list ([filter-len '(64 128 256 512)])
;;           (for/list ([buf-len '(64 128 256 512)])
;;                     (for/list ([alpha '(0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9 1.0 1.1 1.2 1.3)])
;;                               (displayln (make-con-test -10 filter-len -10 (list alpha 0.00000001 buf-len (add1 filter-len)))))))

(define (main)
  (for/list ([filter-len '(64 128 256 512)])
            (for/list ([buf-len '(64 128 256 512)])
                      (with-output-to-file (format "buf_~a_filt_~a.dat" buf-len filter-len)
                        #:mode 'text
                        #:exists 'replace
                        (lambda ()
                          (for/list ([alpha '(0.01 0.05 0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9 1.0 1.1 1.2 1.3)])
                                    (displayln (make-con-test -10 filter-len -10 (list alpha 0.00000001 buf-len (add1 filter-len)))
                                               )))))))
;(main)

;; (let ([buf-len 512])
;;   (for/list ([alpha '(0.1 0.5 0.9 1.0 1.1 1.3)])
;;             (with-output-to-file (format "alpha_~a_buf_~a.dat" alpha buf-len)
;;               #:mode 'text
;;               #:exists 'replace
;;               (lambda ()
;;                 (for/list ([filter-len '(64 128 256 512 1024)])
;;                           (displayln (make-con-test -10 filter-len -10 (list alpha 0.00000001 buf-len (add1 filter-len)))
;;                                      ))))))



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

(define (test-residual-echo-level input-level echo-path-len adaptation-time filter-params)
  (let*  ([near (read-dat-file (format "g165/filtered_noise_~a.dat" input-level))]
         [far (read-dat-file (format "g165/echo_~a_~a.dat" input-level echo-path-len))]
         [filter (apply make-object rnlms% filter-params)]
;;         [err-signal (send filter process far near )]
         [adaptation-signal (send filter process (vector-take far adaptation-time) (vector-take  near adaptation-time))])

    (send filter set-options 'OPT_INHIBIT_ADAPTATION)
    (let ([residual-signal (send filter process (vector-drop far adaptation-time) (vector-drop  near adaptation-time))])
      
      (with-output-to-file "test.dat"
        #:mode 'text
        #:exists 'replace
        (lambda ()
          (vector-map  displayln adaptation-signal)
          (vector-map  displayln residual-signal)
          ))
      (signal-level residual-signal)
      )))
  
;(displayln (signal-level (read-dat-file (format "g165/filtered_noise_~a.dat" 20))))
(for/list ([alpha '(0.095 0.45 0.9)]
           [filter-len '(128 512 1024)])

          (for/list ([input-level '(10 20 30)])
                    (test-residual-echo-level input-level filter-len 16000 (list alpha 0.00000001 300 filter-len))))


;; (define (test-residual-echo-level input-level delay-time echo-loss filter)
;;   (let* ([near (read-dat-file (format "g165/filtered_noise_~a.dat" 10))] ;;near
;;          [far (read-dat-file (format "g165/echo_~a_~a.dat" 10 128))  ] ;; far
;; ;         [filter (apply make-object rnlms% filter-params)]
;;          [err-signal (send filter process far near)]
;;          [levels (signal->levels err-signal 10 400)])

;; ;;    (with-output-to-file (format "test_~a_~a_~a_~a.dat" input-level delay-time echo-loss  filter-params)
;;     (with-output-to-file "test.dat"
;;       #:mode 'text
;;       #:exists 'replace
;;       (lambda () (vector-map  displayln err-signal)))

;;     (list
;;      (* 10 (find-thresold levels  (signal-level (vector-take err-signal (/ (vector-length err-signal) 4)))))
;;      (signal-level err-signal)
;;      (vector-ref levels (sub1 (vector-length levels)))
;;      )
;;     ))

;;(print (test-residual-echo-level -10 64 -10 (make-object rnlms% 0.1 0.00000001 65 65)))
