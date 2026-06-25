#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>
#include <limits>

using namespace std;
using namespace cv;

int clampValue(int val, int low, int high) {
    return max(low, min(val, high));
}

double getRectangleSum(const Mat& integralImg, int x1, int y1, int x2, int y2) {
    // convert image coordinates to integral-image coordinates
    int left = x1;
    int top = y1;
    int right = x2 + 1;
    int bottom = y2 + 1;

    return integralImg.at<double>(bottom, right)
         - integralImg.at<double>(top, right)
         - integralImg.at<double>(bottom, left)
         + integralImg.at<double>(top, left);
}

int main(int argc, char** argv) {
   
    if (argc != 4) {
        cout << "Usage: kuwahara <input_image> <output_image> <kernel_size>" << endl;
        return 1;
    }

    string inputFile = argv[1];
    string outputFile = argv[2];
    int k = atoi(argv[3]);

    if (k < 3 || k % 2 == 0) {
        cout << "Error: kernel size must be an odd number >= 3" << endl;
        return 1;
    }

    Mat input = imread(inputFile, IMREAD_GRAYSCALE);
    if (input.empty()) {
        cout << "Error: could not open input image." << endl;
        return 1;
    }

    int rows = input.rows;
    int cols = input.cols;
    int r = k / 2;

    Mat output(rows, cols, CV_8UC1);

    // Integral image and squared integral image
    Mat integralSum = Mat::zeros(rows + 1, cols + 1, CV_64F);
    Mat integralSqSum = Mat::zeros(rows + 1, cols + 1, CV_64F);

    // Build summed-area tables
    for (int y = 1; y <= rows; y++) {
        for (int x = 1; x <= cols; x++) {
            double pixel = static_cast<double>(input.at<uchar>(y - 1, x - 1));
            double pixelSq = pixel * pixel;

            integralSum.at<double>(y, x) =
                pixel
                + integralSum.at<double>(y - 1, x)
                + integralSum.at<double>(y, x - 1)
                - integralSum.at<double>(y - 1, x - 1);

            integralSqSum.at<double>(y, x) =
                pixelSq
                + integralSqSum.at<double>(y - 1, x)
                + integralSqSum.at<double>(y, x - 1)
                - integralSqSum.at<double>(y - 1, x - 1);
        }
    }

    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {

            double minVariance = numeric_limits<double>::max();
            double bestMean = 0.0;

            for (int region = 0; region < 4; region++) {
                int x1, y1, x2, y2;

                if (region == 0) {
                    x1 = x - r; y1 = y - r;
                    x2 = x;     y2 = y;
                } else if (region == 1) {
                    x1 = x;     y1 = y - r;
                    x2 = x + r; y2 = y;
                } else if (region == 2) {
                    x1 = x - r; y1 = y;
                    x2 = x;     y2 = y + r;
                } else {
                    x1 = x;     y1 = y;
                    x2 = x + r; y2 = y + r;
                }

                // clip to valid image boundaries
                x1 = clampValue(x1, 0, cols - 1);
                y1 = clampValue(y1, 0, rows - 1);
                x2 = clampValue(x2, 0, cols - 1);
                y2 = clampValue(y2, 0, rows - 1);

                int N = (x2 - x1 + 1) * (y2 - y1 + 1);
                if (N <= 0) {
                    continue;
                }

                double sum = getRectangleSum(integralSum, x1, y1, x2, y2);
                double sumSq = getRectangleSum(integralSqSum, x1, y1, x2, y2);

                double mean = sum / N;
                double varianceMeasure = sumSq - (sum * sum) / N;

                if (varianceMeasure < minVariance) {
                    minVariance = varianceMeasure;
                    bestMean = mean;
                }
            }

            int outValue = static_cast<int>(round(bestMean));
            outValue = clampValue(outValue, 0, 255);
            output.at<uchar>(y, x) = static_cast<uchar>(outValue);
        }
    }

    if (!imwrite(outputFile, output)) {
        cout << "Error: could not save output image." << endl;
        return 1;
    }

    cout << "Kuwahara filtering completed successfully." << endl;
    return 0;
}