all: schedule

schedule: main.o rate.o edf_llf.o
	g++ main.o rate.o edf_llf.o -o schedule	

main.o: main.cpp rate.hpp edf_llf.hpp structDef.hpp
	g++ -c main.cpp

rate.o: rate.cpp rate.hpp structDef.hpp
	g++ -c rate.cpp

edf_llf.o: edf_llf.cpp edf_llf.hpp structDef.hpp
	g++ -c edf_llf.cpp

clean: 
	rm -f *.o
