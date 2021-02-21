.PHONY: all build run-ami ami run-evaluation

# Default haarcascades installation path, provide a custom value with OPENCV_SAMPLES_DATA_PATH=<custom_path> if different
# Example usage: `make run-ami OPENCV_SAMPLES_DATA_PATH=/path_to_opencv/data/haarcascades`
export OPENCV_SAMPLES_DATA_PATH=${HOME}/opencv/data/haarcascades/

all:
	@make ami
	@make build

ami:
	@echo "Downloading the Mathematical Analysis of Images (AMI) Ear Database\n"
	rm -rf datasets/AMI/*
	mkdir -p datasets/AMI/

	@curl -s https://ctim.ulpgc.es/research_works/ami_ear_database/subset-1.zip -o datasets/AMI/subset-1.zip
	@unzip -qq datasets/AMI/subset-1.zip -d datasets/AMI/
	@mv datasets/AMI/subset-1/* datasets/AMI/
	@rm -rf datasets/AMI/subset-1/ datasets/AMI/subset-1.zip

	@echo "\nPart 1/4 downloaded ... "

	@curl -s https://ctim.ulpgc.es/research_works/ami_ear_database/subset-2.zip -o datasets/AMI/subset-2.zip
	@unzip -qq datasets/AMI/subset-2.zip -d datasets/AMI/
	@mv datasets/AMI/subset-2/* datasets/AMI/
	@rm -rf datasets/AMI/subset-2/ datasets/AMI/subset-2.zip

	@echo "Part 2/4 downloaded ... "

	@curl -s https://ctim.ulpgc.es/research_works/ami_ear_database/subset-3.zip -o datasets/AMI/subset-3.zip
	@unzip -qq datasets/AMI/subset-3.zip -d datasets/AMI/
	@mv datasets/AMI/subset-3/* datasets/AMI/
	@rm -rf datasets/AMI/subset-3/ datasets/AMI/subset-3.zip

	@echo "Part 3/4 downloaded ... "

	@curl -s https://ctim.ulpgc.es/research_works/ami_ear_database/subset-4.zip -o datasets/AMI/subset-4.zip
	@unzip -qq datasets/AMI/subset-4.zip -d datasets/AMI/
	@mv datasets/AMI/subset-4/* datasets/AMI/
	@rm -rf datasets/AMI/subset-4/ datasets/AMI/subset-4.zip

	@echo "AMI dataset successfully downloaded, you may find it in the datasets/ directory.\n"

build:
	rm -rf build/* GalleryGenerator
	cmake -S src -B build
	cd build && $(MAKE)
	@ln -s build/GalleryGenerator . && echo "\nA simbolic link to GalleryGenerator has been created in the project directory."

run-ami:
	@make build > /dev/null
	@./build/GalleryGenerator datasets/AMI/

run-evaluation:
	@make build > /dev/null
	@./build/GalleryGenerator datasets/AMI/ "evaluation"
