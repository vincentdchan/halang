func closureAddThree(a) {
	func closureAddTwo(b) {
		func closureAddOne(c) {
			return a + b + c;
		}
		return closureAddOne
	}
	return closureAddTwo
}

var twoFun = closureAddThree(1)
var oneFun = twoFun(2)
print(oneFun(3)) // 6
print(oneFun(4)) // 7
print(oneFun(5)) // 8

