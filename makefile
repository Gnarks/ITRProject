main : main.c dependencies/assembly.h
	cc -o main main.c

clean : 
	rm main
