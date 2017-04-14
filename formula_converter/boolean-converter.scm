;; Boolean formula converter

;; Defines concrete and initial abstract syntax for Boolean formulas.
;; Permits large operators and indexed variables.

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

;; A simpler abstract syntax for Boolean formulas without large
;; operators and indexed variables.
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

;; ======== Converting Complex Boolean Formulas into Simple ones ============

;; List of variable names already used.
(define var-ls '(NULL))  

;; Initializes list of variables names used to empty.
(define init-var-ls!
  (lambda ()
    (set! var-ls '(NULL))))

;; Converts a variable to a number by first looking up in var-ls.  If found
;; it returns that number, otherwise it is assigned the next unique number.
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

;; Datatype for maintaining variable indexes in scope.
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

;; Converts a Boolean formula in the initial abstract syntax with
;; large operations and indexed variable into a simple Boolean formula
;; with concrete variables.
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



;; ====== Reduce Simple Boolean Formulas into Equivalent 3CNFs =========

;; Datatype for representing 3cnfs.  Literals indicated by numbers.
;; Negative numbers indicate negation.
(define-datatype 3cnf-clause 3cnf-clause?
  (clause
   (a integer?)
   (b integer?)
   (c integer?)
   )
  )


;; Helpers to store and manipulate additional variables created as
;; part of the reduction.
(define reduce-var-count 0)

(define init-reduce!
  (lambda ()
    (set! reduce-var-count 0)))

(define get-new-var-num
  (lambda ()
    (set! reduce-var-count (+ reduce-var-count 1))
    (var->num* reduce-var-count var-ls 0)))

;; Creates a list of clauses for subformula of the form x = y AND z.
(define make-and-clauses ;; int x int x int -> Listof(3cnf-clause)
  (lambda (x y z)
    (list
     (clause (- x) y (- z))
     (clause (- x) (- y) z)
     (clause (- x) y z)
     (clause x (- y) (- z)))))

;; Creates a list of clauses for subformula of the form x = y OR z.
(define make-or-clauses ;; int x int x int -> Listof(3cnf-clause)
  (lambda (x y z)
    (list
     (clause (- x) y z)
     (clause x (- y) (- z))
     (clause x (- y) z)
     (clause x y (- z)))))

;; Creates a list of clauses for subformula of the form x = !y.
(define make-not-clauses ;; int x int -> Listof(3cnf-clause)
  (lambda (x y)
    ;; x = !y
    ;; (x or y) and (not x or not y)
    (list
     (clause x y y)
     (clause (- x) (- y) (- y)))))
    
;; Converts a simple Boolean formula into a list of 3cnf clauses and
;; an integer indicating the variable of the output gate.
(define reduce ;; bfs -> Listof(3cnf-clause) X int
  (lambda (bfs)
    (cases boolean-formula-simple bfs
	   [var-num (num) (cons '() num)]
	   [and-formula-simple (bfs1 bfs2)
			       (let* ([new (get-new-var-num)]
				      [fc (reduce bfs1)]   
				      [sc (reduce bfs2)]
				      [fcv (cdr fc)]
				      [scv (cdr sc)])
				 (cons (append (make-and-clauses new fcv scv) (car fc) (car sc))
				       new))]	   
	   [or-formula-simple (bfs1 bfs2)
			      (let* ([new (get-new-var-num)]
				      [fc (reduce bfs1)]   
				      [sc (reduce bfs2)]
				      [fcv (cdr fc)]
				      [scv (cdr sc)])
				(cons (append (make-or-clauses new fcv scv) (car fc) (car sc))
				      new))]
	   [not-formula-simple (bfs)
			       (let* ([new (get-new-var-num)]
				      [fc (reduce bfs)]
				      [fcv (cdr fc)])
				 (cons (append (make-not-clauses new fcv) (car fc))
				       new))]
	   [var-formula-simple (var ixs)
			       (raise "var-formula-simple should not be present in reduce, run expand first")]
	   )))


(define init!
  (lambda ()
    (init-reduce!)
    (init-var-ls!)))
     

;; Parses a given string in concrete syntax of complex Boolean formulas into a 3cnf.
(define process&display
  (lambda (str)
    (init!)
    (let*
	([res (reduce (expand (parse str)))]
	 [clauses (car res)]
	 [var (cdr res)]
	 ;; Forces the output variable of reduction to be true.
	 [clauses2 (cons (clause var var var) clauses)])
      (format-output clauses2)
      )))

(define format-output
  (lambda (clauses)
    (printf "p cnf ~d ~d\n" (- (length var-ls) 1) (length clauses))
    (format-output* clauses)))

(define format-output*
  (lambda (clauses)
    (cond
     [(not (null? clauses))
      (let ([c (car clauses)])
	(cases 3cnf-clause c
	       [clause (x y z) (printf "~d ~d ~d 0 \n" x y z)])
	(format-output* (cdr clauses)))])))


;; (get-input-string) -- Reads a line from the interactive input
;; port.  Ignores zero length strings.
(define get-input-string
  (lambda ()
    (let ([str (get-line (current-input-port))])
      (if (= (string-length str) 0) 
	  (get-input-string)
	  str))))


;;(process&display (get-input-string))
;; So it can be used on commandline, for example:
;; echo "(not (var x))" | scm -q boolean-converter.scm | tail -n+5 > simple_not.cnf



;; (or (or (and (and (var y1) (var y2)) (var x2)) (and (var y1) (and (not (var y2)) (var x2)))) (or (and (var y1) (and (not (var y2)) (not (var x2)))) (and (not (var y1)) (and (var y2) (not (var x2))))))


(define construct-USP-formula
  (lambda (k s)
    "(not (var fixme))"
    ))


(define display-USP-formula
  (lambda (k s)
    (process&display (construct-USP-formula k s))))
