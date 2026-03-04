all: schedule

schedule: main.o rate.o edf_lst.o
	g++ main.o rate.o edf_lst.o -o schedule	

main.o: main.cpp rate.hpp edf_lst.hpp structDef.hpp
	g++ -c main.cpp

rate.o: rate.cpp rate.hpp structDef.hpp
	g++ -c rate.cpp

edf_lst.o: edf_lst.cpp edf_lst.hpp structDef.hpp
	g++ -c edf_lst.cpp

clean: 
	rm -f *.o
