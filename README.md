# key-frame-extractor
Tool to extract each key frame from a video file, split grayscale version of frame into grid with dimensions specified by user,
and output median pixel value for each cell of grid to file

# Requirements:
opencv
pkg-config

# Instructions:
to compile executable 'main':

- g++ $(pkg-config --cflags --libs opencv4) -std=c++11 main.cpp -o main

To run executable:

./main [path to video file] [path to output file] [NUM_HORIZONTAL_ZONES] [NUM_VERTICAL_ZONES] [KEY_FRAME_THRESHOLD]

NUM_HORIZONTAL_ZONES specifies number of horizontal zones in the output grid- it must be an integer greater than 0 and less than the number of rows of each frame in input video file

NUM_VERTICAL_ZONES specifies number of vertical zones in the output grid- it must be an integer greater than 0 and less than the number of columns of each frame in input video file

KEY_FRAME_THRESHOLD specifies the difference threshold for determining if a frame is a key frame - the higher this value, the fewer key frames will be found. This value must be a number between 0 and 100 (inclusive). 5 is a recommended value

# Note:

Algorithm for detecting key frames in video file comes from this project: https://github.com/tafsiri/filmstrip
