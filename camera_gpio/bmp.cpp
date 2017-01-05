#include "bmp.h"
#include <vector>
#include <stdlib.h>

#include <thread>
#include "frame.h"
#include "action.h"
#include "state.h"

using namespace std;

extern bool doTest;
extern bool doVerbose;
extern bool doDumpAction;
extern int BmpNum;
int imageWidth, imageHeight;

BYTE savedata[1024*1024*3];

void SaveBitmapToFile(BYTE* pBitmapBits, 
					  LONG lWidth,   
					  LONG lHeight,  
					  WORD wBitsPerPixel,   
					  const unsigned long& padding_size, 
					  char *lpszFileName )   
{       
	// Some basic bitmap parameters       
	unsigned long headers_size = sizeof( BITMAPFILEHEADER ) +
		sizeof( BITMAPINFOHEADER );        
	unsigned long pixel_data_size = lHeight * ( ( lWidth * ( wBitsPerPixel / 8 ) ) + padding_size );    
	BITMAPINFOHEADER bmpInfoHeader = {0};             
	// Set the size    
	bmpInfoHeader.biSize = sizeof(BITMAPINFOHEADER);    
	// Bit count      
	bmpInfoHeader.biBitCount = wBitsPerPixel;   
	// Use all colors      
	bmpInfoHeader.biClrImportant = 0;   
	// Use as many colors according to bits per pixel  
	bmpInfoHeader.biClrUsed = 0;             
	// Store as un Compressed    
	bmpInfoHeader.biCompression = BI_RGB;  
	// Set the height in pixels   
	bmpInfoHeader.biHeight = lHeight;   
	// Width of the Image in pixels    
	bmpInfoHeader.biWidth = lWidth;   
	// Default number of planes    
	bmpInfoHeader.biPlanes = 1;    
	// Calculate the image size in bytes   
	bmpInfoHeader.biSizeImage = pixel_data_size;     
	BITMAPFILEHEADER bfh = {0};      
	// This value should be values of BM letters i.e 0x4D42 
	// 0x4D = M 0×42 = B storing in reverse order to match with endian 
	bfh.bfType = 0x4D42;  
	//bfh.bfType = 'B'+('M' << 8);    
	// <<8 used to shift ‘M’ to end  */  
	// Offset to the RGBQUAD      
	bfh.bfOffBits = headers_size;   
	// Total size of image including size of headers   
	bfh.bfSize =  headers_size + pixel_data_size;
	 
	// Create the file in disk to write  
	FILE *out;
    out = fopen(lpszFileName,"wb");
    if (out == NULL)
       return;
    
  	// Write the File header 
	fwrite(&bfh, sizeof(char), sizeof(bfh), out);
  	// Write the bitmap info header
	fwrite(&bmpInfoHeader, sizeof(char), sizeof(bmpInfoHeader), out);
  	// Write the RGB Data 
    fwrite(pBitmapBits, sizeof(char), bmpInfoHeader.biSizeImage, out);
    
  	// Close the file handle
    fclose (out);  
} 
   
/**
 *  LoadBMP opens bmp file in filename and returns data array
 */
BYTE *LoadBMP(int *width, int *height, unsigned long *size, char *filename)
{
	BITMAPFILEHEADER bmpheader;
	BITMAPINFOHEADER bmpinfo;
	
    FILE *in;
    in=fopen(filename,"rb");
 
    fread(&bmpheader,sizeof(BITMAPFILEHEADER),1,in);
    fread(&bmpinfo,sizeof(BITMAPINFOHEADER),1,in);
    
    if (bmpheader.bfType != 'MB') {
		return NULL;
	}
	
	*width = bmpinfo.biWidth;
	*height = bmpinfo.biHeight;
	
	if (bmpinfo.biCompression != BI_RGB)
	   return NULL;
	   
	if (bmpinfo.biBitCount != 24)
		return NULL;
		
    *size = bmpheader.bfSize - bmpheader.bfOffBits;
    
    BYTE *buffer = new BYTE[*size];
    fseek(in, bmpheader.bfOffBits, SEEK_SET);
    fread(buffer, *size, 1, in);
    
    fclose(in);		
		
    return buffer;
}


#define VAL_IGNORE 180
#define LINE_CONTRAST 1.075
#define DELTAVAL 30
#define VERT 6
#define HORIZ 6
#define RANGEL 400
#define RANGEH 400

#define VAL(POS) pmatrix[(POS)] + pmatrix[(POS)+1] + pmatrix[(POS)+2]
#define SETVAL(POS,R,G,B) pmatrix[(POS)] = (R); pmatrix[(POS+1)] = (G); pmatrix[(POS+2)] = (B)			
#define POSFIDX(IDX) (IDX)*3

