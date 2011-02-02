all: sdp

sdp: main.o sdp.o l2cap.o
	gcc main.o sdp.o l2cap.o -o sdp-test -lbluetooth

main.o: main.c
	gcc -c main.c -lbluetooth

sdp.o: sdp.c
	gcc -c sdp.c -lbluetooth
	
l2cap.o: l2cap.c
	gcc -c l2cap.c -lbluetooth	

clean:
	rm -rf *.o sdp-test
