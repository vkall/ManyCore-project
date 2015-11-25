# ManyCore-project
ManyCore programming course project.

Visually compares images in the directory and returns
the images that are closest to the reference image.

Compiled using g++: 
```
g++ pic_compare.cpp pgetopt.cpp -lpuzzle -I ./libpuzzle/ -o pic_compare
```

Then run in command line: 
```
./pic_compare referencefile.jpg ./picturefolder/
```
You can use the -o flag to write the output to a text file.
