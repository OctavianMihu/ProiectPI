// OpenCVApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdlib.h>
#include "common.h"
#include <opencv2/core/utils/logger.hpp>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <Windows.h>
#include <string.h>
#include <stdio.h>
#include <windows.h>
#include <iostream>

#define MAX_LINE_LENGTH 256
#define NUM_TAGS 4


wchar_t* projectPath;

void testOpenImage()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src;
		src = imread(fname);
		imshow("image", src);
		waitKey();
	}
}

void testOpenImagesFld()
{
	char folderName[MAX_PATH];
	if (openFolderDlg(folderName) == 0)
		return;
	char fname[MAX_PATH];
	FileGetter fg(folderName, "bmp");
	while (fg.getNextAbsFile(fname))
	{
		Mat src;
		src = imread(fname);
		imshow(fg.getFoundFileName(), src);
		if (waitKey() == 27) //ESC pressed
			break;
	}
}

void testImageOpenAndSave()
{
	_wchdir(projectPath);

	Mat src, dst;

	src = imread("Images/Lena_24bits.bmp", IMREAD_COLOR);	// Read the image

	if (!src.data)	// Check for invalid input
	{
		printf("Could not open or find the image\n");
		return;
	}

	// Get the image resolution
	Size src_size = Size(src.cols, src.rows);

	// Display window
	const char* WIN_SRC = "Src"; //window for the source image
	namedWindow(WIN_SRC, WINDOW_AUTOSIZE);
	moveWindow(WIN_SRC, 0, 0);

	const char* WIN_DST = "Dst"; //window for the destination (processed) image
	namedWindow(WIN_DST, WINDOW_AUTOSIZE);
	moveWindow(WIN_DST, src_size.width + 10, 0);

	cvtColor(src, dst, COLOR_BGR2GRAY); //converts the source image to a grayscale one

	imwrite("Images/Lena_24bits_gray.bmp", dst); //writes the destination to file

	imshow(WIN_SRC, src);
	imshow(WIN_DST, dst);

	waitKey(0);
}

void testNegativeImage()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		double t = (double)getTickCount(); // Get the current time [s]

		Mat src = imread(fname, IMREAD_GRAYSCALE);
		int height = src.rows;
		int width = src.cols;
		Mat dst = Mat(height, width, CV_8UC1);
		// Accessing individual pixels in an 8 bits/pixel image
		// Inefficient way -> slow
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				uchar val = src.at<uchar>(i, j);
				uchar neg = 255 - val;
				dst.at<uchar>(i, j) = neg;
			}
		}

		// Get the current time again and compute the time difference [s]
		t = ((double)getTickCount() - t) / getTickFrequency();
		// Print (in the console window) the processing time in [ms] 
		printf("Time = %.3f [ms]\n", t * 1000);

		imshow("input image", src);
		imshow("negative image", dst);
		waitKey();
	}
}

void testNegativeImageFast()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src = imread(fname, IMREAD_GRAYSCALE);
		int height = src.rows;
		int width = src.cols;
		Mat dst = src.clone();

		double t = (double)getTickCount(); // Get the current time [s]

		// The fastest approach of accessing the pixels -> using pointers
		uchar* lpSrc = src.data;
		uchar* lpDst = dst.data;
		int w = (int)src.step; // no dword alignment is done !!!
		for (int i = 0; i < height; i++)
			for (int j = 0; j < width; j++) {
				uchar val = lpSrc[i * w + j];
				lpDst[i * w + j] = 255 - val;
			}

		// Get the current time again and compute the time difference [s]
		t = ((double)getTickCount() - t) / getTickFrequency();
		// Print (in the console window) the processing time in [ms] 
		printf("Time = %.3f [ms]\n", t * 1000);

		imshow("input image", src);
		imshow("negative image", dst);
		waitKey();
	}
}

