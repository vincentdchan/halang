func fib(n) {
	if (n == 1 || n == 2)
		return 1
	return fib(n-1) + fib(n-2)
}

print("fib(5):")
print(fib(5))
print("fib(10):")
print(fib(10))
print("fib(15):")
print(fib(15))
print("fib(20):")
print(fib(20))
print("fib(25):")
print(fib(25))
print("fib(30):")
print(fib(30))
