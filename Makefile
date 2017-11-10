all:	
	gcc problema1.c -l pthread -o primos
	gcc problema2.c -l pthread -o primosn

clean:	
	rm -rf *o primos
	rm -rf *o primosn
