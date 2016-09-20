# Ha Language 哈语言
A toy language implementation. Including lexer, parser, code generation and interpreter.



# Usage

By default, the program read the file named "source.txt" in the same location with the program, but you can pass the filename as the parameter to the program.



# Language

This language is similar to JavaScript, but it has differences because this project is not completely finished.



**Typed Halang**

Sinse [#bc39771](https://github.com/ChannelOne/halang/commit/bc39771a55f74c3a23778337b53875158d50cf9b) commit, Halang is building a type system. Now all the function and variable has it's own type.

## String

In HaLang, string is a immutable built-in object. It's described within *""*.

```javascript
var a = "Hello World!"; // define a variable name "a", and it's type is string.
var b : string; // define a string variable named "a"
```



## Expression

Similar to JavaScript.

```
2 * (3 + 4) // 14
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
	a = -a
	
if (a > 0) { a = -a } else { a + 100 } // Correct! output: 100
```

This situation is due to the implementation of parser. I just implemented it in a very early stage. There will be a enhancement in the future to improve this situation.

## Loop

You can use *while* expression to loop. The usage of *while* are similar to *if* statement. 

```javascript
var a = 0
while (a <= 50)
{
  a = a + 1
}
```

The code above will output 0 - 50.

## Function

**# 2016/8/4 Updates**

Halang supports function now.

```javascript
func hello(a : int) : int // define a function return int
{
	if (a > 3)
		return a + 1
	else
		return a
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
func hello(a : int) : int
{
	if (a == 0)
		return 1
	return a*hello(a-1)
}
```

And also, Halang supports closure.

```javascript
func hello(a : int) : int
{
	func ho(b)
	{
		a = a + b
		return a;
	}
	return ho;
}

var a = hello(5);	// type int
a(1);			// 6
a(1);			// 7
a(8);			// 15
```



# Future

In the future, many features will be add to HaLang. Such as

1. Closure
2. break and continue statement
3. Yield
4. ...