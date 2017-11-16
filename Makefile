all:	
	gcc problema1.c -l pthread -o primos
	gcc problema2.c -l pthread -o primosn
	gcc problema3.c -o primoscatch
	g++ problema4.cpp -l pthread -o semaforo

clean:	
	rm -rf *o primos
	rm -rf *o primosn
	rm -rf *o primoscatch
	rm -rf *o semaforo
