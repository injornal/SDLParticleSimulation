build-run:
	g++ src/main.cpp -o play `sdl2-config --cflags --libs` -l SDL2_Image
	./play

clean:
	rm play


.PHONY: clean build-run
