all: domain.o plan.o

domain.o: domain.h domain.cpp
	g++ $(FLAGS) -c -o domain.o domain.cpp

plan.o: domain.h plan.h plan.cpp
	g++ $(FLAGS) -c -o plan.o plan.cpp
