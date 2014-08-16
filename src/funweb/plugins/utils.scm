
(use-modules (ice-9 regex))
(use-modules (ice-9 rw))
(use-modules (ice-9 iconv))


(define (displayf var)
  (display var) (newline)
  )

;;
(define (list_links page start keyword)
  (define re1 "<a[^>]*title=\"([^\"]+)\"[^>]*href=\"(http://[^\"]+)\"")
  (define re2 "<a[^>]*href=\"(http://[^\"]+)\"[^>]*title=\"([^\"]+)\"")
  (define mats1 "")
  (define mats2 "")

  
  (set! mats1 (string-match re1 page))
  ;(set! mats2 (string-match re2 page))
  
  (displayf (regexp-match? mats1))
  (displayf (regexp-match? mats2))

  (if (regexp-match? mats1) 
      (begin
       (displayf (match:count mats1))
       ;(displayf (match:string mats1))
       (displayf (match:substring mats1))
       )
      #f
      )

  (if (regexp-match? mats2) 
      (begin
       (displayf (match:count mats2))
       ;(displayf (match:string mats2))
       (displayf (match:substring mats2))
       )
      #f
      )  

  (list "a" "b" "c")
  )

;;;;;
(define (file_put_contents fname fdata)
  (let* ((tdata "abc123哈哈") 
         (port (open-output-file fname)))
    (set-port-encoding! port "UTF-8")
    ; (write-string/partial tdata "UTF-8" port 0)
    (string-for-each (lambda (c)
                       (write-char c port) 
                       )
                     fdata)
    (close port)
    )
  (string-length fdata)
;  (displayf fname)
  )

(define (file_get_contents fname)
  (let* ((fdata "<!-- ddddd -->")
         (port (open-input-file fname)))
    (set-port-encoding! port "UTF-8")
    (while (not (eof-object? (peek-char port)))
           (set! fdata (string-append fdata (string (read-char port))))
           ;(display " ")
           )

    (displayf (string-length fdata))
    (string-append fdata "<!-- eeee -->")
    
    )
  ;(displayf fname)
  )