void testColor2Gray()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src = imread(fname);

		int height = src.rows;
		int width = src.cols;

		Mat dst = Mat(height, width, CV_8UC1);

		// Accessing individual pixels in a RGB 24 bits/pixel image
		// Inefficient way -> slow
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				Vec3b v3 = src.at<Vec3b>(i, j);
				uchar b = v3[0];
				uchar g = v3[1];
				uchar r = v3[2];
				dst.at<uchar>(i, j) = (r + g + b) / 3;
			}
		}

		imshow("input image", src);
		imshow("gray image", dst);
		waitKey();
	}
}

void testBGR2HSV()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src = imread(fname);
		int height = src.rows;
		int width = src.cols;

		// HSV components
		Mat H = Mat(height, width, CV_8UC1);
		Mat S = Mat(height, width, CV_8UC1);
		Mat V = Mat(height, width, CV_8UC1);

		// Defining pointers to each matrix (8 bits/pixels) of the individual components H, S, V 
		uchar* lpH = H.data;
		uchar* lpS = S.data;
		uchar* lpV = V.data;

		Mat hsvImg;
		cvtColor(src, hsvImg, COLOR_BGR2HSV);

		// Defining the pointer to the HSV image matrix (24 bits/pixel)
		uchar* hsvDataPtr = hsvImg.data;

		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				int hi = i * width * 3 + j * 3;
				int gi = i * width + j;

				lpH[gi] = hsvDataPtr[hi] * 510 / 360;	// lpH = 0 .. 255
				lpS[gi] = hsvDataPtr[hi + 1];			// lpS = 0 .. 255
				lpV[gi] = hsvDataPtr[hi + 2];			// lpV = 0 .. 255
			}
		}

		imshow("input image", src);
		imshow("H", H);
		imshow("S", S);
		imshow("V", V);

		waitKey();
	}
}

void testResize()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src;
		src = imread(fname);
		Mat dst1, dst2;
		//without interpolation
		resizeImg(src, dst1, 320, false);
		//with interpolation
		resizeImg(src, dst2, 320, true);
		imshow("input image", src);
		imshow("resized image (without interpolation)", dst1);
		imshow("resized image (with interpolation)", dst2);
		waitKey();
	}
}

void testCanny()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src, dst, gauss;
		src = imread(fname, IMREAD_GRAYSCALE);
		double k = 0.4;
		int pH = 50;
		int pL = (int)k * pH;
		GaussianBlur(src, gauss, Size(5, 5), 0.8, 0.8);
		Canny(gauss, dst, pL, pH, 3);
		imshow("input image", src);
		imshow("canny", dst);
		waitKey();
	}
}

void testVideoSequence()
{
	_wchdir(projectPath);

	VideoCapture cap("Videos/rubic.avi"); // off-line video from file
	//VideoCapture cap(0);	// live video from web cam
	if (!cap.isOpened()) {
		printf("Cannot open video capture device.\n");
		waitKey(0);
		return;
	}

	Mat edges;
	Mat frame;
	char c;

	while (cap.read(frame))
	{
		Mat grayFrame;
		cvtColor(frame, grayFrame, COLOR_BGR2GRAY);
		Canny(grayFrame, edges, 40, 100, 3);
		imshow("source", frame);
		imshow("gray", grayFrame);
		imshow("edges", edges);
		c = waitKey(100);  // waits 100ms and advances to the next frame
		if (c == 27) {
			// press ESC to exit
			printf("ESC pressed - capture finished\n");
			break;  //ESC pressed
		};
	}
}


