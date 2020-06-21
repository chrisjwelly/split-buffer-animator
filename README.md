# Split Buffer Animator

This C program creates animations of the Split Buffer. It is inspired by and adapted from [skeeto/gap-buffer-animator][original_repo]. A split buffer (unofficial name) is a variation of the gap buffer. This [blog post][split_buffer_blog] provides further information about this data structure.

![][gif]

## Installation
To run the commands in the `Makefile`, ensure you have `convert`, `gifsicle`, and `ppmtoy4m` in your machine.

Ubuntu:

Ubuntu should already come with `convert`. Simply run these commands:
```sh
$ sudo apt-get gifsicle
$ sudo apt-get ppmtoy4m
```

macOS:

macOS does not come with a built-in `convert`. The `convert` utility comes with `imagemagick` .
```sh
$ brew install imagemagick
$ brew install gifsicle
$ brew install ppmtoy4m
```

Note for macOS: You may need to change the compiler in the `Makefile` from `c99` to something else, such as `gcc`. Currently, there is also a bug where the `.gif` generated in macOS off-by-one for the waiting frames. More specifically, every calls to wait will start one frame earlier, prematurely pausing any series of inserts.

## Usage
The `Makefile` specifies the recipe to create the `.gif` animations.

* Creating the `.gif` file:
```sh
$ make gif
```
* Viewing the `.gif` file (without saving them):
```sh
$ make view
```
* Cleaning up generated files
```sh
$ make clean
```

## Creating your own animations
To create your own animation, you can rewrite the contents of the `intro[]` array in `main` method of `splitbuf-anim.c` with the actions that you want. The `opcode` `enum` specifies what actions can be taken. More examples can be found in `gapbuf.c`, which is the original file that this project is adapted from.

Important: The `num_horiz_chars` variable inside `splitbuf_draw` should be updated to the number maximum total size of the buffer at any point of time of your own text. A helper code in the program has been written that will print this value to the `stdout`.

TODO: Update this documentation to explain more about the different `opcode`s.

[original_repo]: https://github.com/skeeto/gap-buffer-animator
[split_buffer_blog]: https://www.briansteffens.com/2017/06/19/split-buffers.html
[gif]: ./gifs/intro.gif
