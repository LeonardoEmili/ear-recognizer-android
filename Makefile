.PHONY: all

all:
	rm -rf build/*
	rm -f GalleryGenerator
	cmake -S src -B build
	cd build && $(MAKE)
	ln -s build/GalleryGenerator .