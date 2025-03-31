main : main.c dependencies/assembly.h
	cc -o itrmain main.c dependencies/assembly.c

clean : 
	rm itrmain