/**
 *  firstroadrowFrows
 *  from a set of rows deemed to all be the first road row,
 *  return the avg row as long as all rows are no more than
 *  THRESHHOLDPIXELS_FIRSTROADROW pixel from the avg, else -1.
 *  To guarantee the row will always be from the row, return
 *  avg + 2*THRESHHOLDPIXELS_FIRSTROADROW
 *  
 *  
 */
int firstroadrowFrows(int *rows, int count)
{
	const int THRESHHOLDPIXELS_FIRSTROADROW = 5;
	int sumrow = 0;
	for (int i = 0; i < count; i++) {
		sumrow += rows[i];
	}
	int avgrow = sumrow/count;
	for (int i = 0; i < count; i++) {
		if (abs(avgrow - rows[i]) > THRESHHOLDPIXELS_FIRSTROADROW)
		    return -1;
	}
	return avgrow + 2*THRESHHOLDPIXELS_FIRSTROADROW;
}

int findfirstroadrow(BYTE *pmatrix, int width, int height)
{ 
	int largestdelta = 0;	
	int largestrow = 0;
	int offset, offset_next;
	int middle = width/2;
	
	for (int i = 0; i < height/2; i++) {	
		if (i == 0) {	
			offset = i*POSFIDX(width) + POSFIDX(middle);
		} else {
			offset = offset_next;
		}
		 
		offset_next = (i+1)*POSFIDX(width) + POSFIDX(middle);
		
		int val = VAL(offset);		
		int val_next = VAL(offset_next);
		
		int delta = abs(val_next - val);

		if (delta > largestdelta) {
			largestdelta = delta;
			largestrow = i;
		}
	}
	// largestdelta_middle represents the largest jump down the middle so assume this is where
	// the car and road meet
	return largestrow;
}

/**
 *  findlargestval
 *  return largestval and largestvalidx for a row at y from a/3 to b/3 columns
 */
void findlargestval(BYTE* pmatrix, int apos, int bpos, int rowpos, int &largestval, int &largestvalidx) 
{
	int val;
	if (apos == 0) { // left side
		for (int xpos = apos; xpos < bpos; xpos += 3) {
			val = VAL(rowpos + xpos);

			if (val < VAL_IGNORE) 
				continue;
			if (val > largestval) {
				largestval = val;
				largestvalidx = xpos/3;
			}	
		} // for
	} else { // right side
		for (int xpos = apos; xpos < bpos; xpos += 3) {
			val = VAL(rowpos + xpos);
			// adjust to prefer more inner lane (closer to middle)
			if (val < VAL_IGNORE) 
				continue;
			if (val > largestval) {
				largestval = val;
				largestvalidx = xpos/3;
			}	
		} // for
	}	
}
	
std::vector<RI> pri_1_left;	 // pass 1 left
std::vector<RI> pri_1_right; // pass 1 right
 
void pass1(int lb, int ub, int width, int height, int y, BYTE* pmatrix)
{
	RI aRI;
	bool b = true;
	int largestval = 0;
	int largestvalidx = 0;
	bool bisleft = (lb == 0);
	long rowpos = y*POSFIDX(width);
	findlargestval(pmatrix, lb, ub, rowpos, largestval, largestvalidx);

	if (!bisleft)
	   b = ((width - largestvalidx) > 10);
	if ((largestvalidx > 10) && (b)) {
		int EXPECTEDWIDTH =  10;
		
		int lpos = POSFIDX(largestvalidx - EXPECTEDWIDTH);
		int rpos = POSFIDX(largestvalidx + EXPECTEDWIDTH);
		
		int lval = VAL(rowpos + lpos);
		int rval = VAL(rowpos + rpos);
		int dleftval = (largestval - lval);
		int drightval = (largestval - rval);
					
		if ((dleftval > DELTAVAL) && (drightval > DELTAVAL) ) {
			aRI.row = height - y;
				
			// pixel at [largestvalidx] is brightest pixel	
			// want to pick index of pixel between [largestvalidx - EXPECTEDWIDTH]
			// and [largestvalidx] which has largest change in brightness when 
			// compared to adjacent pixel to the right.  This way we find the
			// border of the marking instead of somewhere inside.
			// This assumes marking is at most EXPECTEDWIDTH pixels wide.		
			int largestdiffval = 0;
			int largestidx = 0;
			for (int idx = largestvalidx - EXPECTEDWIDTH; idx <= largestvalidx ; idx++) {
				int nextpos = POSFIDX(idx+1);
				int pos = nextpos - 3;
				int diffval = VAL(rowpos + nextpos) - VAL(rowpos + pos);
				if (diffval > largestdiffval) {
					largestdiffval = diffval;
					largestidx = idx;
				}							
			}						
			aRI.idx = largestidx;
			
			if (bisleft)	
				pri_1_left.push_back(aRI);
			else
				pri_1_right.push_back(aRI);
					
			// mark first pass pixels red
			int largestpos = POSFIDX(largestidx);
			SETVAL(rowpos + largestpos, 0, 0, 255);	
			//printf("red %s y = %d idx = %d largestdiff = %d\n", bisleft ? "left" : "right", aRI.row, aRI.idx, largestdiffval);
		}
	}
}  
 
 std::vector<RI> pri_2_left;  // 2nd pass left
 std::vector<RI> pri_2_right; // 2nd pass right
 	
