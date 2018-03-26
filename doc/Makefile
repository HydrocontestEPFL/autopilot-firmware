DOTSRC=$(wildcard *.dot)
GRAPHS=$(DOTSRC:.dot=.png)

all: $(GRAPHS)

%.png: %.dot
	dot -Tpng -o $@ $<

