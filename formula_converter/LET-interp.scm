;; LET-interp.scm

(load "helpers.scm")

;; ================ Parser Definitions ==================================

;; This defines the translation from the concrete syntax to the abstract syntax.
;; Whenever  you add to or modify the concrete or abstract syntax the specification
;; below must be updated.

(define the-grammar
  '((program                        ;; <Program> ::= 
     (expression)                   ;;   Concrete    <Expression>
     a-prog)                        ;;   Abstract    (a-prog exp)
    
    (expression                     ;; <Expression> ::= 
     (number)                       ;;   Concrete       <Number> 
     const-exp)                     ;;   Abstract       (const-exp num)
    
    (expression                            ;; <Expression> ::= 
     ("-(" expression "," expression ")")  ;;   Concrete       -(<Expression>,<Expression>)
     diff-exp)                             ;;   Abstract       (diff-exp exp1 exp2)

    (expression                     ;; <Expression> ::= 
     ("zero?(" expression ")")      ;;   Concrete       zero?(<Expression>)
     zero?-exp)                     ;;   Abstract       (zero?-exp exp)
    
    (expression                                             ;; <Expression> ::= 
     ("if" expression "then" expression "else" expression)  ;;   Concrete       if <Expression> then <Expression> else <Expression>
     if-exp)                                                ;;   Abstract       (if-exp exp1 exp2 exp3)
    
    (expression           ;; var
     (identifier)
     var-exp)
    
    (expression
     ("let" identifier "=" expression "in" expression)
     let-exp)

    ))

;; Sets up the parser using the above concrete <-> abstract grammars.
;; Defines a function call parse that takes a string in the concrete
;; syntax and returns the corresponding abstract syntax tree. You must
;; have defined the-grammar first.
(load "lex-scan-parse.scm")


;; =============== Environment Definition =============================

;; This is an implementation of the var-val pair list representation
;; of an environment, we wrote earlier.  I translated the
;; representation into a define-datatype so we get the constructors
;; and type checking predicate for free, and can use cases to process.

(define-datatype environment environment?
  (empty-env)                   ;; (empty-env) gives an empty environment
  (extend-env                   ;; (extend-env var val env) extends the environment
   (var symbol?)
   (val expval?)
   (env environment?))
  )

;; (apply-env env target-var) s to figure out the maping of target-var
;; in the environment env.
(define apply-env ; Env x Var -> SType
  (lambda (env target-var)
    (cases environment env
	   [empty-env () (raise-exception 'apply-env "No binding for ~s" target-var)]
	   [extend-env (var val env*) 
		       (cond
			[(equal? var target-var) val]
			[else (apply-env env* target-var)])])))

;; ==================== Expressed Values ==================================

(define-datatype expval expval?
  (num-val
   (num number?))
  (bool-val
   (b boolean?))
  )

(define expval->num
  (lambda (ev)
    (cases expval ev
	   [num-val (num) num]
	   [else (raise-exception 'expval->num "Expressed value is not a number: ~s" ev)])))

(define expval->bool
  (lambda (ev)
    (cases expval ev
	   [bool-val (b) b]
	   [else (raise-exception 'expval->bool "Expressed value is not a Boolean: ~s" ev)])))

(define expval->string
  (lambda (ev)
    (cases expval ev
	   [bool-val (b) (if b "#true" "#false")]
	   [num-val (num) (number->string num)]
	   )))


;; ==================== Evaluater =========================================
(define value-of-prog
  (lambda (prog env)
    (cases program prog
	   [a-prog (exp)  (value-of-exp exp env)]
	   [else (raise-exception 'value-of-prog "Abstract syntax case not implemented: ~s" (car prog))])))

(define value-of-exp
  (lambda (exp env)
    (cases expression exp
	   [const-exp (num) (num-val num)]
	   [diff-exp (rand1 rand2) (num-val (- (expval->num (value-of-exp rand1 env)) (expval->num (value-of-exp rand2 env))))]
	   [zero?-exp (exp1) (bool-val (= (expval->num (value-of-exp exp1 env)) 0))]
	   [if-exp (exp1 exp2 exp3) 
		   (let
		       ([val1 (expval->bool (value-of-exp exp1 env))])
		     (if val1 (value-of-exp exp2 env) (value-of-exp exp3 env)))]
	   [var-exp (var) (apply-env env var)]
	   [let-exp (var exp1 exp2)
		    (let 
			([val1 (value-of-exp exp1 env)])
		      (value-of-exp exp2 (extend-env var val1 env)))]
	   [else (raise-exception 'value-of-exp "Abstract syntax case not implemented: ~s" (car exp))])))


;; =================== Interpreter =========================================
;; (start) -- Starts the interpreter.
(define start
  (lambda ()
    (begin
      (display "\n=== Welcome to the CSC-370 Almost-LET Interpreter === \n\n")
      (read-eval-print (empty-env)))))

;; (get-input-string) -- Reads a line from the interactive input
;; port.  Ignores zero length strings.
(define get-input-string
  (lambda ()
    (let ([str (get-line (current-input-port))])
      (if (= (string-length str) 0) 
	  (get-input-string)
	  str))))

;; (read-eval-print) -- Main read, eval, and print loop.
(define read-eval-print
  (lambda (env)
    ;; Display an interpreter prompt
    (display "==> ")
    ;; Read a line user input
    (let ([concrete-code (get-input-string)])
      (cond
       [(equal? concrete-code "!quit")  
	(display "Goodbye!")  ;; Quit if 'quit entered.
	(newline)]
       [else
	(guard
	 (ex
	  [else
	   (display "PARSE ERROR: \n")
	   (display-exception ex)])
	 ;; Parse code, eval expression, and print result.
	 (let
	     ([abstract-code (parse concrete-code)])
	   (guard
	    (ex
	     [else
	      (display "RUNTIME ERROR: \n")
	      (display-exception ex)])
	    (display (expval->string (value-of-prog abstract-code env)))
	    (newline))))
	;; "Loop".  Notice it is tail recursive.
	(read-eval-print env)]
       ))))