void testSnap()
{
	_wchdir(projectPath);

	VideoCapture cap(0); // open the deafult camera (i.e. the built in web cam)
	if (!cap.isOpened()) // openenig the video device failed
	{
		printf("Cannot open video capture device.\n");
		return;
	}

	Mat frame;
	char numberStr[256];
	char fileName[256];

	// video resolution
	Size capS = Size((int)cap.get(CAP_PROP_FRAME_WIDTH),
		(int)cap.get(CAP_PROP_FRAME_HEIGHT));

	// Display window
	const char* WIN_SRC = "Src"; //window for the source frame
	namedWindow(WIN_SRC, WINDOW_AUTOSIZE);
	moveWindow(WIN_SRC, 0, 0);

	const char* WIN_DST = "Snapped"; //window for showing the snapped frame
	namedWindow(WIN_DST, WINDOW_AUTOSIZE);
	moveWindow(WIN_DST, capS.width + 10, 0);

	char c;
	int frameNum = -1;
	int frameCount = 0;

	for (;;)
	{
		cap >> frame; // get a new frame from camera
		if (frame.empty())
		{
			printf("End of the video file\n");
			break;
		}

		++frameNum;

		imshow(WIN_SRC, frame);

		c = waitKey(10);  // waits a key press to advance to the next frame
		if (c == 27) {
			// press ESC to exit
			printf("ESC pressed - capture finished");
			break;  //ESC pressed
		}
		if (c == 115) { //'s' pressed - snap the image to a file
			frameCount++;
			fileName[0] = NULL;
			sprintf(numberStr, "%d", frameCount);
			strcat(fileName, "Images/A");
			strcat(fileName, numberStr);
			strcat(fileName, ".bmp");
			bool bSuccess = imwrite(fileName, frame);
			if (!bSuccess)
			{
				printf("Error writing the snapped image\n");
			}
			else
				imshow(WIN_DST, frame);
		}
	}

}

void MyCallBackFunc(int event, int x, int y, int flags, void* param)
{
	//More examples: http://opencvexamples.blogspot.com/2014/01/detect-mouse-clicks-and-moves-on-image.html
	Mat* src = (Mat*)param;
	if (event == EVENT_LBUTTONDOWN)
	{
		printf("Pos(x,y): %d,%d  Color(RGB): %d,%d,%d\n",
			x, y,
			(int)(*src).at<Vec3b>(y, x)[2],
			(int)(*src).at<Vec3b>(y, x)[1],
			(int)(*src).at<Vec3b>(y, x)[0]);
	}
}

void testMouseClick()
{
	Mat src;
	// Read image from file 
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		src = imread(fname);
		//Create a window
		namedWindow("My Window", 1);

		//set the callback function for any mouse event
		setMouseCallback("My Window", MyCallBackFunc, &src);

		//show the image
		imshow("My Window", src);

		// Wait until user press some key
		waitKey(0);
	}
}

/* Histogram display function - display a histogram using bars (simlilar to L3 / Image Processing)
Input:
name - destination (output) window name
hist - pointer to the vector containing the histogram values
hist_cols - no. of bins (elements) in the histogram = histogram image width
hist_height - height of the histogram image
Call example:
showHistogram ("MyHist", hist_dir, 255, 200);
*/

void showHistogram(const std::string& name, int* hist, const int  hist_cols, const int hist_height)
{

	Mat imgHist(hist_height, hist_cols, CV_8UC3, CV_RGB(255, 255, 255)); // constructs a white image

	//computes histogram maximum
	int max_hist = 0;
	for (int i = 0; i < hist_cols; i++)
		if (hist[i] > max_hist)
			max_hist = hist[i];
	double scale = 1.0;
	scale = (double)hist_height / max_hist;
	int baseline = hist_height - 1;

	for (int x = 0; x < hist_cols; x++) {
		Point p1 = Point(x, baseline);
		Point p2 = Point(x, baseline - cvRound(hist[x] * scale));
		line(imgHist, p1, p2, CV_RGB(255, 0, 255)); // histogram bins colored in magenta
	}

	imshow(name, imgHist);
}
void ListFiles(const TCHAR* dir)
{
	WIN32_FIND_DATA info; // file/dir info
	TCHAR fullPath[MAX_PATH]; //dir path
	HANDLE hFind = INVALID_HANDLE_VALUE; //operation handle
	DWORD dwError = 0; //error codes, if any

	_tcscpy_s(fullPath, MAX_PATH, dir);  //strcpy
	_tcscat_s(fullPath, MAX_PATH, TEXT("\\*"));

	hFind = FindFirstFile(fullPath, &info);

	if (hFind == INVALID_HANDLE_VALUE) {
		printf("Invalid handle value\n");
		return;
	}
	do {
		if (_tcscmp(info.cFileName, TEXT(".")) != 0 && _tcscmp(info.cFileName, TEXT("..")) != 0)
		{
			if (info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				_tprintf("%s directory path");
			}
		}
	} while (1);
}

