;; Boolean formula converter

;; ================ Parser Definitions ==================================

;; This defines the translation from the concrete syntax to the abstract syntax.
;; Whenever  you add to or modify the concrete or abstract syntax the specification
;; below must be updated.

(define the-grammar
  '(

    (boolean-formula
     ("(and-many" identifier number number boolean-formula ")")
     and-many-formula)

    (boolean-formula
     ("(and" boolean-formula boolean-formula ")")
     and-formula)
   
    (boolean-formula
     ("(or-many" identifier number number boolean-formula ")")
     or-many-formula)
    
    (boolean-formula
     ("(or" boolean-formula boolean-formula ")")
     or-formula)
    
    (boolean-formula
     ("(not" boolean-formula ")")
     not-formula)

    (boolean-formula
     ("(var" identifier (arbno identifier) ")")
     var-formula)

    ))

;; Sets up the parser using the above concrete <-> abstract grammars.
;; Defines a function call parse that takes a string in the concrete
;; syntax and returns the corresponding abstract syntax tree. You must
;; have defined the-grammar first.
(load "lex-scan-parse.scm")



(define-datatype boolean-formula-simple boolean-formula-simple?
  (and-formula-simple
   (rand1 boolean-formula-simple?)
   (rand2 boolean-formula-simple?))
  (or-formula-simple
   (rand1 boolean-formula-simple?)
   (rand2 boolean-formula-simple?))
  (not-formula-simple
   (rand boolean-formula-simple?))
  (var-formula-simple
   (name symbol?)
   (indexes (list-of number?)))
  (var-num
   (num integer?))
  )

(define var-ls '())
	   
(define var->num
  (lambda (var)
    (cases boolean-formula-simple var
	   [var-formula-simple (name indexes) (var->num*
					       var
					       var-ls 0)]
	   [else (raise-exception 'var->num "Not a var ~a" var)])))

(define var->num*
  (lambda (var ls n)
    (cond
     [(null? ls) (set! var-ls (append var-ls (list var))) n]
     [(equal? var (car ls)) n]
     [else (var->num* var (cdr ls) (+ n 1))])))
      
(define empty-env
  (lambda ()
    (lambda (var)
      (raise-exception 'empty-env "No mapping for ~s." var))))

(define extend-env
  (lambda (var num env)
    (lambda (target-var)
      (cond
       [(equal? target-var var) num]
       [else (env target-var)]))))

(define apply-env
  (lambda (var env)
    (env var)))

(define expand
  (lambda (bf)
    (expand* bf (empty-env))))

(define expand*
  (lambda (bf env)
    (cases boolean-formula bf
	   [not-formula (bf) (not-formula-simple (expand* bf env))]
	   [and-formula (bf1 bf2) (and-formula-simple (expand* bf1 env) (expand* bf2 env))]
	   [or-formula (bf1 bf2) (or-formula-simple (expand* bf1 env) (expand* bf2 env))]
	   [and-many-formula (var lower upper bf)
			     (cond
			      [(< lower upper)
			       (and-formula-simple (expand* bf (extend-env var lower env))
						   (expand* (and-many-formula var (+ lower 1) upper bf) env))]
			      [(= lower upper)
			       (expand* bf (extend-env var upper env))]
			      [else (raise-exception 'expand "Out of order bounds ~s" bf)])]
	   [or-many-formula (var lower upper bf)
			    (cond
			      [(< lower upper)
			       (or-formula-simple (expand* bf (extend-env var lower env))
						  (expand* (and-many-formula var (+ lower 1) upper bf) env))]
			      [(= lower upper)
			       (expand* bf (extend-env var upper env))]
			      [else (raise-exception 'expand "Out of order bounds ~s" bf)])]
	   [var-formula (name indexes)
			(var-num
			 (var->num 
			  (var-formula-simple name (map (lambda (i) (apply-env i env)) indexes))))]
	   )))
