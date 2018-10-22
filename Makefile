all: create montador ligador

create: 
	mkdir -p obj/

montador:  auxiliar.o Directive.o Instruction.o Line.o
	g++ -std=c++11 -I include/ -c src/montador.cpp -o obj/montador.o
	g++ -o montador obj/montador.o obj/auxiliar.o obj/Directive.o obj/Instruction.o obj/Line.o

ligador: auxiliar.o
	g++ -std=c++11 -I include/ -c src/ligador.cpp -o obj/ligador.o
	g++ -o ligador obj/ligador.o obj/auxiliar.o

auxiliar.o:
	g++ -std=c++11 -I include/ -c src/auxiliar.cpp -o obj/auxiliar.o

Directive.o:
	g++ -std=c++11 -I include/ -c src/Directive.cpp -o obj/Directive.o

Instruction.o:
	g++ -std=c++11 -I include/ -c src/Instruction.cpp -o obj/Instruction.o

Line.o:
	g++ -std=c++11 -I include/ -c src/Line.cpp -o obj/Line.o

clean:
	rm -f obj/*.o *.pre *.obj *.e montador ligador