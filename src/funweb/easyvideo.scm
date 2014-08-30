#!/usr/bin/guile
!#
;;
;; usage:
;;     ev.scm url <play|dl>
;;
(use-modules (ice-9 format))
(use-modules (ice-9 popen))
(use-modules (ice-9 rdelim))

(setlocale LC_ALL "en_US.UTF-8")
(setlocale LC_CTYPE "en_US.UTF-8")

(define (displayf x) 
  (display x)
  (newline))

(define turl "http://tv.sohu.com/20140819/n403568411.shtml")
(define cmd "/home/gzleo/opensource/smplayer-mix/src/funweb/you-get/you-get -u ")
(define tvar "")

(set! turl (car (cdr (program-arguments))))
(displayf turl) (newline)

(displayf (string-append "cadr is:" (cadr (program-arguments))))
;(displayf (string-append "caddr is:" (caddr (program-arguments))))
;(displayf (string-append "cadddr is:" (cadddr (program-arguments))))

(define (geturl surl)
  (define lines '())
  (define cmd_output "")
  (define urls '())
  (define pls "")
  (define title "")

  (set! cmd (string-append cmd surl))
  (displayf (format #f "fetching ~a ..." surl))

  (let* ((port (open-input-pipe cmd))
         (str ""))
    (while #t
           (begin
             (set! str (read-line port))
             (if (eof-object? str)
                 (begin
                   (break))
                 (begin
                   (if (or (string-contains str "title:") 
                           (string-contains str "Title:"))
                       (set! title str))
                       ;(displayf (format #f "not title line ~a" str)))
                   (set! cmd_output (string-append cmd_output str "\n"))))
             ))
    (close-pipe port))

  ; (display cmd_output)(newline)(newline)

  (set! lines (string-split (string-trim-both cmd_output) #\newline))
  (set! urls (car (last-pair lines)))
  (set! urls (string-trim-both urls #\[))
  (set! urls (string-trim-both urls #\]))
  ; (display urls) (newline)

  (set! title (string-trim-both (car (last-pair (string-split title #\:)))))
  (displayf title)

  (list title (string-split urls #\,))
   ;(set! pls (string-join (string-split urls #\,)))
   ; (display pls) (newline)
  )

(define (dlvideo urls name path)
  (define cnter 100)
  (define file "")

  (set! path (string-append (getenv "HOME") "/evdl"))
  ; cleanup
  (system (format #f "set -x; rm -vf \"~a/~a\"" path name))

  ; download
  (map (lambda (url)
         (set! cnter (+ cnter 1))
         (set! file (format #f "evpart_~d.mp4" cnter))
         (display (format #f "downloading... ~a to ~a \n" url file))
         (let* ((cmd (format #f "set -x; aria2c -x 5 -k 1M -d \"~a\" -o \"~a\" ~a  2>&1"
                             path file url))
                (port (open-input-pipe cmd))
                (str ""))
           (while #t 
                  (begin
                    (set! str (read-line port))
                    (if (eof-object? str)
                        (begin (break))
                        (display (string-append str "\n")))
                        ))
           (close-pipe port))
         ;; combine
         (system (format #f "set -x; cat \"~a/~a\" | tee -a \"~a/~a\" >/dev/null"
                         path file path name))
         (system (format #f "set -x; rm -vf \"~a/~a\"" path file)))
       urls)
  )

; main
(define title "")
(cond ((equal? "dl" (caddr (program-arguments)))
       (begin
         (displayf "download it")
         (set! tvar (geturl turl))
         (set! title (car tvar))
         (set! tvar (cadr tvar))

         (dlvideo tvar (string-append title ".mp4") "/tmp")
         ))

      ((equal? "play" (caddr (program-arguments)))
       (begin
         (displayf "play it")
         (set! tvar (geturl turl))
         (set! title (car tvar))
         (set! tvar (cadr tvar))

         (system (string-append "smplayer " (string-join tvar)))
         ))

      (else 
       (begin
         (displayf "unknown action:")
      )))

(quit)

;; (set! cmd (string-append cmd turl))
;; (let* ((port (open-input-pipe cmd))
;;        (str ""))
;;   (while #t
;;          (begin
;;            (set! str (read-line port))
;;            (if (eof-object? str)
;;                (begin
;;                  (break))
;;                (set! cmd_output (string-append cmd_output str "\n")))
;;            ))
;;   (close-pipe port))

;; (display cmd_output)(newline)(newline)

;; (set! lines (string-split (string-trim-both cmd_output) #\newline))
;; (define urls (car (last-pair lines)))
;; (set! urls (string-trim-both urls #\[))
;; (set! urls (string-trim-both urls #\]))
;; (display urls) (newline)

;; (define pls (string-join (string-split urls #\,)))
;; (display pls) (newline)

;; (system (string-append "smplayer " pls))






