(load "../core.lisp")

((lambda {y} {((lambda {y} {lambda {x} {* y 2}}) 3) 0}) 4)

(((lambda {m}
			{(lambda {f} {f f})
				(lambda {j}
					{m (lambda {x} {(j j) x})})})
		(lambda {f}
			{lambda {coll}
				{if (empty coll)
					0
					{+ (first coll) (f (rest coll))}}}))
		{1 2 3 4})
