(set {is_nil} (lambda {x} {is_same_type x ()}))
(set {is_int} (lambda {x} {is_same_type x 0}))
(set {is_float} (lambda {x} {is_same_type x 0.0}))
(set {is_string} (lambda {x} {is_same_type x ""}))
(set {is_list} (lambda {x} {is_same_type x {}}))
(set {is_builtin} (lambda {x} {is_same_type x is_same_type}))
(set {is_lambda} (lambda {x} {is_same_type x (lambda {} {})}))

(set {not} (lambda {condition} {if condition () "T"}))		;取反操作
(set {empty} (lambda {lis} {not (first lis)}))			;测试一个列表是否为空

(set {Y} (lambda {m} {(lambda {f} {f f})			;Y-Combinator
		(lambda {j} {m (lambda {x} {(j j) x})})}))

(set {seq} (Y							;对列表中的每个表达式求值并返回最后一个值
	(lambda {f}
		{lambda {seqs}
			{if (empty (rest seqs)) 
				{eval (first seqs)}
				{let {} 
					{eval (first seqs)}
					{f (rest seqs)}}}})))

(set {and} (lambda {cond1 cond2} {if cond1 {if cond2 "T"}}))				;逻辑与
(set {or} (lambda {cond1 cond2} {not (and (not cond1) (not cond2))}))			;逻辑或
(set {xor} (lambda {cond1 cond2} {and (or cond1 cond2) (not (and cond1 cond2))}))	;异或

(set {second} (lambda {lis} {if (rest lis) {first (rest lis)} ()}))			;列表中的第二个元素
(set {assert} (lambda {condition msg} {if (not condition) {seq {{print msg "\n"} {exit}}}}))

(set {>} (lambda {v1 v2} {< v2 v1}))
(set {=} (lambda {v1 v2} {and (not (< v1 v2)) (not (> v1 v2))}))
(set {>=} (lambda {v1 v2} {not (< v1 v2)}))
(set {<=} (lambda {v1 v2} {not (> v1 v2)}))

(set {return} (lambda {retval} {as "" retval}))

(set {join} (lambda {lis1 lis2} {(Y (lambda {f} {
			lambda {lis} {
				seq {
					{assert (and (is_list (first lis)) (is_list (second lis))) "join need two list"}
					{
					if(empty (second lis)) 
						{return (first lis)}
						{f (list (append (first lis) (first (second lis))) (rest (second lis)))}
					}
				}
			}}))(list lis1 lis2)}))

(set {mod} (lambda {x y} {seq {{assert (and (is_int x) (is_int y)) "mod: need two interger"}{- x (* (/ x y) y)}}}))

(set {is_callable} (lambda {x} {or (is_builtin x) (is_lambda x)}))
