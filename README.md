# Ha Language 哈语言
A toy language implementation. Including lexer, parser, code generation and interpreter.



# Usage

By default, the program read the file named "source.txt" in the same location with the program, but you can pass the filename as the parameter to the program.



# Language

This language is similar to javascript, but it has differences because this project is not completely finished.

## Expression

A single line of expression will cause a ouput.

```
2 * (3 + 4) // output: 14
```

## Assignment

All the assignment should use *let* expression.

```
let a = 1 + 1  // a == 2
let b = a + 1  // b == 3
```

## Branches

You can use the *if* and *else* expression. But there is a little differences from Javascript. You must add the brackets after the *if* expression whether there is only one expression.

```
let a = 100
if (a > 0)
	a            // Error! no brackets
	
if (a > 0) { a } else { a + 100 } // Correct! output: 100
```

This situation is due to the implementation of parser. I just implemented it in a very early stage. There will be a enhancement in the future to improve this situation.

## Loop

You can use *while* expression to loop. The usage of *while* are similar to *if* statement. You **must** add the brackets.

```
let a = 0
while (a <= 50)
{
  a
  let a = a + 1
}
```

The code above will output 0 - 50.

# Future

In the future, many features will be add to HaLang. Such as

1. Functions
2. String
3. Class
4. Yield
5. ...