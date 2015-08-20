(set {not} (lambda {condition} {if condition () "true"}))
(set {empty} (lambda {lis} {not (first lis)}))

(set {seq} ((lambda {m}
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

(set {loop} ((lambda {m}
			{(lambda {f} {f f})
				(lambda {j}
					{m (lambda {x y} {(j j) x y})})})
		(lambda {f}
			{
				lambda {condition body} {
					if (eval condition)
					{seq {{eval body} {f condition body}}}
				}
			}))
		)

(set {and} (lambda {cond1 cond2} {if cond1 {if cond2 "T"}}))
(set {or} (lambda {cond1 cond2} {not (and (not cond1) (not cond2))}))
