#include <cstdio>

#include <conio.h>
 
#include "opencv2/opencv.hpp"

void drawCandy( cv::Mat frame , char * windowName );
void drawSobel( cv::Mat frame , char * windowName );
void drawDraw( cv::Mat frame , char * windowName );

void checkAlgorithm( char * windowName , bool & flag , char * message );

cv::Vec3b calculatePixel( cv::Vec3b pixel );

int main(int argc, char * argv[]) {
	char * windowName = "Video Analysis Candy";
	char * windowName2 = "Video Analysis Sobel";
	char * windowNameRaw = "Video Analysis Raw";
	char * windowNameDraw = "Video Analysis Draw";

	cv::VideoCapture cameraDevice;
	
	cameraDevice.open(0);

    if( !cameraDevice.isOpened() ){
        printf("Couldn't open camera stream\n"); 
    }
    else {
        printf("Camera opened properly\n");

		printf( "s - sobel algorithm\nc - candy algorithm\nr - raw frame\nq - quit\n" );

		cv::Mat frame;

		bool candyFrame = false,
			sobelFrame = false,
			rawFrame = false;

		char getKey = 0;

		cv::namedWindow( windowNameDraw , CV_WINDOW_AUTOSIZE );

		while( true ){
			cameraDevice >> frame;

			getKey = 0;

			if( kbhit() ){
				getKey = getch();
			}

			if( getKey == 'q' ){
				break;
			}

			if( getKey == 'r' && !rawFrame ){
				checkAlgorithm( windowNameRaw , rawFrame , "Raw frame run\n" );
			}

			if( getKey == 'c' && !candyFrame ){
				checkAlgorithm( windowName , candyFrame , "Candy algorithm run\n" );
			}

			if( getKey == 's' && !sobelFrame ){
				checkAlgorithm( windowName2 , sobelFrame , "Sobel algorithm run\n" );
			}

			if( candyFrame ){
				drawCandy( frame , windowName );
			}
			if( sobelFrame ){
				drawSobel( frame , windowName2 );
			}
			if( rawFrame ){
				cv::imshow( windowNameRaw , frame );
			}

			drawDraw( frame , windowNameDraw );

			cv::waitKey( 5 );
		}
    }

	getchar();

    return 0;
}

void drawCandy( cv::Mat frame , char * windowName ){
	//candy algorithm 
	cv::Mat edges;

	cv::cvtColor(frame, edges, CV_BGR2GRAY);
	cv::GaussianBlur(edges, edges, cv::Size(7,7), 1.5, 1.5);
			
	cv::Canny(edges, edges, 0, 30, 3);
	cv::imshow( windowName, edges );
}

void drawSobel( cv::Mat frame , char * windowName ){
	//sobel algorithm

	cv::Mat edges;

	cv::GaussianBlur( frame, edges, cv::Size(3,3), 0, 0, cv::BORDER_DEFAULT );
	cv::cvtColor( edges, edges, CV_RGB2GRAY );

	cv::Mat grad_x, grad_y;
	cv::Mat abs_grad_x, abs_grad_y;

	cv::Sobel( edges, grad_x, CV_16S, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT );
	cv::Sobel( edges, grad_y, CV_16S, 0, 1, 3, 1, 0, cv::BORDER_DEFAULT );

	cv::convertScaleAbs( grad_x, abs_grad_x );
	cv::convertScaleAbs( grad_y, abs_grad_y );

	cv::addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, edges );
			
	cv::imshow( windowName, edges );
}

void drawDraw( cv::Mat frame , char * windowName ){
	cv::Mat frameCopy;
	cv::Mat frameDiff;
	cv::Mat frameDiff2;

	cv::MatIterator_<cv::Vec3b> it, end;

	cv::Vec3b test( 0 , 0 , 255 );

	cv::Vec3b colorSearch = calculatePixel( test );

	int iTotal = 0;

	frame.copyTo( frameCopy );
	frameCopy.copyTo( frameDiff2 );

	for( it = frameCopy.begin<cv::Vec3b>(), end = frameCopy.end<cv::Vec3b>(); it != end; ++it ){

		iTotal = (*it)[0] + (*it)[1] + (*it)[2];

		if( iTotal != 0 ){
			(*it)[0] = ( (*it)[0] * 255 ) / iTotal;
			(*it)[1] = ( (*it)[1] * 255 ) / iTotal;
			(*it)[2] =  255 - (*it)[0] - (*it)[1];
		}
	}

	for( it = frameDiff2.begin<cv::Vec3b>(), end = frameDiff2.end<cv::Vec3b>(); it != end; ++it ){
		(*it)[0] = colorSearch.val[ 0 ];
		(*it)[1] = colorSearch.val[ 1 ];
		(*it)[2] = colorSearch.val[ 2 ];
	}

	cv::absdiff( frameCopy ,  frameDiff2, frameDiff );

	frameDiff2 = cv::Mat(frameDiff.size(),CV_8U);
    for(int i=0;i<frameDiff2.rows;i++)
    for(int i2=0;i2<frameDiff2.cols;i2++) {
        cv::Vec3b piksel = frameDiff.at<cv::Vec3b>(i,i2);
        frameDiff2.at<uchar>(i,i2) = cv::saturate_cast<uchar>((int)piksel.val[0]+piksel.val[1]+piksel.val[2]);
    }

	cv::threshold(frameDiff2,frameDiff2,150,255,cv::THRESH_BINARY_INV);

	cv::imshow( windowName , frameDiff2 );
}

void checkAlgorithm( char * windowName , bool & flag , char * message ){
	flag = true;

	printf( "%s" , message );

	cv::namedWindow( windowName , CV_WINDOW_AUTOSIZE );
}

cv::Vec3b calculatePixel( cv::Vec3b pixel ){
	int iTotal = pixel.val[0] + pixel.val[1] + pixel.val[2];

	if( iTotal != 0 ){
		pixel.val[0] = ( pixel.val[0] * 255 ) / iTotal;
		pixel.val[1] = ( pixel.val[1] * 255 ) / iTotal;
		pixel.val[2] =  255 - pixel.val[0] - pixel.val[1];
	}

	return pixel;
}