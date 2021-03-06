.POSIX:
.SUFFIXES: .ppm .gif
# In Mac OS, might have to change c99 to gcc
CC     = c99
CFLAGS = -Wall -Wextra -O3 -g3
LDLIBS = -lm

splitbuf-anim: splitbuf-anim.c splitbuf.c array.c

intro.ppm multicursors.ppm macros.ppm illusion.ppm: splitbuf-anim
	./splitbuf-anim

view: intro.ppm multicursors.ppm macros.ppm illusion.ppm
	ppmtoy4m -F 10:1 < intro.ppm        | mpv --really-quiet -

gif: intro.gif

clean:
	rm -f splitbuf-anim
	rm -f intro.ppm
	rm -f intro.gif

.ppm.gif:
	convert ppm:- gif:- < $< | gifsicle -O3 -d10 --loop > $@
