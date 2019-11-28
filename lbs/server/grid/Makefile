INCLUDE = -I../../ \

CPPFLAGS = -Wall -fPIC $(INCLUDE) $(DEBUG_DEF) -std=c++11 -g
LIBS = libgrid.a

all : clean $(LIBS)

clean:
	rm -rf *.o *.a

libgrid.a: hashtable.o lbs_grid.o lbs_index.o bitmaps.o nnheap.o
	ar cr $@ $^

.o : .cpp
	g++ $(CPPFLAGS) -c $^