/*
typedef struct {
	int number;
	char tag[MAX_LINE_LENGTH];
} TagNumber;


void assignTag(int number, char* tag) {
	char* tags[NUM_TAGS] = { "Clubs", "Diamonds", "Hearts", "Spades" };


	if (number == 0 || number == 4 || number == 8 || number == 12 || number == 16 || number == 21 || number == 25 || number == 29 || number == 33 || number == 37 || number == 41
		|| number == 45 || number == 49){
		strcpy(tag, tags[0]); // Clubs
	}
	else if (number == 1 || number == 5 || number == 9 || number == 13 || number == 17 || number == 22 || number == 26 || number == 20 || number == 34 || number == 38 || number == 42
		|| number == 46 || number == 50) {
		strcpy(tag, tags[1]); // Diamonds
	}
	else if (number == 2 || number == 6 || number == 10 || number == 14 || number == 18 || number == 23 || number == 27 || number == 31 || number == 35 || number == 39 || number == 43
		|| number == 47 || number == 51) {
		strcpy(tag, tags[2]); // Hearts
	}
	else if (number == 3 || number == 7 || number == 11 || number == 15 || number == 19 || number == 24 || number == 28 || number == 32 || number == 36 || number == 40 || number == 44
		|| number == 48 || number == 52) {
		strcpy(tag, tags[3]); // Spades
	}
	else {
		strcpy(tag, "Unknown");
	}
}

void processDataFromFile(const char* filename) {
	FILE* file = fopen(filename, "r");
	if (file == NULL) {
		fprintf(stderr, "Error: Unable to open file\n");
		return;
	}

	TagNumber* tagNumbers = (TagNumber*)malloc(NUM_TAGS * sizeof(TagNumber));
	if (tagNumbers == NULL) {
		fprintf(stderr, "Error: Memory allocation failed\n");
		fclose(file);
		return;
	}


	char line[MAX_LINE_LENGTH];
	int count = 0;
	fgets(line, sizeof(line), file);
	while (fgets(line, sizeof(line), file) != NULL) {

		int number;
		sscanf(line, "%d", &number);


		assignTag(number, tagNumbers[count].tag);

		// Store the number and tag in the array
		tagNumbers[count].number = number;
		count++;
	}


	fclose(file);


	printf("Tags and their corresponding numbers:\n");
	for (int i = 0; i < count; i++) {
		printf("Number: %d, Tag: %s\n", tagNumbers[i].number, tagNumbers[i].tag);
	}


	//free (tagNumbers);
}
*/
typedef struct {
	int tag;
	Mat mat;
}image;

int count = 0;

std::vector<image> test_images;
std::vector<image> train_images;



