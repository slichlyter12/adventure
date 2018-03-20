adventure: lichlyts.adventure.c
	gcc lichlyts.adventure.c -o lichlyts.adventure
debug: lichlyts.adventure.c
	gcc -g lichlyts.adventure.c -o lichlyts.adventure.d
	gdb lichlyts.adventure.d
clean: 
	ls | grep -P "^lichlyts\.rooms\.([0-9]+|d)" | xargs -d"\n" rm -rf
	rm lichlyts.adventure
