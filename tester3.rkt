#lang racket
(require ffi/unsafe)


;;;;;;;;;;;;;;;;;;;;;;;;;;;;utilites;;;;;;;;;;;;;;;;;;;;;;;;;;;;


(define (lg  num)
  (/ (log num) (log 10)))

;; (define (signal-level signal)
;;   (* 10 (lg (add1 (/ (foldl + 0 (map sqr signal)) (length  signal))))))

;; (define-syntax-rule (add! x y)
;;   (set! x (+ x y)))
  
(define (signal-level signal)
  (- (* 10 (lg (add1
                (/ (let loop ([i (sub1 (vector-length signal))] [sum 0])
                     (if (< i 0)
                         sum
                         (loop (sub1 i) (+ sum (sqr (vector-ref signal i))))))
                   (vector-length signal)))))
     90.30899870323904 ;; 90.30899870323904 == (signal-level #(32768))
     ))
  
                 
(define (int->float bytes)
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

 ;; (define-syntax my-loop
 ;;   (syntax-rules ()
 ;;     ((_ (bindings) (exit-condition)  body0 body1 ...)
 ;;      (let loop (exit-already)
 ;;        (when (not exit-already)
 ;;          (let (bindings)
 ;;            body0
 ;;            body1
 ;;            ...
 ;;            (loop exit-condition)
 ;;            ))))))

  
;;;;;;;;;;;;;;;;;;;;;;;;;;;;white noise generator ;;;;;;;;;;;;;;;;;;
;;TODO

(define (generate-white-noise level)
  (let* ([filename (string-append "g165/filtered_noise_" (number->string (- level)) ".dat")]
         [file (open-input-file filename #:mode 'binary)]
         [data (make-vector (/ (file-size filename) 2))])
    (let loop ([i 0])
      (let ([num (read-bytes 2 file)])
        (when (not (eof-object? num))
          (begin
            (int->float num)
            (vector-set! data i (int->float num))
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
    (vector-map (lambda (x) (* x coeff)) signal)))
  
  
;;;;;;;;;;;;;;;;;;;;;;;;;;;;test-generic;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define test-interface (interface () get-test-data check-test-results))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;test-g165;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define test-g165-1% (class* object%
                      (test-interface)
                      (super-new)
                      (init input-level delay-time echo-loss)
                      
                      (define R-in (generate-white-noise input-level))
                      (define S-in (delay-signal (attenuate R-in echo-loss) delay-time))

                      ;;возвращает R_in и S_in
                      (define/public (get-test-data)
                        (values R-in S-in))
                      (define/public (check-test-results R-out S-out)
                        ;;TODO
                        #t
                        )))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;testing-algo;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;; (define algo% (class object%
                
;;                 (super-new)
;;                 (init init-func process-func filter-params)

;;                 (define filter (apply init-func filter-params))
;;                 (define process-func process-func)
                
;;                 (define/public (run-test test)
;;                   (send test check-test-results (call-with-values (send test get-test-data) process-func))
;;                   )))

(define c-algo% (class object%
                  
                (super-new)

                (init sizeof-func init-struct-func c-process-func filter-params)
                
                (define filter-mem
                  (let ([mem (malloc (apply sizeof-func filter-params))])
                    (apply init-struct-func mem filter-params)
                    mem))
                
                (define process-func (lambda (. args) (apply c-process-func mem args)))
                
                (define/public (run-test test)
                  (call-with-values
                      (call-with-values
                          (send test get-test-data) process-func)
                    (lambda(. args) send/apply test check-test-results args)
                  ))
                ))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;c-rnlms;;;;;;;;;;;;;;;;;;;;;;;;;;;;
                     
(define _size_t (make-ctype _ulong #f #f))
(define _num _float)

(define lib (ffi-lib "/home/lexa/develop/rnlms/librnlms.so"))

(define sizeof_rnlms  (get-ffi-obj "sizeof_rnlms" lib (_fun _size_t -> _size_t)))
(define rnlms_init_struct (get-ffi-obj "rnlms_init_struct" lib (_fun (data betta delta memory-factor filter-len) :: (data : _pointer) (betta : _num)(delta : _num) (memory-factor : _num) (filter-len : _size_t) -> _int)))

(define rnlms_process (get-ffi-obj "rnlms_process" lib (_fun (hnd x_arr y_arr err_out size) :: (hnd : _pointer) (x_arr : _pointer) (y_arr : _pointer)
(new c-algo% [sizeof-func (lambda (. args) (sizeof_rnlms (last args)))] [init-struct-func rnlms_init_struct] [c-process-func

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; (define (init-rnlms BETTA DELTA MEMORY_FACTOR filter_len)
;;   (let ([mem (malloc (apply sizeof-func filter-params))]
;;         [c-rnlms-init (get-ffi-obj "rnlms_init_struct" lib (_fun (data betta delta memory-factor filter-len) :: (data : _pointer) (betta : _num)(delta : _num) (memory-factor : _num) (filter-len : _size_t) -> _int))])
                      
;;     (apply c-rnlms-init (list mem BETTA DELTA MEMORY_FACTOR filter_len))
;;     mem))

;;(define (c-process-func far-data near-data)
  
  

;; (define sizeof_rnlms  (get-ffi-obj "sizeof_rnlms" lib (_fun _size_t -> _size_t)))

;; (define c_rnlms_init (get-ffi-obj "rnlms_init" lib (_fun (data betta delta memory-factor filter-len) :: (data : _pointer) (betta : _num)(delta : _num) (memory-factor : _num) (filter-len : _size_t) -> _pointer)))

;; (define c_rnlms_func (get-ffi-obj "rnlms_func" lib (_fun (data far near err output) :: (data : _pointer) (far : _num)(near : _num) (err : _pointer) (output : _pointer) -> _pointer)))

;; (define (make-test filter-len  far-filename near-filename err-filename out-filename)
;;   (let ([far-file (open-input-file far-filename #:mode 'binary)]
;;         [near-file (open-input-file near-filename #:mode 'binary)]
;;         [err-file (open-output-file err-filename #:mode 'text #:exists 'replace)]
;;         [out-file (open-output-file out-filename #:mode 'text #:exists 'replace)]

;;         [filter-mem (malloc (rlms_sizeOfRequiredMemory filter-len) 'raw)]
;;         [err (malloc _num)]
;;         [out (malloc _num)]
        
;;         )
;;     (register-finalizer filter-mem (lambda (filter-mem) (free filter-mem)))
;;     (c_rnlms_init filter-mem 1.0 0.3 0.9999 filter-len)
        
;;     (let loop ([far  (read-bytes 2 far-file)]
;;                [near (read-bytes 2 near-file)])
      
;;       (unless (ormap eof-object? (list far near))
;;         (begin
;;           (c_rnlms_func filter-mem (int->float far) (int->float near) err out)

;;          (displayln (ptr-ref err _num) err-file)
;;          (displayln (ptr-ref out _num) out-file)
;;          (loop (read-bytes 2 far-file) (read-bytes 2 near-file))
         
;;         )))
;;     (free filter-mem)
;;     ; (free err)
;;     ;; (free out)
;;     (close-input-port far-file)
;;     (close-input-port near-file)
;;     (close-output-port err-file)
;;     (close-output-port out-file)
;;     ))

;; (make-test 100  "/home/lexa/develop/rnlms/g165/filtered_noise_10.dat" "/home/lexa/develop/rnlms/g165/echo_10_128.dat" "/home/lexa/develop/rnlms/error.txt" "/home/lexa/develop/rnlms/output.txt")
