
#define CAPTURE
#define FRAME
#include "capture.h"
#include "turn.h"
#include "button.h"

extern bool doTest;
extern bool togglesenabled;

/**
 * capturethread
 * capture frames and  push them to the queue
 * 
 * 
 */
void capturethread(raspicam::RaspiCam *pCamera)
{
	using std::chrono::high_resolution_clock;
	using std::chrono::milliseconds;
	Frame f;
	raspicam::RaspiCam Camera = *pCamera;
	
	printf("capturethread started in %s mode\n", doTest ? "test" : "real");
	
	if (doTest) {
		int imageWidth, imageHeight;
		unsigned long size;
		set1(true);
		
		while (!bexitcapture) {
			if (isApressed()) {
				// Load image.bmp and add it to queue for processing
				printf("capturethread in test mode.. button pressed so Loading image.bmp\n");
				BYTE* bytes = LoadBMP(&imageWidth, &imageHeight, &size, "image.bmp" );
			
				f.captureticks = clock();
				f.imageWidth = imageWidth;
				f.imageHeight = imageHeight;
				memcpy(&f.data[0], bytes,  size);
				free(bytes);

				fq.push(f);
				if (togglesenabled)
					toggle2();
				sleep(3); // sleep 3 seconds to allow button depress
			}
			std::this_thread::sleep_for(milliseconds(100));

			
		}
		printf("capturethread test mode exiting\n");	
		return;
	}
	
	printf("Connecting to camera\n");
  
    if ( !Camera.open() ) {
        printf("Error opening camera\n");
        return;
    }
    cout<<"Connected to camera ="<<Camera.getId() <<" bufs="<<Camera.getImageBufferSize( )<<endl;
    set1(true);

	unsigned char *data = new unsigned char[  Camera.getImageBufferSize( )];
	int imageWidth = Camera.getWidth();
	int imageHeight = Camera.getHeight();
	clock_t start, end;
	
	sleep(1); // sleep a little since I noticed first grab/retrieve was black
	int counter = 0;
	while (!bexitcapture) {
		start = clock();
		
		Camera.grab();
        Camera.retrieve ( data );
		f.captureticks = clock();
		
		memcpy(&f.data[0], data,  Camera.getImageBufferSize( ));
		f.imageWidth = imageWidth;
		f.imageHeight = imageHeight;
		fq.push(f);
		
		if (togglesenabled)
			toggle2();
		
		end = clock();
			     
		// sleep enough so frame capture and push takes 100ms
		std::this_thread::sleep_for(milliseconds(100 - (end - start)/(CLOCKS_PER_SEC/100)));   
	}
	Camera.release();
	printf("camera released\n");
	printf("capturethread exiting\n");
}
