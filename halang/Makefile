

halang: ast.o codegen.o context.o Dict.o GC.o \
		lex.o object.o parser.o ScriptContext.o \
		String.o svm.o function.o
	g++ --std=c++11 -o halang halang.cpp \
		ast.o codegen.o context.o Dict.o GC.o \
		lex.o object.o parser.o ScriptContext.o \
		String.o svm.o function.o

ast.o: ast.cpp
	g++ -Wall -g -c --std=c++11 ast.cpp

codegen.o: codegen.cpp
	g++ -Wall -g -c --std=c++11 codegen.cpp

context.o: context.cpp
	g++ -Wall -g -c --std=c++11 context.cpp

Dict.o: Dict.cpp
	g++ -Wall -g -c --std=c++11 Dict.cpp

function.o: function.cpp
	g++ -Wall -g -c --std=c++11 function.cpp

GC.o: GC.cpp
	g++ -Wall -g -c --std=c++11 GC.cpp

lex.o: lex.cpp
	g++ -Wall -g -c --std=c++11 lex.cpp

object.o: object.cpp
	g++ -Wall -g -c --std=c++11 object.cpp

parser.o: parser.cpp
	g++ -Wall -g -c --std=c++11 parser.cpp

ScriptContext.o: ScriptContext.cpp
	g++ -Wall -g -c --std=c++11 ScriptContext.cpp

String.o: String.cpp
	g++ -Wall -g -c --std=c++11 String.cpp

svm.o: svm.cpp
	g++ -Wall -g -c --std=c++11 svm.cpp

clean:
	rm ./*.o;
	rm halang;