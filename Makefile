all: libalarm.so alarmManager modul deleter filter

libalarm.so: libalarm.o
	gcc -shared -o libalarm.so libalarm.o

libalarm.o: src/libalarm.c
	gcc -fPIC -c src/libalarm.c

alarmManager: alarmManager.o
	gcc alarmManager.o -o alarmManager -L. -lalarm -lzmq

alarmManager.o: src/alarmManager.c
	gcc -c src/alarmManager.c

modul: modul.o
	gcc modul.o -o modul -L. -lalarm -lzmq

modul.o: src/modul.c
	gcc -c src/modul.c

filter: filter.o
	gcc filter.o -o filter -L. -lalarm -lzmq

filter.o: src/filter.c
	gcc -c src/filter.c

deleter: deleter.o
	gcc deleter.o -o deleter -L. -lalarm -lzmq

deleter.o: src/deleter.c
	gcc -c src/deleter.c

clean:
	rm -f *.o
