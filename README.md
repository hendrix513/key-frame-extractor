# key-frame-extractor
Tool to extract each key frame from a video file, split grayscale version of frame into grid with dimensions specified by user,
and output timestamp of frame and median pixel value for each cell of grid to file

# Requirements:
opencv, 
pkg-config

# Instructions:
to compile executable 'main':

- make

To run executable:

./main [path to video file] [path to output file] [NUM_HORIZONTAL_ZONES] [NUM_VERTICAL_ZONES] [KEY_FRAME_THRESHOLD]

NUM_HORIZONTAL_ZONES specifies number of horizontal zones in the output grid- it must be an integer greater than 0 and less than the number of rows of each frame in input video file

NUM_VERTICAL_ZONES specifies number of vertical zones in the output grid- it must be an integer greater than 0 and less than the number of columns of each frame in input video file

KEY_FRAME_THRESHOLD specifies the difference threshold for determining if a frame is a key frame - the higher this value, the fewer key frames will be found. This value must be a number between 0 and 100 (inclusive). 5 is a recommended value

# Example:

./main ~/Downloads/SampleVideo_1280x720_5mb.mp4 ./key_frames.csv 6 4 5

This will take the file '~/Downloads/SampleVideo_1280x720_2mb.flv' as input and output to './key_frames.csv'. The output grid will have 6 horizontal zones and 4 vertical zones, and keyframes will be determined using a threshold value of 5. This is the content of key_frames.csv after running the above:

8.4,174,178,188,166,172,182,184,179,185,189,189,183,187,187,187,178,126,190,189,179,128,201,195,189
12.96,203,207,206,56,213,218,211,83,227,230,214,222,236,216,171,243,236,186,182,237,138,161,123,126
16.08,174,196,72,52,60,196,91,63,199,199,203,53,195,82,216,80,96,132,223,179,87,138,189,165
21.72,191,197,204,215,216,213,229,230,235,170,186,227,142,177,167,146,69,89,107,103,66,67,66,75
23.6,39,112,96,66,22,99,97,54,22,35,128,93,34,58,96,101,33,34,60,84,32,58,113,121
25.76,178,190,122,131,175,150,87,120,202,104,80,115,144,78,103,78,114,126,70,62,93,90,89,74
27.84,72,88,86,75,81,123,158,61,87,92,159,52,85,86,168,50,80,80,73,57,74,66,65,54

Each row has 25 values separated by commas- the first is the timestamp. The 24 other values represent the median pixel values of the 6x4 grid


# Note:

Algorithm for detecting key frames in video file comes from this project: https://github.com/tafsiri/filmstrip
