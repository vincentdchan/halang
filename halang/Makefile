CC=g++
CFLAGS=-Wall -g -c --std=c++14

halang: token.o ast.o codegen.o context.o Dict.o GC.o \
		lex.o object.o parser.o ScriptContext.o \
		String.o svm.o function.o StringBuffer.o
	$(CC) --std=c++11 -o halang halang.cpp \
		ast.o codegen.o context.o Dict.o GC.o \
		lex.o object.o parser.o ScriptContext.o \
		String.o svm.o function.o StringBuffer.o

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