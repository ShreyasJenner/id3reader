all:
#	@gcc -o syncint src/syncint.c
	@gcc -o bin/id3reader src/{id3reader.c,syncint.c,jpeg_writer.c,tag_reader.c,frame_reader.c,free.c}
