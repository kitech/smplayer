#!/usr/bin/guile
!#
;;
;;
;;
;;
(use-modules (ice-9 popen))
(use-modules (ice-9 rdelim))


(define turl "http://tv.sohu.com/20140819/n403568411.shtml")
(define cmd "/home/gzleo/opensource/smplayer-mix/src/funweb/you-get/you-get -u ")
(define lines '())
(define cmd_output "")

(set! turl (car (cdr (program-arguments))))
(display turl) (newline)
; (quit)


(set! cmd (string-append cmd turl))
(let* ((port (open-input-pipe cmd))
       (str ""))
  (while #t
         (begin
           (set! str (read-line port))
           (if (eof-object? str)
               (begin
                 (break))
               (set! cmd_output (string-append cmd_output str "\n")))
           ))
  (close-pipe port))

(display cmd_output)(newline)(newline)

(set! lines (string-split (string-trim-both cmd_output) #\newline))
(define urls (car (last-pair lines)))
(set! urls (string-trim-both urls #\[))
(set! urls (string-trim-both urls #\]))
(display urls) (newline)

(define pls (string-join (string-split urls #\,)))
(display pls) (newline)

(system (string-append "smplayer " pls))






