PROJECT_NAME = img2text

main: src/main.c stb/stb_image.h stb/stb_image_write.h
	gcc src/main.c -g -o build/$(PROJECT_NAME)

run: main
	./build/$(PROJECT_NAME)

fast: src/main.c stb/stb_image.h stb/stb_image_write.h 
	gcc src/main.c -O3 -o build/$(PROJECT_NAME)
