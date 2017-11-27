CC=g++
CPPVER=--std=c++14
CFLAGS=-Wall -g -c --std=c++14

halang: token.o ast.o codegen.o context.o Dict.o GC.o \
		lex.o object.o parser.o ScriptContext.o \
		String.o svm.o function.o StringBuffer.o
	$(CC) $(CPPVER) -o halang halang.cpp \
		ast.o codegen.o context.o Dict.o GC.o \
		lex.o object.o parser.o ScriptContext.o \
		String.o svm.o function.o StringBuffer.o

test: testlex testparser
	./testlex;
	./testparser

testlex: token.o StringBuffer.o lex.o testlex.cpp
	$(CC) $(CPPVER) -o testlex testlex.cpp \
		token.o StringBuffer.o lex.o

testparser: testlex ast.o parser.o testparser.cpp \
	ASTVisitor.o
	$(CC) $(CPPVER) -o testparser testparser.cpp \
		token.o StringBuffer.o lex.o \
		ast.o parser.o ASTVisitor.o

ASTVisitor.o: ast.o ASTVisitor.cpp
	$(CC) $(CFLAGS) ASTVisitor.cpp

token.o: token.h token.cpp
	$(CC) $(CFLAGS) token.cpp

StringBuffer.o: StringBuffer.cpp StringBuffer.h
	$(CC) $(CFLAGS) StringBuffer.cpp

ast.o: ast.h ast.cpp
	$(CC) $(CFLAGS) ast.cpp

codegen.o: codegen.h codegen.cpp
	$(CC) $(CFLAGS) codegen.cpp

context.o: context.h context.cpp
	$(CC) $(CFLAGS) context.cpp

Dict.o: Dict.h Dict.cpp
	$(CC) $(CFLAGS) Dict.cpp

function.o: function.h function.cpp
	$(CC) $(CFLAGS) function.cpp

GC.o: GC.h GC.cpp
	$(CC) $(CFLAGS) GC.cpp

lex.o: lex.h lex.cpp
	$(CC) $(CFLAGS) lex.cpp

object.o: object.h object.cpp
	$(CC) $(CFLAGS) object.cpp

parser.o: parser.h parser.cpp
	$(CC) $(CFLAGS) parser.cpp

ScriptContext.o: ScriptContext.h ScriptContext.cpp
	$(CC) $(CFLAGS) ScriptContext.cpp

String.o: String.h String.cpp
	$(CC) $(CFLAGS) String.cpp

svm.o: svm.h svm.cpp
	$(CC) $(CFLAGS) svm.cpp

clean:
	rm ./*.o;
	rm halang;
	rm testlex;
	rm testparser