void pass2(bool bisleft, std::vector<RI> in, std::vector<RI> &out, int width, int height,  BYTE *pmatrix)
{
	// now exclude all that are not lines
	bool newline = true;

	for (int i = 1; i < (int) in.size(); i++) {		
		if ((in[i-1].row > in[i].row) && ((in[i-1].row - in[i].row) < VERT)) {
			int idx1 = in[i-1].idx ;
			int idx2 = in[i].idx;
			int delta = bisleft ? (idx2 - idx1) : (idx1 - idx2);
			if ((delta > 0) && (delta < HORIZ)) {
//printf("%s delta %d good row = %d idx = %d\n", bisleft ? "L" : "R", delta, in[i].row, in[i].idx);				
				if (newline) {
//printf("new\n");					
						out.push_back(in[i-1]);
						out.push_back(in[i]);	
						newline = false;
				} else {
					out.push_back(in[i]);
				}
			} else if ((delta < 0) && (out.size() < 20)) { // was 8
				if (delta >= -2) // skip
					continue;
//printf("%s clear delta %d neg row = %d idx = %d size = %d\n", bisleft ? "L" : "R", delta, in[i].row, in[i].idx, out.size());							
				out.clear();
				newline = true;
			} else if (delta < 0) {
				break;
			}
		} else {
			if (out.size() >= 8)  // was 8  with 20 here and 20 there saw all blues
				break;
			else {	
//printf("%s clear vert neg row = %d idx = %d size = %d\n", bisleft ? "L" : "R", in[i].row, in[i].idx, out.size());				
				//printf("not vert and out size = %d < 10\n", out.size());
				newline = true;
				out.clear();
			}
		}
	}
	
	// mark second pass pixels as blue
	for (int i = 0; i < (int) out.size(); i++) {
		int pos = (height - out[i].row)*POSFIDX(width) + POSFIDX(out[i].idx);
		SETVAL(pos, 255, 0, 0);
	}
}

void dumppri(std::vector<RI> in, int width, int height) 
{
	for (int i = 0; i < (int) in.size(); i++) {
		printf("(%d %d) ", in[i].row, in[i].idx);	
	}
	printf("\n");
}

