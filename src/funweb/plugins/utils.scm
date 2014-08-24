
(use-modules (ice-9 regex))
(use-modules (ice-9 rw))
(use-modules (ice-9 iconv))
(use-modules (ice-9 popen))
(use-modules (ice-9 rdelim))

(define (displayf var)
  (display var) (newline)
  )

;; video id
(define id_rgx "http://v.youku.com/v_show/id_(.+).html")
(define url_m3u8_p "http://v.youku.com/player/getM3U8/vid/~a/type/~a/ts/v.m3u8")
;;
(define (list_links page start keyword)
  (define re1 "<a[^>]*title=\"([^\"]+)\"[^>]*href=\"(http://[^\"]+)\"")
  (define re2 "<a[^>]*href=\"(http://[^\"]+)\"[^>]*title=\"([^\"]+)\"")
  (define mats1 "")
  (define mats2 "")
  (define urls '())

  (define m1start 0)

  (set! mats1 (string-match re1 page))
  ;(set! mats2 (string-match re2 page))
  
  (displayf (regexp-match? mats1))
  (displayf (regexp-match? mats2))

  (while #t
         (set! mats1 (string-match re1 page m1start))         
         (if (regexp-match? mats1) 
             (begin
               (set! m1start (+ (match:end mats1) 1))
               (displayf (match:count mats1))
                                        ;(displayf (match:string mats1))
               (displayf (match:substring mats1 1))
               (displayf (match:substring mats1))

               ;; get vid
               (let* ((mats_id (string-match id_rgx (match:substring mats1 2))))
                 (if (regexp-match? mats_id)
                     (begin
                       (displayf (match:substring mats_id 1))
                       ;(displayf (format #f url_m3u8_p (match:substring mats_id 1) "flv"))
                                        ;(set! urls (append urls (list 7)))
                       (set! urls (append urls 
                                          (list (string-append (match:substring mats_id 1)
                                                               "###" 
                                                               (match:substring mats1 1)))))
                       ;(displayf urls)
                       (displayf "======")
                       )
                     (begin
                       (displayf "no video id found");
                       )
                     )
                 )
               )
             (begin
               (break)
               )
             )
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
  (append urls '("urls"))
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


(define vm_url_tpl "http://v.youku.com/v_show/id_~a.html")
; http://v.youku.com/v_show/id_XNzUzMDA5MTQw.html
(define (get_you_url vid)
  (define full_output "")
  (define full_url (format #f vm_url_tpl vid))
  (define full_cmd (string-append "../../you-get/you-get -u " full_url))
  (define str "")
  (define url_list '())
  (define jurls "")

  (displayf full_url)
  (displayf full_cmd)

  (let* ((port (open-input-pipe full_cmd))
         (ier 0))
    (while #t
           (begin
             (set! str (read-line port))
             (if (eof-object? str)
                 (begin 
                   (break))
                 (set! full_output (string-append full_output str "\n")))
             ))

    (close-pipe port))

  (displayf full_output)
  (set! url_list (string-split (string-trim-both full_output) #\newline))
  (displayf (car (last-pair url_list)))
  (car (last-pair url_list))
  )
