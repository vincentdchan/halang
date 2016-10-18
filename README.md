# Ha Language 哈语言
A toy dynamic language implementation.

- ✔ Lexer
- ✔ Parser
- ✔ Code Generation
- ✔ Stack VM
- ✔ Simple mark-sweep GC
- ✔ Function and Closure
- ◦ Class
- ◦ Yield
- ◦ ...



# Usage

By default, the program read the file named "source.txt" in the same location with the program, but you can pass the filename as the parameter to the program.



# Language

This language is similar to JavaScript, but it has differences because this project is not completely finished.

## String

In HaLang, string is a immutable built-in object. It's described within *""*.

```javascript
var a = "Hello World!" // define a variable name "a", and it's value is "Hello World!"
print(a)

// concat
var b = "Hello " + "world!"	// result: Hello world!
// whilch is equal to "Hello".__add__("world")
print(a)
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
	
if (a > 0) { print(-a) } else { print(a + 100) } // Correct! output: -100
```

This situation is due to the implementation of parser. I just implemented it in a very early stage. There will be a enhancement in the future to improve this situation.

## Loop

You can use *while* expression to loop. The usage of *while* are similar to *if* statement. 

```javascript
var a = 0
while (a <= 50)
{
  a = a + 1
  print(a)
}

// for statement
for (var i = 0; i < 10; i++) {
  print(i);
}
// which is equal to
function _tmp() {
  	var i = 0;
  	while(i < 10) {
  		print(i);
    	i++;
	}
}
_tmp();
```

## Function

**# 2016/8/4 Updates**

Halang supports function now.

```javascript
func hello(a)
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
func hello(a)
{
	if (a == 0)
		return 1
	return a*hello(a-1)
}
```

And also, Halang supports closure.

```javascript
func hello(a)
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

## Closure

```javascript
func A(a) {
	func B(b) {
    	func C(c) {
          	return a + b + c;
		}
      	return C;
	}
  	return B;
}
var funcB = A("a");
var funcC = funB("b");
print(funC("c"));
```



# Object-Originted Programming

In `Halang`, you can use `prototype` to implement OOP, the type of prototype is usually a Dict, Dict is and internal object.

Let's assume `a` is an integer, `Halang` has included the `prototype` of Integer.

```
a + 1
```
The expression below is equal to:
```
a.__add__(1)
```
Because the `prototype` of integer includes `__add__` method, so expression below can calculate the sum of a and 1. And you can get the `prototype` of integer and modify it, so that you can **override** the default `__add__`.

# Future

In the future, many features will be add to HaLang. Such as

1. break and continue statement
2. Class support
3. Yield
4. binding to C lib
5. ...