bool GetFrameStats(BYTE* pmatrix, int width, int height, int firstrow, FrameStats &fs)
{
	static bool bFirstCall = true;
	
	for (int y = firstrow; y < height; y++) {					
		pass1(0, 3*width/2, width, height, y, pmatrix);
		pass1(3*width/2, 3*(width-1), width, height, y, pmatrix);
	}
	
	if (doTest && doVerbose) {
		printf("pri_1_left\n");
		dumppri(pri_1_left, width, height); 
	
	    printf("pri_1_right\n");
		dumppri(pri_1_right, width, height);

		#define SETVAL2(POS,R,G,B) pm[(POS)] = (R); pm[(POS+1)] = (G); pm[(POS+2)] = (B)			

		BYTE blackmatrix[width*3][height];
		BYTE *pm = &blackmatrix[0][0];
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				SETVAL2(j*POSFIDX(width) + POSFIDX(i), 0, 0, 0);
			}
		}
		for (int i = 0; i < (int) pri_1_left.size(); i++) {
			SETVAL2((height - pri_1_left[i].row)*POSFIDX(width) + POSFIDX(pri_1_left[i].idx), 0, 255, 0);	
		}
		for (int i = 0; i < (int) pri_1_right.size(); i++) {
			SETVAL2((height - pri_1_right[i].row)*POSFIDX(width) + POSFIDX(pri_1_right[i].idx), 0, 0, 255);	
		}
		SaveBitmapToFile( (BYTE*) pm, width, height, 24, 0, "black_image.bmp" ); 
	
	}
				
	pass2(true, pri_1_left, pri_2_left, width, height, pmatrix);
	pass2(false, pri_1_right, pri_2_right, width, height, pmatrix);
	
	if (doTest && doVerbose) {
		printf("pri_2_left\n");
		dumppri(pri_2_left, width, height);

		printf("pri_2_right\n");
		dumppri(pri_2_right, width, height);
	}
		
	int foundi = -1;
	int count = 0;	
	int row1 = 0;
	if (!pri_2_right.empty()) {
		row1 = pri_2_right[0].row;
		for (int i = 1; i < (int) pri_2_right.size(); i++) {
			int row2 = pri_2_right[i].row;
			if (((row1 - row2) <= 3) && ((row1 - row2) >= 0)) {
				row1 = row2;
				count++;
				//printf("i = %d row1 = %d row2 = %d count = %d\n", i, row1, row2, count);
				if (count == 5) {
					foundi = i;
					break;
				}
			} else {
				row1 = row2;
				count = 0;
			}
		}
	}
	if (foundi  != -1) {
		fs.brightmarker = true;
		/* Note to self:
		 * Formula derived by way of similar triangles
		 *    P[f](i,r)
		 * 
		 * 
		 *           P[f-5](i,r)             
		 *
		 *
		 *                   (x,height)
		 * 
		 * (P[f-5].r - P[f].r)/(P[f-5].i - P[f].i) = (height - P[f].r)/(x-P[f].i)
		 * (P[f-5].r - P[f].r)*(x-P[f].i) = (P[f-5].i - P[f].i)*(height - P[f].r)
		 * x-P[f].i = (P[f-5].i - P[f].i)*(height - P[f].r)/(P[f-5].r - P[f].r)
		 * x = P[f].i + (P[f-5].i - P[f].i)*(height - P[f].r)/(P[f-5].r - P[f].r)
		 *
		 */
		
		//fs.rightmarker = pri_2_right[foundi-5].idx + (pri_2_right[foundi].idx - pri_2_right[foundi-5].idx)*(height - pri_2_right[foundi-5].row)/(pri_2_right[foundi].row-pri_2_right[foundi-5].row);
		fs.rightmarker = pri_2_right[foundi].idx + (pri_2_right[foundi-5].idx - pri_2_right[foundi].idx)*(height - pri_2_right[foundi].row)/(pri_2_right[foundi-5].row-pri_2_right[foundi].row);
		
		if (doVerbose) {
			printf("right: x,y = %d,%d x,y = %d,%d  rise:run %d:%d = (h(%d)-%d):(x-%d)\n",
			pri_2_right[foundi].idx,
			pri_2_right[foundi].row,
			pri_2_right[foundi-5].idx,
			pri_2_right[foundi-5].row,
			pri_2_right[foundi-5].row - pri_2_right[foundi].row,
			pri_2_right[foundi-5].idx - pri_2_right[foundi].idx,
			height,
			pri_2_right[foundi].row,
			pri_2_right[foundi].idx);	
			printf("right: x = %d\n", fs.rightmarker);		
		}
    
		// mark yellow the 2 endpoint pixels used
		SETVAL((height - pri_2_right[foundi-5].row)*POSFIDX(width) + POSFIDX(pri_2_right[foundi-5].idx), 0, 255, 255);
		SETVAL((height - pri_2_right[foundi].row)*POSFIDX(width) + POSFIDX(pri_2_right[foundi].idx), 0, 255, 255);
	} else
	fs.brightmarker = false;
	
	if (!pri_2_left.empty()) {
		row1 = pri_2_left[0].row;
		count = 0;
		foundi = -1;
		for (int i = 1; i < (int) pri_2_left.size(); i++) {
			int row2 = pri_2_left[i].row;
			if (((row1 - row2) <= 3) && ((row1 - row2) >= 0)) {
				row1 = row2;
				count++;
				if (count == 5) {
					foundi = i;
					break;
				}
			} else {
				row1 = row2;
				count = 0;
			}
		}
	}

	if (foundi  != -1) {
		fs.bleftmarker = true;
		fs.leftmarker = pri_2_left[foundi-5].idx + (pri_2_left[foundi].idx - pri_2_left[foundi-5].idx)*(height - pri_2_left[foundi-5].row)/(pri_2_left[foundi].row-pri_2_left[foundi-5].row);
		if (doVerbose) {
			printf("left: x,y = %d,%d x,y = %d,%d  rise:run %d:%d = (h(%d)-%d):(x-%d)\n",
			pri_2_left[foundi].idx,
			pri_2_left[foundi].row,
			pri_2_left[foundi-5].idx,
			pri_2_left[foundi-5].row,
			pri_2_left[foundi-5].row - pri_2_left[foundi].row,
			pri_2_left[foundi-5].idx - pri_2_left[foundi].idx,
			height,
			pri_2_left[foundi].row,
			pri_2_left[foundi].idx);	
			printf("left: x = %d\n", fs.leftmarker);		
		}

		// mark yellow the 2 endpoints used
		SETVAL((height - pri_2_left[foundi].row)*POSFIDX(width) + POSFIDX(pri_2_left[foundi].idx), 0, 255, 255);
		SETVAL((height - pri_2_left[foundi - 5].row)*POSFIDX(width) + POSFIDX(pri_2_left[foundi - 5].idx), 2, 255, 255);
	} else
		fs.bleftmarker = false;

	if (doTest) {
		SaveBitmapToFile( (BYTE*) pmatrix, width, height, 24, 0, "pri_image.bmp" ); 
		// want to save black image with only red pixels from pass 1
	}	

	if (bFirstCall) {
		bFirstCall = false;
		SaveBitmapToFile( (BYTE*) pmatrix, width, height, 24, 0, "first.bmp" ); 
	}
	if (doDumpAction) {  // save image in case it results in an action
		//char buf[256];
		//sprintf(buf, "Action_%d.bmp", BmpNum);
		//SaveBitmapToFile( (BYTE*) pmatrix, width, height, 24, 0, buf);
		imageWidth = width;
		imageHeight = height; // this is dumb!
		memcpy(savedata, pmatrix, POSFIDX(width)*height);
	}

	if (fs.bleftmarker && fs.brightmarker)
		fs.markerswidth = fs.rightmarker - fs.leftmarker;
	fs.carinfront = false;
	fs.cardistance = 0;
	fs.carapproachrate = 0;
	
	if (!pri_2_right.empty())
		pri_2_right.clear();
	if (!pri_2_left.empty())
		pri_2_left.clear();
	if (!pri_1_right.empty())
		pri_1_right.clear();
	if (!pri_1_left.empty())
		pri_1_left.clear();		
	return fs.bleftmarker || fs.brightmarker;
}

