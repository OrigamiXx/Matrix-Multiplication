;; Boolean formula converter

;; ================ Parser Definitions ==================================

;; This defines the translation from the concrete syntax to the abstract syntax.
;; Whenever  you add to or modify the concrete or abstract syntax the specification
;; below must be updated.

(define the-grammar
  '(

    (boolean-formula
     ("(and" identifier number number boolean-formula ")")
     and-many-formula)

    (boolean-formula
     ("(and" boolean-formula boolean-formula ")")
     and-formula)
   
    (boolean-formula
     ("(or" identifier number number boolean-formula ")")
     or-many-formula)
    
    (boolean-formula
     ("(or" boolean-formula boolean-formula ")")
     or-formula)
    
    (boolean-formula
     ("(not" boolean-forumla ")")
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

