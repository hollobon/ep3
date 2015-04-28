ep3: ep3main.o ep3compile.o ep3symtab.o ep3.tab.o lex.yy.o 
	gcc -o ep3 ep3main.o ep3compile.o ep3symtab.o ep3.tab.o lex.yy.o

ep3main.o: ep3main.c ep3.h
	gcc -c ep3main.c
    
ep3compile.o: ep3compile.c ep3.h
	gcc -c ep3compile.c
		
ep3symtab.o: ep3symtab.c ep3.h
	gcc -c ep3symtab.c
    
ep3.tab.o: ep3.tab.c
	gcc -c ep3.tab.c

ep3.tab.c: ep3.y ep3.h
	bison -d ep3.y

lex.yy.o: lex.yy.c
	gcc -c lex.yy.c

lex.yy.c: ep3.l ep3.h
	flex ep3.l

clean:
	rm *.o *~	
