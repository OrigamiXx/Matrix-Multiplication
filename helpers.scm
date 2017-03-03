;; helpers.scm
;; Contains miscellaneous helper functions.
;; Currently only contains exception handling code. Version 2.

;; ================== Error Handling & Display ============================

;; Datatype for encapsulating exception information for later
;; display. Has one variant: (ex-val who format data)
;;   who = a symbol specifying the name of the function the exception
;;         occured in.
;;   format = an eopl:printf appropriate format string indicating the
;;            type of error.
;;   data = a list of values to put into the format string when displayed.
(define-datatype except except?
  (runtime-except-val
   (who symbol?)
   (format string?)
   (data list?))
  (parse-except-val
   (who symbol?)
   (format string?)
   (data list?))
)

;; Displays the exception message.  The input to this function should
;; be data constructed using (except-val who format data). In the case the
;; data is not given via an except-val, the function assumes the exception
;; was thrown by Scheme (not us) and attempts to display it.
(define display-exception
  (lambda (e)
    (cond
     [(except? e)   ;; Raised by us.
      (cases except e
	     [runtime-except-val (who format data) 
				 (display "Runtime Error:\n")
				 (apply eopl:printf (cons format data))
				 (newline)]
	     [parse-except-val (who format data) 
			       (display "Parse Error:\n")
			       (apply eopl:printf (cons format data))
			       (newline)])]
     [else          ;; Raised by Scheme.
      (display "Exception raised by Scheme:\n")
      (set! doh e)
      (if (who-condition? e)
	  (apply printf (cons "  Exception occured in ~:s:" (list (condition-who e))))
	  #t
	  )
      (if (and (message-condition? e) (irritants-condition? e))
	  (apply printf (cons (string-append "  " (condition-message e)) (condition-irritants e)))
	  (if (message-condition? e)
	      (apply printf (list (condition-message e)))
	      #t
	      ))
      (newline)]
     )))

(define raise-exception-maker
  (lambda (evar)
    (lambda (who format . data)
      (raise (evar who format data))
      )))

;; Overrides sllgen:error to prevent stopping.
(define sllgen:error (raise-exception-maker parse-except-val))
(define raise-exception (raise-exception-maker runtime-except-val))
