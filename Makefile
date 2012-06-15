CUDD_HOME=/home/dmorwood/code/cudd-2.4.2
FLAGS    =-g
INCLUDES =-I$(CUDD_HOME)/include/
LIBDIR   =-L$(CUDD_HOME)/cudd/ -L$(CUDD_HOME)/util/ -L$(CUDD_HOME)/mtr -L$(CUDD_HOME)/st -L$(CUDD_HOME)/epd -L$(CUDD_HOME)/obj
LIBS     =-lobj -lcudd -lutil -lmtr -lst -lepd 

all: domain.o plan.o matrix.o InteriorPoint.o matrixTest IPTest

domain.o: domain.h domain.cpp
	g++ $(FLAGS) -c -o domain.o domain.cpp

plan.o: domain.h plan.h plan.cpp
	g++ $(FLAGS) $(INCLUDES) -c -o plan.o plan.cpp

matrix.o: matrix.h matrix.cpp
	g++ $(FLAGS) $(INCLUDES) -c -o matrix.o matrix.cpp

InteriorPoint.o: InteriorPoint.h InteriorPoint.cpp
	g++ $(FLAGS) $(INCLUDES) -c -o InteriorPoint.o InteriorPoint.cpp

matrixTest: tests/matrixTest.cpp matrix.o
	g++ $(FLAGS) $(INCLUDES) -o matrixTest tests/matrixTest.cpp matrix.o $(LIBDIR) $(LIBS)

IPTest: tests/InteriorPointTest.cpp InteriorPoint.o matrix.o plan.o domain.o
	g++ $(FLAGS) $(INCLUDES) -o IPTest tests/InteriorPointTest.cpp InteriorPoint.o matrix.o plan.o domain.o $(LIBDIR) $(LIBS)

clean:
	rm *.o
