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
(if (and "T" "T") {print "and test1 passed\n"} {print "and test1 failed\n"})
(if (and "T" ()) {print "and test2 failed\n"} {print "and test2 passed\n"})
(if (and () "T") {print "and test3 failed\n"} {print "and test3 passed\n"})
(if (and () ()) {print "and test4 failed\n"} {print "and test4 passed\n"})

;logic or
(if (or () ()) {print "or test1 failed\n"} {print "or test1 passed\n"})
(if (or "T" ()) {print "or test2 passed\n"} {print "or test2 failed\n"})
(if (or () "T") {print "or test3 passed\n"} {print "or test3 failed\n"})
(if (or "T" "T") {print "or test4 passed\n"} {print "or test4 failed\n"})

;loop test
(let {(x 100)} {loop {not (= x 0)} {seq {{print x "\n"} {as x (- x 1)}}}})

;join test
(join {1 2 3 4 5} {6 7 8 9 10})

;native test
(native "./libnative_test.so" "test_func")
((native "./libnative_test.so" "test_func") 1 "hello" 2.0 {list} native)
(set {f} (native "./libnative_test.so" "test_func"))
(f "hello" "world")

;mod test
(mod 3 2)
(mod -11 6)

(print "\\")

(print 1.0 "\t" 2 "\t" "hello" "\n")

((Y (lambda {fib} {lambda {n} 
			{if (= n 0) 
				0 
				{if (= n 1) 
					1
					{+ (fib (- n 1)) (fib (- n 2))}
				}}})) 25)
