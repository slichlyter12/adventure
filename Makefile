adventure: lichlyts.adventure.c
	gcc lichlyts.adventure.c -o lichlyts.adventure
debug: lichlyts.adventure.c
	gcc -g lichlyts.adventure.c -o lichlyts.adventure.d
	gdb lichlyts.adventure.d
clean: 
	rm lichlyts.adventure
	rm -rf lichlyts.rooms.*
	rm -rf *.dSYM
	rm lichlyts.adventure.d