void DumpRequest()
{
	char buf[256];
	sprintf(buf, "/home/pi/usbdrv/projects/camera_gpio/Action_%d.bmp", BmpNum);
	SaveBitmapToFile( (BYTE*) savedata, imageWidth, imageHeight, 24, 0, buf);
}

int StreetRow(BYTE* pmatrix, int width, int height)
{
	unsigned long rowavgval[height];
	for (int y = 0; y < height; y++) {
		unsigned long sumval = 0;
		for (int x = 0; x < width; x++) {
			unsigned long val = VAL(y*POSFIDX(width) + POSFIDX(x));
			sumval += val;
		}
		rowavgval[y] = sumval/width;
	}
	unsigned long largest = 0;
	unsigned long fortyrowavgval[height/40];
	for (int i = 0; i < height/40; i++) {
		unsigned long sumval = 0;
		for (int j = 0; j < 40; j++) {
			sumval += rowavgval[i*40 + j];
		}
		fortyrowavgval[i] = sumval/40;
		if (sumval/40 > largest) 
			largest = sumval/40;
		printf("%d: %d\n", i, (int) fortyrowavgval[i]);
	}
	return 0;
}


bool SetActionFStatsAndState(Action &a, FrameStats &fs, State &s, int width)
{
	const int PIXELSMOVE_THRESHHOLD = 5;
	a.aa = NOTHING;
	
	if (fs.bleftmarker && fs.brightmarker) {
		int leftside = width/2 - fs.leftmarker;
		int rightside = fs.rightmarker - width/2;
		if (doVerbose)
			printf("leftside = %d rightside = %d\n", leftside, rightside);
		if (leftside > rightside) {
		// go left?
			if ((leftside - rightside) > PIXELSMOVE_THRESHHOLD ) {
				if (doVerbose) {
					 printf("go left\n");
					 a.bplaysound = true;
				}
				a.aa |= LEFT;
			}
		} else if ((rightside - leftside) > PIXELSMOVE_THRESHHOLD) {
		// go right
			if (doVerbose) {
				printf("go right\n");
				a.bplaysound = true;
			}
			a.aa |= RIGHT;
		}
		// todo: implement decision to start
		// a.aa |= START;

		// todo:: implement decistion to stop
		// a.aa |= STOP;
		return true;
	} else if (doVerbose) {
		printf("bleftmarker = %d brightmarker = %d\n", fs.bleftmarker, fs.brightmarker);
	}
	return false;
}










