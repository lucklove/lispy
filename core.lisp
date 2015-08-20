(set {not} (lambda {condition} {if condition () "T"}))		;取反操作
(set {empty} (lambda {lis} {not (first lis)}))			;测试一个列表是否为空

(set {seq} ((lambda {m}						;对列表中的每个表达式求值并返回最后一个值
			{(lambda {f} {f f})
				(lambda {j}
					{m (lambda {x} {(j j) x})})})
		(lambda {f}
			{lambda {seqs}
				{
					if (empty (rest seqs)) 
						{eval (first seqs)}
						{let {} 
							{eval (first seqs)}
							{f (rest seqs)}
						}
				}}))
		)

(set {Y} (lambda {m} {(lambda {f} {f f})			;Y因子
		(lambda {j}
				{m (lambda {x} {(j j) x})})}))

(set {Y2} (lambda {m} {(lambda {f} {f f})			;两个参数的版本
		(lambda {j}
				{m (lambda {x y} {(j j) x y})})}))

(set {and} (lambda {cond1 cond2} {if cond1 {if cond2 "T"}}))		;逻辑与
(set {or} (lambda {cond1 cond2} {not (and (not cond1) (not cond2))}))	;逻辑或
(set {second} (lambda {lis} {if (rest lis) {first (rest lis)} ()}))	;列表中的第二个元素
(set {assert} (lambda {condition msg} {if (not condition) {seq {{println msg} {exit}}}}))

(set {loop} (lambda {condition body} {(Y (lambda {f} {					;循环
			lambda {lis} {
				if (eval (first lis))
				{seq {{eval (second lis)} {f lis}}}
			}
		})) (list condition body)}))

