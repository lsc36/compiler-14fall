# compiler hw1

---

## 需求

建置相關

 - g++
 - cmake

測試相關

 - ruby
 - colordiff

## 目前CFG（會更動到完成目標）
Program -> Declarations Statements $
Declations -> Declaration Declarations
            | lambda
Declaration -> floatdcl id
             | intdcl id
Statements -> Statement Statements
            | lambda
Statement -> id = val Expression
           | print id
Expression -> + term Expression
            | - term Expression
            | lambda
term -> val term'
term' -> * val term'
	   | / val term'
	   | lambda
val -> unary- positiveVal
     | positive_val
positiveVal -> id
			  | inum
			  | fnum
