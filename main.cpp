#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;
using namespace std;

#define RESIZE_FACTOR          .25
#define GAUSSIAN_BLUR_PARAM    9

int select_val(vector <int> v, int idx){
    vector <int> lows, highs;
    int pivot = v[idx];
    int num_pivots = 0, num_lows, total_lows_pivots;

    if (v.size() == 1)
        return v[0];

    for (auto num: v){
       if (num == pivot)
           num_pivots++;
       else
           (num < pivot ? lows : highs).push_back(num);
    }

    num_lows = lows.size();
    total_lows_pivots = num_lows + num_pivots;

    if (idx < num_lows)
        return select_val(lows, idx);
    else if (idx < total_lows_pivots)
        return pivot;
    else
        return select_val(highs, idx-total_lows_pivots);
}

double get_median_val(vector <int> v){
    sort(v.begin(), v.end());
    int size = v.size();
    int mid_idx = size/2;
    int mid_val = select_val(v, mid_idx);

    return size % 2 ? mid_val : (mid_val+ select_val(v, mid_idx-1))/2.0;
}

int main(int argc, char *argv[]) {
    // handle arguments
    assert (argc == 6);
    char *input_filename = argv[1];
    char *output_filename = argv[2];
    int num_horizontal_zones = atoi(argv[3]);
    int num_vertical_zones = atoi(argv[4]);
    double key_frame_threshold = atof(argv[5]);

    assert(num_horizontal_zones > 0);
    assert(num_vertical_zones > 0);
    assert(key_frame_threshold >= 0);
    assert(key_frame_threshold <= 100);

    ofstream fs;

    remove(output_filename);
    fs.open(output_filename);

    // initialize video capture
    VideoCapture cap(input_filename);
    double time_per_frame = 1 / cap.get(CAP_PROP_FPS);

    if(!cap.isOpened()){
        cout << "Error opening video stream or file" << endl;
        return -1;}

    double time = 0;
    double total_diff = 0;

    int size, mid_idx, mid_val, ii, jj, r;

    MatIterator_<uchar> it;

    Size resize_size = Size();
    Size gaussian_blur_size = Size(GAUSSIAN_BLUR_PARAM, GAUSSIAN_BLUR_PARAM);

    Mat frame, last_gray_frame;

    cap >> frame;
    int key_frame_num_pixels = .01 * key_frame_threshold * frame.rows * frame.cols;

    // sanity check arguments

    if (num_horizontal_zones > frame.rows){
        cout << "Error: more horizontal zones than rows" << endl;
        return -1;}

    if (num_vertical_zones > frame.cols){
        cout << "Error: more vertical zones than columns" << endl;
        return -1;}

    // iterate through frames
    for(double tm = 0; !frame.empty(); tm += time_per_frame){
        Mat gray_frame, gray_frame_scaled, gray_frame_blurred, gray_frame_diff;

        //convert to grayscale
        cvtColor(frame, gray_frame, COLOR_RGB2GRAY);
        assert(gray_frame.isContinuous());

        /* resize and apply blur before comparing to previous frame, to which the same
         steps have been applied
         */
        resize(gray_frame, gray_frame_scaled, resize_size, RESIZE_FACTOR, RESIZE_FACTOR, INTER_NEAREST);
        GaussianBlur(gray_frame_scaled, gray_frame_blurred, gaussian_blur_size, 0);

        if (!last_gray_frame.empty()){
            absdiff(gray_frame_blurred, last_gray_frame, gray_frame_diff);

            /* if the number of non-zero pixels in the diff is above the specified threshold,
               then frame is a keyframe
             */
            if (countNonZero(gray_frame_diff) > key_frame_num_pixels){
                vector<vector <vector<int>>> groups(num_horizontal_zones,
                 vector <vector<int>>(num_vertical_zones, vector<int>()));

                /* iterate through pixels of gray frame, appending each value
                   to the vector of its corresponding 'zone'
                 */

                #pragma omp parallel for
                for (ii=0; ii<gray_frame.rows; ii++){
                    it = gray_frame.row(ii).begin<uchar>();
                    r = (ii*num_horizontal_zones)/gray_frame.rows;

                    for (jj=0; jj<gray_frame.cols; jj++, it++)
                        groups[r][(jj*num_vertical_zones)/gray_frame.cols].push_back((int)*it);
                }

                /* iterate through each vector of pixels, find median value for
                   each and append to vector for output to file. Each vector starts
                   with the timestamp of the frame
                 */
                vector<double> medians {tm};
                for (auto group_row: groups){
                    #pragma omp parallel for schedule(static)
                    for (auto v: group_row)
                        medians.push_back(get_median_val(v));
                }

                /* write timestamp/median value vector to output file, with
                   each value separated by comma
                 */
                for (ii = 0; ii < medians.size()-1; ii++)
                    fs << medians[ii] << ',';
                fs << medians[ii] << endl;
            }
        }

        last_gray_frame = gray_frame_blurred;
        cap >> frame;
    }

    fs.close();
    return 0;
}