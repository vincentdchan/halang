# Ha Language 哈语言
A toy language implementation. Including lexer, parser, code generation and interpreter.



# Usage

By default, the program read the file named "source.txt" in the same location with the program, but you can pass the filename as the parameter to the program.



# Language

This language is similar to JavaScript, but it has differences because this project is not completely finished.

## String

In HaLang, string is a immutable built-in object. It's described within *""*.

```javascript
var a = "Hello World!";
print a // output a
```



## Expression

Similar to JavaScript.

```
2 * (3 + 4) // 14
```

## Print Statement

You can print a expression.

```javascript
print 1 + 1 // output : 2

var a = 3
print a			// output: 3
print a + 1		// output: 4
```

## Assignment

You can define a variable by using *var* statement.

and you can do assign operation everywhere.

```javascript
var a = 1 + 1  // a == 2
var b = a + 1  // b == 3

a = a + 1 // a ==3
```

## Branches

You can use the *if* and *else* expression. But there is a little differences from JavaScript. 

```javascript
var a = 100
if (a > 0)
	print a            // ! no brackets
	
if (a > 0) { print a } else { a + 100 } // Correct! output: 100
```

This situation is due to the implementation of parser. I just implemented it in a very early stage. There will be a enhancement in the future to improve this situation.

## Loop

You can use *while* expression to loop. The usage of *while* are similar to *if* statement. 

```javascript
var a = 0
while (a <= 50)
{
  print a
  a = a + 1
}
```

The code above will output 0 - 50.

## Function

**2016/8/4 Updates**

Halang supports function now.

```javascript
func hello(a)
{
	if (a > 3)
		print a + 1
	else
		print 0
}

var a = 0
while (a < 30)
{
	hello(a)
	a = a + 1
}
```

It's quite easy to understand.

**# 2016/8/6 Updates** 

HaLang support recursive calling function now.

example:

```javascript
func hello(a)
{
	if (a == 0)
		return 1
	return a*hello(a-1)
}

print hello(5)
```



# Future

In the future, many features will be add to HaLang. Such as

1. Closure
2. break and continue statement
3. Yield
4. ...