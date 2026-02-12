#img2text
##Usage

```console
img2ascii: <flags> <input_path>
	-h number	desired height of the image in characters (default 40)
	-w number	desired width  of the image in characters (if not provided it will be calculated from the original image ratio)
	-o path		output path
	-d		enable debug information (emiting intermidiate files, debug messages)
	-s		print the output to standard output
```

##Compiling

to compile this project you will need a c compiler (gcc) and gnumake

>[!Note]
>i bet you can get it running with other compilers as well

first, get the stb submodule 
```sh
git submodule init
git submodule update
```

next, compile the project :)
```sh
#will compile without optimizations and with debug information
make

#will compile with maximal level of optimizatoins
make fast

```
the result will wait for you in ./build/img2text

##Some notes

- this program is not super optimized and i reckognize that the order in which
the passes are done could be optimized... this program is mostly for educational purposes

- some features are planned for the future but no promises ! :)
