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

(define empty-env
  (lambda ()
    (lambda (var)
      (raise 'empty-env "No mapping for ~s." var))))

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
	   [not-formula (bf) (not-formula (expand bf env))]
	   [and-formula (bf1 bf2) (and-formula (expand bf1 env) (expand bf2 env))]
	   [or-formula (bf1 bf2) (or-formula (expand bf1 env) (expand bf2 env))]
	   [and-many-formula (var lower upper bf)
			     (cond
			      [(< lower upper)
			       (and-formula (expand bf (extend-env var lower env))
					    (expand (and-many-formula var (+ lower 1) upper bf)))]
			      [(= lower upper)
			       (expand bf (extend-env var upper env))]
			      [else (raise 'expand "Out of order bounds ~s" bf)])]
	   [or-many-formula (var lower upper bf)
			    (cond
			      [(< lower upper)
			       (or-formula (expand bf (extend-env var lower env))
					   (expand (and-many-formula var (+ lower 1) upper bf)))]
			      [(= lower upper)
			       (expand bf (extend-env var upper env))]
			      [else (raise-exception 'expand "Out of order bounds ~s" bf)])]
	   

			     
	   )))
