(set {defun} (lambda {func param_list body} {set func (lambda param_list body)}))
(defun {not} {condition} {if condition () "T"})
(defun {empty} {lis} {not (first lis)})
(defun {seq} {seqs} {if (empty (rest seqs)) {eval (first seqs)} {let {} {eval (first seqs)} {self (rest seqs)}}})
;(defun {loop} {condition body} {let {(ret ())} {if condition {seq {{as ret (eval body)} {self condition body}}} ret}})

;(loop 0 {println "hello"})
