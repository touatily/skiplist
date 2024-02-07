

#Header include directories



main:  main.cpp src/skiplist.hpp src/slnode.hpp
	g++ -o main main.cpp

clean:
	rm -f main


