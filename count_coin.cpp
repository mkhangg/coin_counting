/***********************************************************************************************************************
* @file: count_coin.cpp
* @brief: coin counting program using morphological analysis
* @author: khang nguyen
***********************************************************************************************************************/

#include <string>
#include <iostream>
#include <algorithm>
#include "opencv2/opencv.hpp"

#define LOWER_BOUND_DIME 162
#define UPPER_BOUND_DIME 170
#define LOWER_BOUND_PENNY 172
#define UPPER_BOUND_PENNY 180
#define LOwER_BOUND_NICKEL 192
#define UPPER_BOUND_NICKEL 200
#define LOWER_BOUND_QUARTER 220
#define UPPER_BOUND_QUARTER 228
#define NUM_COMNMAND_LINE_ARGUMENTS 1

using namespace cv;
using namespace std;

int main(int argc, char **argv) {
	Mat inputImage, templateImage, grayImage, edgeDetectedImage, edgeDilatedImage, edgeErodedImage;
    
    const int resize_width = 768, resize_height = 1024;
    
    const int cannyThres1 = 100, cannyThres2 = 200, cannyAperture = 3;
    const double morphologySize = 5.00;
    const int minEllipseInliers = 50;
    
    int dime_count = 0;
    int penny_count = 0;
    int nickel_count = 0;
    int quarter_count = 0;

    // Check for command line arguments
    if (argc != NUM_COMNMAND_LINE_ARGUMENTS + 1) {
        printf("Error CLI.\n");
		return 0;
    } else {
        inputImage = imread(argv[1], IMREAD_COLOR);

        // Check for file error
        if (inputImage.data == 0) {
            cout << "Error while opening file " << argv[1] << endl;
            return 0;
        }
    }

    // Resize images
    resize(inputImage, inputImage, Size(resize_width, resize_height), INTER_LINEAR);

    cout << "Width: " << inputImage.size().width << endl;
    cout << "Height: " << inputImage.size().height << endl;
    cout << "No channels: " << inputImage.channels() << endl;

    // Convert the input image to grayscale
    cvtColor(inputImage, grayImage, COLOR_BGR2GRAY);

    // Detect edges in the input image
    Canny(grayImage, edgeDetectedImage, cannyThres1, cannyThres2, cannyAperture);
    
    // Denoise small morphologies by dilating and eroding back
    dilate(edgeDetectedImage, edgeDilatedImage, Mat(), Point(-1, -1), morphologySize);
    erode(edgeDilatedImage, edgeErodedImage, Mat(), Point(-1, -1), morphologySize);

    // Find contours in the input image
    vector<vector<Point> > contours;
    findContours(edgeErodedImage, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));

    // Find and draw ellipses based on contours in the input image
    vector<RotatedRect> fittedEllipses(contours.size());
    for (int i = 0; i < contours.size(); i++) {
        // Compute an ellipse only if the contour has more than 5 points (the minimum for ellipse fitting)
        if (contours.at(i).size() > 5) {
            fittedEllipses[i] = fitEllipse(contours[i]);
        }
    }

    templateImage = inputImage.clone();
    for (int i = 0; i < contours.size(); i++) {
        if (contours.at(i).size() > minEllipseInliers) {
            
            Scalar color = Scalar(rand()%256, rand()%256, rand()%256);

            // Count number of coins in each category
            int dim = max(fittedEllipses[i].size.width, fittedEllipses[i].size.height);
            if (dim <= UPPER_BOUND_QUARTER && dim >= LOWER_BOUND_QUARTER) {
                color = Scalar(0, 255, 0);      // Green
                quarter_count++;
            } else if (dim <= UPPER_BOUND_NICKEL && dim >= LOwER_BOUND_NICKEL) {
                color = Scalar(0, 255, 255);    // Yellow
                nickel_count++;
            } else if (dim <= UPPER_BOUND_PENNY && dim >= LOWER_BOUND_PENNY) {
                color = Scalar(0, 0, 255);      // Red
                penny_count++;
            } else if (dim <= UPPER_BOUND_DIME && dim >= LOWER_BOUND_DIME) {
                color = Scalar(255, 0, 0);      // Blue
                dime_count++;
            }

            ellipse(templateImage, fittedEllipses[i], color, 2);

            // cout << "fittedEllipses[" << i << "].size.width = " << fittedEllipses[i].size.width << endl;
            // cout << "fittedEllipses[" << i << "].size.height = " << fittedEllipses[i].size.height << endl;
        }
    }

    // Calculate the total price of coins
    double total_price = penny_count*0.01 + nickel_count*0.05 + dime_count*0.10 + quarter_count*0.25;

    // Print results to terminal
    cout << "Penny - " << penny_count << endl;
    cout << "Nickel - " << nickel_count << endl;
    cout << "Dime - " << dime_count << endl;
    cout << "Quarter - " << quarter_count << endl;
    cout << "Total - $" << total_price << endl;

    // Display images
	imshow("Input Image", inputImage);
    imshow("Detected Coins", templateImage);
    
    waitKey();

	return 0;
}
