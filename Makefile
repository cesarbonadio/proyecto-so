all:	
	gcc problema1.c -l pthread -o primos
	gcc problema2.c -l pthread -o primosn
	gcc problema3.c -o primosm

clean:	
	rm -rf *o primos
	rm -rf *o primosn
	rm -rf *o primosm
