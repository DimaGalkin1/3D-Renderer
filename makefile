make:
	g++ main.cpp engine/camera.cpp engine/objects.cpp engine/rendering.cpp engine/utils.cpp -o 3DL -lSDL2main -lSDL2 -lm -O3 -ffast-math -lSDL2_image
	./3DL