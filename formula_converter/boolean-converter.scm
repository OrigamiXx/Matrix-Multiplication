;; Boolean formula converter

;; So it can be used on commandline, for example:
;; scm --script boolean-converter.scm | tail -n+5 > output.cnf
;; scheme --script boolean-converter.scm | tail -n+5 > output.cnf

(load "~/chez-init.scm")

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

    (boolean-formula
     ("(delta" identifier identifier ")")
     delta-formula)

    )
  )

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
  (true-simple)

  (false-simple)
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
						  (expand* (or-many-formula var (+ lower 1) upper bf) env))]
			      [(= lower upper)
			       (expand* bf (extend-env var upper env))]
			      [else (raise-exception 'expand "Out of order bounds ~s" bf)])]
	   [var-formula (name indexes)
			(var-num
			 (var->num
			  (var-formula-simple name (map (lambda (i) (apply-env i env)) indexes))))]

	   [delta-formula (index1 index2)
	   		  (cond
	   		   [(= (apply-env index1 env) (apply-env index2 env)) (true-simple)]
	   		   [else (false-simple)] )]
	   )))
;; Substitude the variables that are puzzle variables from the
;; original input string to concrete puzzle data
;; input from a 3-d array puzzle
(define subst
  (lambda (bfs p)
    (subst* bfs 'p p)))

(define subst*
  (lambda (bfs var p)
    (cases boolean-formula-simple bfs
	   [var-num (num)
		    (let ([x (list-ref var-ls num)])
		      (cases boolean-formula-simple x
			     [var-formula-simple (var* ixs)
						 (if (equal? var var*)
						      (if (equal?
							  (list-ref
							   (list-ref
							    (list-ref p (- (car ixs) 1))
							    (- (cadr ixs) 1))
							   (- (caddr ixs) 1)) #t)
							 (true-simple)
							 (false-simple))
						     (var-num num))]
			     [else (raise-exception 'subst* "var-ls must stored the wrong thing ~a" (cadr x))]))]
	   [and-formula-simple (bfs1 bfs2)
			       (and-formula-simple (subst* bfs1 var p) (subst* bfs2 var p))]
	   [or-formula-simple (bfs1 bfs2)
			      (or-formula-simple (subst* bfs1 var p) (subst* bfs2 var p))]
	   [not-formula-simple (bfs)
			       (not-formula-simple (subst* bfs var p))]
	   [var-formula-simple (var ixs)
			       (raise "var-formula-simple should not be present in subst, run expand first")]
	   [true-simple () (true-simple)]
	   [false-simple () (false-simple)]
	   )))
;; Simplify the true and false variable from the delta-formula
;; corresponding to the and, or, not formulas.
;; There will be no true false formulas after this step.
(define simplify
  (lambda (bfs)
    (cases boolean-formula-simple bfs
	   [var-num (num) (var-num num)]
	   [and-formula-simple (bfs1 bfs2)
			       (let ([simp1 (simplify bfs1)]
				     [simp2 (simplify bfs2)])
				 (cond
				  [(equal? (true-simple) simp1) simp2]
				  [(equal? (false-simple) simp1) (false-simple)]
				  [(equal? (true-simple) simp2) simp1]
				  [(equal? (false-simple) simp2) (false-simple)]
				  [else (and-formula-simple simp1 simp2)]))]
	   [or-formula-simple (bfs1 bfs2)
			      (let ([simp1 (simplify bfs1)]
				    [simp2 (simplify bfs2)])
				(cond
				 [(equal? (true-simple) simp1) (true-simple)]
				 [(equal? (false-simple) simp1) simp2]
				 [(equal? (true-simple) simp2) (true-simple)]
				 [(equal? (false-simple) simp2) simp1]
				 [else (or-formula-simple simp1 simp2)]))]
	   [not-formula-simple (bfs)
			       (let ([simp (simplify bfs)])
				 (cond
				  [(equal? (true-simple) simp) (false-simple)]
				  [(equal? (false-simple) simp) (true-simple)]
				  [else (not-formula-simple simp)]))]
	   [var-formula-simple (var ixs)
			       (raise "var-formula-simple should not be present in subst, run expand first")]
	   [true-simple () (true-simple)]
	   [false-simple () (false-simple)]
	   )))



;; ====== Reduce Simple Boolean Formulas into Equivalent 3CNFs =========

;; Datatype for representing 3cnfs.  Literals indicated by numbers.
;; Negative numbers indicate negation.
(define-datatype 3cnf-clause 3cnf-clause?
  (clause
   (a integer?)
   (b integer?)
   (c integer?)
   ))


;; Helpers to store and manipulate additional variables created as
;; part of the reduction.
(define reduce-var-count 0)

(define init-reduce!
  (lambda ()
    (set! reduce-var-count 0)))

(define get-new-var-num
  (lambda ()
    (set! reduce-var-count (+ reduce-var-count 1))
    (var->num** reduce-var-count var-ls 0)))

;; The faster verison of var->num* for reduce
;; Since reduce's get-new-var-num will always generate new variable
;; dont need to go through the list like var->num*
(define var->num**
  (lambda (var ls n)
    (set! var-ls (append var-ls (list var)))
      (- (length var-ls) 1) ))


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
	   [true-simple () (raise "true-simple should not be present in reduce, run simplify first")]
	   [false-simple () (raise "false-simple should not be present in reduce, run simplify first")]
	   )))


(define init!
  (lambda ()
    (init-reduce!)
    (init-var-ls!)))

;; display help methods
(define iddisp
  (lambda (x)
    (display x)
    (newline)
    x))

(define gotHere
  (lambda (x)
    (display "Got here")
    (newline)
    x))


;; Parses a given string in concrete syntax of complex Boolean formulas into a 3cnf.
(define process&display
  (lambda (str p)
    (init!)
    (let*
	([res (reduce (simplify (subst  (expand (parse str)) p)))]
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


;; (or (or (and (and (var y1) (var y2)) (var x2)) (and (var y1) (and (not (var y2)) (var x2)))) (or (and (var y1) (and (not (var y2)) (not (var x2)))) (and (not (var y1)) (and (var y2) (not (var x2))))))

;; First part - Forces y's to be result of applying permutation x to the puzzle p.
;; Second part - Checks whether y's don't satisfy strong USP property.
;; Third part - Forces x to be permutations.

(define construct-USP-formula
  (lambda (k s)
    (format "(and (and
                 (and-many i 1 ~d (and-many j 1 ~d (and-many r 1 3 (and-many q 1 3
                      (or (and (var y i j r q) (or-many l 1 ~d (and (var p l j r) (var x l i q)) ) )
                           (and (not (var y i j r q)) (not (or-many l 1 ~d (and (var p l j r) (var x l i q))))) )  ))))

     (not (or (and-many i 1 ~d (and-many j 1 ~d (and (or (and-many o 1 2 (var x i j o))
                                                      (and-many a 1 2 (not (var x i j a))))
                                                 (or (and-many b 2 3 (var x i j b))
                                                      (and-many c 2 3 (not (var x i j c)))) ) ))
           (or-many i 1 ~d (or-many j 1 ~d (or (or (and (and-many d 1 2 (var y i j d d)) (and-many e 3 3 (not (var y i j e e))))
                                               (and (and (and-many f 1 1 (var y i j f f)) (and-many g 2 2 (not (var y i j g g)))) (and-many h 3 3 (var y i j h h))))
                                               (and (and-many n 1 1 (not (var y i j n n))) (and-many m  2 3 (var y i j m m))) )        ))) )
       )


     (and-many q 1 3 (and-many i 1 ~d (and (and (or-many j 1 ~d (var x i j q)) (or-many j 1 ~d (var x j i q)))
                                           (and (and-many j 1 ~d (and-many k 1 ~d (or (delta j k)
                                                                                 (or (or (and (var x i j q) (not (var x i k q)))
                                                                                 (and (not (var x i j q)) (var x i k q)))
                                                                                 (and (not (var x i j q)) (not (var x i k q)))))))
                                                (and-many j 1 ~d (and-many k 1 ~d (or (delta j k)
                                                                                 (or (or (and (var x j i q) (not (var x k i q)))
                                                                                 (and (not (var x j i q)) (var x k i q)))
                                                                                 (and (not (var x j i q)) (not (var x k i q)))))))  )   ) )) )
"


    s k s s     s s     s k                  s s s s s s s)
    ))


;; turn a number 1-3 to a list which is a combination of ture false
(define num->bool
  (lambda (num)
    (cond
     [(= num 1) '(#t #f #f)]
     [(= num 2) '(#f #t #f)]
     [(= num 3) '(#f #f #t)])))

;; puzzle 2d -> puzzle 3d with true false
(define p-simple
  (lambda (puzzle)
    (map (lambda (n) (map num->bool n)) puzzle)))
    ;; Not a Strong USP
    ;(define puzzle
    ;  '( (1 1 2 3)
    ;     (1 3 2 1)
    ;     (1 3 3 1)
    ;     (3 1 1 2)
    ;     (1 1 1 1)
    ;     )      )


    ;; ((1 1 2) (2 2 3) (3 3 1))
     ;(define puzzle
     ;  '( (1 1 2)
     ;     (2 2 3)
     ;     (3 3 1)
     ;     ))



    ;; The 8 by 6 takes 125.6 seconds to check (correct), 6 seconds to generate
    ;; The 12 by 6 takes more than 5 hours
    ;; The 14 by 6 takes more than 10 hours + 3 mins to generate
    ;; Strong 14-by-6 USP
     (define puzzle
       '( (1 2 3 3 1 2)
          (2 3 3 3 1 2)
          (2 1 1 1 2 2)
          (1 3 1 1 2 2)
          (3 2 2 1 2 2)
          (1 1 2 3 2 2)
          (1 3 3 1 3 2)
          (1 1 2 1 1 3)
          (3 1 3 1 1 3)
          (3 2 1 2 1 3)
      ;;    (1 3 1 2 1 3)
      ;;    (3 3 1 1 2 3)
      ;;    (1 2 3 2 2 3)
      ;;    (3 3 3 2 2 3)
          ))

;; ( (2 2 1 3 2) (1 3 1 3 2) (2 1 3 3 2 ) (1 1 1 1 3 ) (3 2 1 1 3 ) (3 3 2 1 3 ) (1 2 2 3 3 ) (2 3 2 3 3 ) )

    ;; Strong 8-by-5 USP
     ;;(define puzzle
      ;; '( (2 2 1 3 2)
      ;;    (1 3 1 3 2)
      ;;    (2 1 3 3 2 )
      ;;    (1 1 1 1 3 )
      ;;    (3 2 1 1 3 )
      ;;    (3 3 2 1 3 )
      ;;    (1 2 2 3 3 )
      ;;    (2 3 2 3 3 )
      ;;    ))
;; echo "#input puzzle" | scheme -q boolean-converter.scm | tail -n+5 > output.cnf
;; ( (2 2 1 3 2) (1 3 1 3 2) (2 1 3 3 2 ) (1 1 1 1 3 ) )
;; $ echo "#input puzzle" | scheme -q boolean-converter.scm | tail -n+5 > output.cnf
;; $~/Desktop/matrix-multiplication/minisat ~/Desktop/matrix-multiplication/formula_converter/output.cnf result.txt

(define puz
    (read (open-input-string  (get-input-string)))
  )
(define display-USP-formula
  (lambda (k s p)
    (process&display (construct-USP-formula k s) p)))



(display-USP-formula (length (car puz)) (length puz) (p-simple puz))