void batchOpen(const TCHAR* dir)
{
	WIN32_FIND_DATA ffd;
	TCHAR szDir[MAX_PATH];
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError = 0;

	_tcscpy_s(szDir, MAX_PATH, dir);
	_tcscat_s(szDir, MAX_PATH, TEXT("\\*"));

	hFind = FindFirstFile(szDir, &ffd);

	if (hFind == INVALID_HANDLE_VALUE)
	{
		printf("Invalid handle value\n");
		return;
	}
	do
	{
		if (_tcscmp(ffd.cFileName, TEXT(".")) != 0 && _tcscmp(ffd.cFileName, TEXT("..")) != 0)
		{
			TCHAR subdir[MAX_PATH];
			_tcscpy_s(subdir, MAX_PATH, dir);
			_tcscat_s(subdir, MAX_PATH, TEXT("\\"));
			_tcscat_s(subdir, MAX_PATH, ffd.cFileName);
			if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				//_tprintf(TEXT("Directory: %s\n"), ffd.cFileName);
				batchOpen(subdir);
			}
			else
			{
				image img;
				if (strstr(ffd.cFileName, ".jpg")) {
					count++;
					img.mat = imread(subdir);
					image img;
					if (strstr(dir, "clubs")) {
						img.tag = 0;
						//printf("%s %d\n", dir, img.tag);
					}
					else
						if (strstr(dir, "diamonds")) {
							img.tag = 1;
							//printf("%s %d\n", dir, img.tag);
						}
						else
							if (strstr(dir, "spades")) {
								img.tag = 2;
								//printf("%s %d\n", dir, img.tag);
							}
							else
								if (strstr(dir, "heart")) {
									img.tag = 3;
									//printf("%s %d\n", dir, img.tag);
								}
					if (strstr(dir, "test")) {
						test_images.push_back(img);
					}
					else if(strstr(dir, "train")) {
						train_images.push_back(img);
					}
				}

			}
		}
	} while (FindNextFile(hFind, &ffd) != 0);

	dwError = GetLastError();
	if (dwError != ERROR_NO_MORE_FILES)
	{
		printf("FindNextFile error: %d\n", dwError);
	}

	FindClose(hFind);

}

int generate_random_label(Mat image) {
	return rand() % 4;
}

int test_generare_random()
{
	char fname[MAX_PATH];
	Mat src;
	while (openFileDlg(fname))
	{

		src = imread(fname);
		imshow("image", src);
		if (src.empty()) {
			printf("Imaginea nu a putut fi incarcata.\n");
			return -1;
		}


		int label = generate_random_label(src);


		printf("Eticheta generata pentru imagine: %d\n ", label);
		waitKey();
	}

	return 0;
}

double calculate_accuracy(const std::vector<image>& vec1, const std::vector<image>& vec2) {
	if (vec1.size() != vec2.size() || vec1.empty()) {
		printf("Eroare: Listele de etichete au lungimi diferite.\n");
		return -7.0;
	}

	int ok_count = 0;
	int total_count = vec1.size();

	for (size_t i = 0; i < vec1.size(); i++) {
		if (vec1[i].tag == vec2[i].tag) {
			ok_count++;
		}
	}

	if (total_count == 0) {
		printf("Eroare: Numarul total de imagini este zero.\n");
		return -1.0;
	}

	double accuracy = static_cast<double>(ok_count) / total_count;

	return accuracy;
}
std::vector<image> gen_pred_tags(std::vector<image>& true_tag) {
	std::vector<image> generated_tag;
	for (int i = 0; i < true_tag.size(); i++) {
		image img;
		img.mat = true_tag[i].mat;
		img.tag = generate_random_label(img.mat);
		generated_tag.push_back(img);
	}
	return generated_tag;
}

double test_vec(std::vector<image>& true_tag) {

	std::vector<image> generated_tag = gen_pred_tags(true_tag);

	return calculate_accuracy(true_tag, generated_tag);
}


