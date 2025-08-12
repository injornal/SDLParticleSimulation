Q = @


all: clean bin/play
	$(Q)echo 'Building...'

run: all
	./bin/play

clean:
	$(Q)echo "Cleanining...."
	$(Q)rm -rf ./bin/*
	$(Q)echo "Done"


bin/play:
	$(Q)echo "g++ src/main.cpp -o ./bin/play `sdl2-config --cflags --libs` -l SDL2_Image"
	$(Q)g++ src/main.cpp -o ./bin/play `sdl2-config --cflags --libs` -l SDL2_Image


.PHONY: clean run all
