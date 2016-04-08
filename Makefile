output: 
	gcc -g lab0.c -o lab0

check:
	echo 123456 >test1.txt
	./lab0 --input=test1.txt
	./lab0 --output=test2.txt
	./lab0 --input=test1.txt --output=test2.txt
	diff test1.txt test2.txt
	./lab0 --catch
	./lab0 --input=test1.txt --output=test2.txt  --segfault --catch

dist:
	tar -cvzf lab0-604582162.tar.gz lab0.c Makefile README screen5.jpg screen6.jpg

clean:
	rm *.o *.txt lab0

#clobber: tar -xvzf

