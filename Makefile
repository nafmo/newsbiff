newsbiff: main.o news.o tcp.o
	g++ -o newsbiff main.o news.o tcp.o -lg++

main.o: main.cc news.h
	g++ -c main.cc

news.o: news.cc news.h tcp.h
	g++ -c news.cc

tcp.o: tcp.cc tcp.h
	g++ -c tcp.cc
