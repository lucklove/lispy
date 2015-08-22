(load "../core.lisp")

((lambda {y} {((lambda {y} {lambda {x} {* y 2}}) 3) 0}) 4)

;Y
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

;logic and
(if (and "T" "T") {println "and test1 passed"} {println "and test1 failed"})
(if (and "T" ()) {println "and test2 failed"} {println "and test2 passed"})
(if (and () "T") {println "and test3 failed"} {println "and test3 passed"})
(if (and () ()) {println "and test4 failed"} {println "and test4 passed"})

;logic or
(if (or () ()) {println "or test1 failed"} {println "or test1 passed"})
(if (or "T" ()) {println "or test2 passed"} {println "or test2 failed"})
(if (or () "T") {println "or test3 passed"} {println "or test3 failed"})
(if (or "T" "T") {println "or test4 passed"} {println "or test4 failed"})

;loop test
(let {(x 100)} {loop {not (= x 0)} {seq {{println x} {as x (- x 1)}}}})

;join test
(join {1 2 3 4 5} {6 7 8 9 10})

;native test
(native "./libnative_test.so" "test_func")
((native "./libnative_test.so" "test_func") 1 "hello" 2.0 {list} native)
(set {f} (native "./libnative_test.so" "test_func"))
(f "hello" "world")
