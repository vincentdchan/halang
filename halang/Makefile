CC=g++
CFLAGS=-Wall -g -c --std=c++11

halang: ast.o codegen.o context.o Dict.o GC.o \
		lex.o object.o parser.o ScriptContext.o \
		String.o svm.o function.o
	mkdir build;
	$(CC) --std=c++11 -o halang halang.cpp \
		ast.o codegen.o context.o Dict.o GC.o \
		lex.o object.o parser.o ScriptContext.o \
		String.o svm.o function.o

ast.o: ast.cpp
	$(CC) $(CFLAGS) ast.cpp

codegen.o: codegen.cpp
	$(CC) $(CFLAGS) codegen.cpp

context.o: context.cpp
	$(CC) $(CFLAGS) context.cpp

Dict.o: Dict.cpp
	$(CC) $(CFLAGS) Dict.cpp

function.o: function.cpp
	$(CC) $(CFLAGS) function.cpp

GC.o: GC.cpp
	$(CC) $(CFLAGS) GC.cpp

lex.o: lex.cpp
	$(CC) $(CFLAGS) lex.cpp

object.o: object.cpp
	$(CC) $(CFLAGS) object.cpp

parser.o: parser.cpp
	$(CC) $(CFLAGS) parser.cpp

ScriptContext.o: ScriptContext.cpp
	$(CC) $(CFLAGS) ScriptContext.cpp

String.o: String.cpp
	$(CC) $(CFLAGS) String.cpp

svm.o: svm.cpp
	$(CC) $(CFLAGS) svm.cpp

clean:
	rm ./*.o;
	rm halang;