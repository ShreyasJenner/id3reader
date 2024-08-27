all:
#	@gcc -o syncint src/syncint.c
	@gcc -o bin/id3reader src/id3reader.c src/syncint.c src/jpeg_writer.c -lncurses
