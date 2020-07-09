# Split Buffer Animator

This C program creates animations of the Split Buffer. It is inspired by and adapted from [skeeto/gap-buffer-animator][original_repo]. A split buffer (unofficial name) is a variation of the gap buffer. This [blog post][split_buffer_blog] provides further information about this data structure.

![][gif]

## Installation
To run the commands in the `Makefile`, ensure you have `convert`, `gifsicle`, and `ppmtoy4m` in your machine.

### Ubuntu

Ubuntu should already come with `convert`. Simply run these commands:
```sh
$ sudo apt-get gifsicle
$ sudo apt-get ppmtoy4m
```

### macOS

macOS does not come with a built-in `convert`. The `convert` utility comes with `imagemagick`. Homebrew also cannot resolve the `ppmtoy4m` package.
```sh
$ brew install imagemagick
$ brew install gifsicle
```

Note for macOS: You may need to change the compiler in the `Makefile` from `c99` to something else, such as `gcc`. Currently, there is also a bug where the `.gif` generated in macOS off-by-one for the waiting frames. More specifically, every calls to wait will start one frame earlier, prematurely pausing any series of inserts. Edit: apparently this is a bug with the macOS preview. Uploaded GIFs will be correctly played.

## Usage
The `Makefile` specifies the recipe to create the `.gif` animations.

* Creating the `.gif` file:
```sh
$ make gif
```
* Viewing the `.gif` file without saving them (Not available in macOS):
```sh
$ make view
```
To view the `.gif`, currently there is no choice but to preview them on macOS. To do so, select the `.gif` in finder and press spacebar, or press Command + Y.
* Cleaning up generated files
```sh
$ make clean
```

## Creating your own animations
To create your own animation, you can rewrite the contents of the `intro[]` array in `main` method of `splitbuf-anim.c` with the actions that you want. The `opcode` `enum` specifies what actions can be taken. More examples can be found in `gapbuf.c`, which is the original file that this project is adapted from.

An `intro[]` array is an array of `command`, which has an `opcode` attached to it and an `arg`.

Important: The `num_horiz_chars` variable inside `splitbuf_draw` should be updated to the number maximum total size of the buffer at any point of time of your own text. A helper code in the program has been written that will print this value to the `stdout`.

### Opcodes
This subsection provides some explanation of the Opcodes
* `C_HALT`: The opcode for the command at the end of the animation.
* `C_WAIT`: The opcode to wait. `.arg.v` takes in an `int` which representes the duration of the waiting. The default is 10 = 1 second.
* `C_FORWARD`: The opcode to move forward. `.arg.v` takes in an `int` which represents how many characters forward to move.
* `C_BACKWARD`: The opcode to move backward. `.arg.v` takes in an posstive`int` which represents how many characters backward to move.
* `C_QMOVE`: The opcode to quietly move. The cursor will jump according to the `int` specified in `.arg.v`. Can take in negative values as well.
* `C_INSERT`: The opcode for inserting a character. The `.arg.v` value takes in a `char` to be inserted.
* `C_QINSERT`: The opcode to quietly insert character. The `.arg.v` value takes in a `char` to be inserted.
* `C_STRING`: The opcode to append a string to where the cursor is. The `.arg.s` value takes in the string to be appended.
* `C_QSTRING`: The opcode to quietly append a string to where the cursor is. The `.arg.s` value takes in the string to be appended.
* `C_DELETE`: The opcode to delete the character after the cursor. The `.arg.v` value takes an `int` representing the number of characters to be deleted.
* `C_BACKSPACE`: The opcode to delete the character before the cursor. The `.arg.v` value takes an `int` representing the number of characters to be deleted.

[original_repo]: https://github.com/skeeto/gap-buffer-animator
[split_buffer_blog]: https://www.briansteffens.com/2017/06/19/split-buffers.html
[gif]: ./gifs/intro.gif
