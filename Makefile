CC = g++
CFLAGS = -Wall -O3
LDFLAGS = -flto
LIBS = -lm -lhwloc -lboost_graph
SRC = ./src/
#BUILD = ./build/
INCLUDE = ./include/
OBJS = init_graph.o output_graph.o main.o
HEADER = init_graph.h output_graph.h



all: ${OBJS}
	${CC} ${CFLAGS} ${LDFLAGS} ${LIBS} ${OBJS} ${INCLUDE}output_graph.h ${INCLUDE}init_graph.h -o hwloc-test

main.o: ${SRC}main.cc
	${CC} ${CFLAGS} -c $<

init_graph.o: ${SRC}init_graph.cc
	${CC} ${CFLAGS} -c $<

output_graph.o: ${SRC}output_graph.cc
	${CC} ${CFLAGS} -c $<

clean:
	-rm -f *.o 
