;; lex-scan-parse.scm
;; Contains lexical specification and code that generates the scanner
;; and parser from the SLLGEN library.

;; You only need the parse function that this code define.

;; WARNING: Do NOT modify this file unless explicitly instructed to do so.

;; ================ Scanner Definitions ==================================

;; This define the lexical specification of the scanner
;; Handles whitespace, comments, identifiers, numbers, and strings
(define the-lexical-spec
   
  '(;; Skips over all whitespace.
    (whitespace (whitespace) skip)

    ;; commit - Skips over everything following % until a newline.
    (comment ("%" (arbno (not #\newline))) skip)
    
    ;; identifier - begin with a letter followed by any number of
    ;; letters, digits, _, -, or ?
    (identifier (letter (arbno (or letter digit "_" "-" "?"))) symbol)

    ;; number - literal positive and negative integers and real numbers
    (number (digit (arbno digit)) number)            
    (number ("-" digit (arbno digit)) number)        
    (number ("-" digit (arbno digit) "." digit (arbno digit)) number)  
    (number (digit (arbno digit) "." digit (arbno digit)) number)

    ;; string - sequences of characters between double quotes
    (string ("\"" (arbno (not #\")) "\"") string)  
    )
  )

;; ================= SLLGEN Boilerplate ==================================
;; WARNING: Presumes a variable the-grammar has be created that stores the
;; mapping between the concrete and abstract syntax.
  
;; This command makes the abstract grammar from the description in the-grammar
;; With this you don't need to use define-datatypes
(sllgen:make-define-datatypes the-lexical-spec the-grammar)

(define show-the-datatypes
  (lambda () (sllgen:list-define-datatypes the-lexical-spec the-grammar)))

;; Use to translate from concrete syntax to abstract syntax
(define scan&parse
  (sllgen:make-string-parser the-lexical-spec the-grammar))

(define just-scan
  (sllgen:make-string-scanner the-lexical-spec the-grammar))

;; Takes a string in the concrete syntax and parses it into the abstract syntax.
(define parse
  (lambda (str)
    (scan&parse str)))
