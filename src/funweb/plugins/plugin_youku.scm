(use-modules (web response))
(use-modules (web client))
(use-modules (ice-9 vlist))
(use-modules (ice-9 iconv))
(use-modules (srfi srfi-4 gnu))

(add-to-load-path "./funweb/plugins")
(load-from-path "utils.scm")

(setlocale LC_ALL "en_US.UTF-8")

(define (search keyword page)
  (define url 
    (string-append "http://www.soku.com/search_video/q_" keyword "_orderby_1_page_" page))
  (define res "")
  (define content "")
  (define u8content "")
  (define tdata "")
  (define links '())

  (display (string-append "url:" url))
  (newline)

  ;(set! tdata (file_get_contents "ac.html"))
  (displayf (string-length tdata))
  (displayf "==========")
  ;(displayf tdata)
  (displayf "==========")


          (set! res (http-get url #:decode-body? #t #:streaming? #t))
          (set! content (read-response-body res))
          (set! u8content (bytevector->string content "UTF-8"))

          (display (string-length u8content)) (newline)

          ;(displayf u8content)          
          (file_put_contents "ac.html" u8content)

          (set! links (list_links u8content  "1" "5" ))
  

          (displayf links)

  (list 1 2 3)
  ; (list_links tdata  "1" "5" )
  
  )

;(display (search "韩国" "1"))
;(display (search "%E9%9F%A9%E5%9B%BD" "1"))

(get_you_url "XNzUzMDA5MTQw")

(define content "")

;; (displayf 
;;  (list_links "dddd  <a href=\"aurl\" title=\"ttttttttt567\">nammm</a>"
;;              "1"
;;              "2")
;; )

(displayf 
(list_links " dddd <a title=\"ttttttttt567\" href=\"burl\">nammm</a>"
            "1"
            "3")
)


(display "aaaaaaaaaa")
(newline)

(define (scm_hehe) (display "jjjjjjjj"))

(scm_hehe)
(newline)

(display "aaaaaab")
(newline)

;; (define  res (http-get "http://www.baidu11111222.com" #:decode-body? #t #:streaming? #t))
;; (display res)
;; (newline)
;; (newline)
;; (display (vector? res))
;; (newline)

;; (display (list? res))
;; (newline)
;; (display (array? res))
;; (newline)
;; (display (vlist? res))
;; (newline)
;; (display (response? res))
;; (newline)
;; (define content (read-response-body res))
;; (newline)
;; (display (string? (bytevector->string content "UTF-8")))
;; (newline)
;; (display (bytevector->string content "UTF-8"))
;; (newline)




