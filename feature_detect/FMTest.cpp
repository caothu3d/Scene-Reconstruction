#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <cmath>
#include "FeatureDetectorSIFT.h"
#include "KeyPointDescriptor.h"
#include "Image.h"


using namespace cv;
using namespace cv::xfeatures2d;
using namespace SceneComps;
using namespace std;

vector<DMatch> matches;
vector<DMatch> emptyMatches;
Mat img_matches;

vector<KeyPoint> sift_keypoints1;
vector<KeyPoint> sift_keypoints2;

Image image1("");
Image image2("");
int matchColor = 250;
int pointColor = 0;

void onMouse(int event, int x, int y, int flags, void* userdata) {
    if ( event == EVENT_MOUSEMOVE ) {

        // Index and distance of closest keypoint
        int closest = 0;
        float minDistance = 1000;

        // Find closest keypoint
        int index = 0;
        for (std::vector<KeyPoint>::iterator it = sift_keypoints1.begin(); it != sift_keypoints1.end(); ++it) {
            int kpx = it->pt.x;
            int kpy = it->pt.y;
            float distance = sqrt(pow(x-kpx, 2) + pow(y-kpy, 2));
            if (distance < minDistance) {
                minDistance = distance;
                closest = index;
            }
            index ++;
        }

        vector<DMatch> line;

        // If in distance threshold, find the matching. Otherwise, draw an empty matching
        if (minDistance < 20) {
            for (std::vector<DMatch>::iterator it = matches.begin(); it != matches.end(); ++it) {
                if (it->queryIdx == closest) {
                    line.push_back(*it);
                }
            }
        }
        drawMatches(image1.matrix, sift_keypoints1, image2.matrix, sift_keypoints2, line, img_matches, matchColor, pointColor);
        imshow("matches", img_matches);
    }
}

int main( int argc, char** argv ) {

	if (argc != 3) { 
		cout << "Must provide filepath argument.\n";
		return -1; 
	}

	// Create image
	string filepath1 = argv[1];
	image1 = Image(filepath1);
    string filepath2 = argv[2];
	image2 = Image(filepath2);
    
    // Detect keypoints
    FeatureDetectorSIFT siftDetector = FeatureDetectorSIFT();
    vector<KeypointDescriptor> keypoints1 = siftDetector.detect(image1);
    vector<KeypointDescriptor> keypoints2 = siftDetector.detect(image2);

    // Convert descriptors back to cv keypoints :(
    sift_keypoints1 = vector<KeyPoint>(keypoints1.begin(), keypoints1.end());
    sift_keypoints2 = vector<KeyPoint>(keypoints2.begin(), keypoints2.end());

    //STUFF FROM THE OPEN CV EXAMPLE BELOW
    // https://github.com/npinto/opencv/blob/master/samples/cpp/matcher_simple.cpp
    cv::Ptr<Feature2D> f2d = xfeatures2d::SIFT::create();
    
    //SiftDescriptorExtractor extractor;
    Mat descriptors1, descriptors2; 
    f2d->compute(image1.matrix, sift_keypoints1, descriptors1);
    f2d->compute(image2.matrix, sift_keypoints2, descriptors2);
    
    BFMatcher matcher;
    matcher.match(descriptors1, descriptors2, matches);
    
    // drawing the results
    namedWindow("matches", 1);
    setMouseCallback("matches", onMouse, NULL);

    drawMatches(image1.matrix, sift_keypoints1, image2.matrix, sift_keypoints2, emptyMatches, img_matches, matchColor, pointColor);
    imshow("matches", img_matches);

	cout << "^C to exit.\n";

    waitKey(0);

	return 0;
}