void show_acc_table(std::vector<image> true_tag, std::vector<image> predicted_tag) {
	int vect_size = true_tag.size();
	int confusion[4][4] = { {0,0,0,0},
							{0,0,0,0},
							{0,0,0,0},	
							{0,0,0,0}};
	for (int i = 0; i < vect_size; i++) {
		confusion[true_tag[i].tag][predicted_tag[i].tag] += 1;
	}

	printf("|-----------------------------------------------|\n");
	printf("|\t   | clubs | diamonds | spades | hearts |\n");
	printf("|----------|-------|----------|--------|--------|\n");
	for (int i = 0; i < 4; i++) {
		printf("|");
		switch (i)
		{
		case 0: 
			printf("   clubs  |");
			break;

		case 1:
			printf(" diamonds |");
			break;
		case 2:
			printf("  spades  |");
			break;

		case 3:
			printf("  hearts  |");
			break;
		}
		for (int j = 0; j < 4; j++) {
			switch (j) {
			case 0:
				printf("  %d  |", confusion[i][j]);
				break;
			case 1:
				printf("   %d    |", confusion[i][j]);
				break;
			case 2:
				printf("  %d   |", confusion[i][j]);
				break;
			case 3:
				printf("  %d   |", confusion[i][j]);
				break;
			}
			
		}
		printf("\n");
		printf("|----------|-------|----------|--------|--------|\n");
	}
	printf("");
	waitKey(0);

}
bool opened = false;
int main()
{
	srand((int)time(0));
	/*const char* path = "C:\\Your\\Directory\\Path";
	int counter = 0;
	Directory directories[100]; // Assuming a maximum of 100 directories

	// List files and assign integer values to directory names
	listFiles(path, &counter, directories);*/

	cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_FATAL);
	projectPath = _wgetcwd(0, 0);

	FILE* file = fopen("poze.txt", "w");
	fclose(file);

	int op;
	do
	{
		system("cls");
		destroyAllWindows();
		printf("Menu:\n");
		printf(" 1 - Open image\n");
		printf(" 2 - Open BMP images from folder\n");
		printf(" 3 - Image negative\n");
		printf(" 4 - Image negative (fast)\n");
		printf(" 5 - BGR->Gray\n");
		printf(" 6 - BGR->Gray (fast, save result to disk) \n");
		printf(" 7 - BGR->HSV\n");
		printf(" 8 - Resize image\n");
		printf(" 9 - Canny edge detection\n");
		printf(" 10 - Edges in a video sequence\n");
		printf(" 11 - Snap frame from live video\n");
		printf(" 12 - Mouse callback demo\n");
		printf(" 13 - Batch Open\n");
		printf(" 14 - Test generare etichete imagine random\n");
		printf(" 15 - Accuracy\n");
		printf(" 16 - Histogram\n");
		printf(" 0 - Exit\n\n");

		printf("Option: ");
		scanf("%d", &op);
		switch (op)
		{

		case 1:
			testOpenImage();
			break;
		case 2:
			testOpenImagesFld();
			break;
		case 3:
			testNegativeImage();
			break;
		case 4:
			testNegativeImageFast();
			break;
		case 5:
			testColor2Gray();
			break;
		case 6:
			testImageOpenAndSave();
			break;
		case 7:
			testBGR2HSV();
			break;
		case 8:
			testResize();
			break;
		case 9:
			testCanny();
			break;
		case 10:
			testVideoSequence();
			break;
		case 11:
			testSnap();
			break;
		case 12:
			testMouseClick();
			break;
		case 13:
			if (!opened) {
				batchOpen(getenv("PI_Images"));
				opened = true;
			}
			{
				FILE* file = fopen("poze.txt", "w");
				fprintf(file, "Numarul de poze la care am dat tag:%d\n", count);
				fclose(file);
			}
			break;
		case 14:
			//test_generare_random();
		{
			FILE* file = fopen("poze.txt", "a");
			fprintf(file, "Acuratetea : %lf\n", calculate_accuracy(train_images, train_images));
			fclose(file);
		}
		break;
		case 15:
			if (!opened) {
				batchOpen(getenv("PI_Images"));
				opened = true;
			}
			{
				FILE* file = fopen("poze.txt", "a");
				fprintf(file, "accuracyRandom: %lf\n", test_vec(train_images));
				fclose(file);
			}
			break;
		case 16:
			if (!opened){
				opened = true;
				batchOpen(getenv("PI_Images"));
			}		
			std::vector<image> gen = gen_pred_tags(train_images);
			show_acc_table(train_images, gen);
			int ok = scanf("%s");
			break;
		}


	} while (op != 0);
	return 0;
}