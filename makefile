main : main.c dependencies/assembly.h
	cc -o main main.c dependencies/assembly.c

clean : 
	rm main
