calc: calc.o scanner.o token.o ast.o parser.o calculator.o
	g++ -g -o calc calc.o scanner.o token.o ast.o parser.o calculator.o
calc.o: calc.C scanner.h token.h
	g++ -g -c calc.C
calculator.o: calculator.C calculator.h parser.h ast.h
	g++ -g -c calculator.C
scanner.o: scanner.C scanner.h token.h
	g++ -g -c scanner.C
token.o: token.C token.h
	g++ -g -c token.C
ast.o: ast.C ast.h
	g++ -g -c ast.C
parser.o: parser.C parser.h
	g++ -g -c parser.C
clean:
	rm -f *.o
	rm -f calc
	rm -f *~
	rm -f *.*~
	rm -f a.ewe
