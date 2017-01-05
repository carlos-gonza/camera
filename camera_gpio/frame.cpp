#define FRAME
#include "frame.h"

//bool GetFrameStats(BYTE* pmatrix, int width, int height, int firstrow, FrameStats &fs)


/**
 *  findlargestval
 *  return largestval and largestvalidx for a row at y from a/3 to b/3 columns
 */
void findlargestval(BYTE* pmatrix, int width, int height, int y, int a, int b, int bufpos , int &largestval, int &largestvalidx) 
{
	int val;
	for (int x = a; x < b; x+=3) {
		val = pmatrix[bufpos + x] + pmatrix[bufpos + x + 1] + pmatrix[bufpos + x + 2];

		if (val < VAL_IGNORE) 
			continue;
		if (val > largestval) {
			largestval = val;
			largestvalidx = x/3;
		}	
	}
}

std::vector<RI> pri_1_left;	 // pass 1 left
std::vector<RI> pri_1_right; // pass 1 right
std::vector<RI> pri_2_left;  // 2nd pass left
std::vector<RI> pri_2_right; // 2nd pass right
 
 void pass1(int lb, int ub, bool bisleft, int width, int height, int y, BYTE* pmatrix)
{
	RI aRI;
	bool b = true;
	int largestval = 0;
	int largestvalidx = 0;
	long bufpos = y*3*width;
	findlargestval(pmatrix, width, height, y, lb, ub, bufpos, largestval, largestvalidx);

	if (!bisleft)
	   b = ((width - largestvalidx) > 10);
	if ((largestvalidx > 10) && (b)) {
		int expectedwidth =  10;
		int lval = pmatrix[bufpos + (largestvalidx - expectedwidth)*3] +
						pmatrix[bufpos + (largestvalidx - expectedwidth)*3 +1] +
						pmatrix[bufpos + (largestvalidx - expectedwidth)*3 + 2];
		int rval = pmatrix[bufpos + (largestvalidx + expectedwidth)*3] +
						pmatrix[bufpos + (largestvalidx + expectedwidth)*3 +1] +
						pmatrix[bufpos + (largestvalidx + expectedwidth)*3 + 2];
		int dleft = (largestval - lval);
		int dright = (largestval - rval);
					
		if ((dleft > DELTA) && (dright > DELTA) ) {
			aRI.row = height - y;
						
			// instead of picking brightest, pick pixel seeing brightest change
			int largestdiff = 0;
			int largestk = 0;
			for (int k = largestvalidx - expectedwidth; k <= largestvalidx ; k++) {
				int diff =
				pmatrix[bufpos + (k+1)*3] + pmatrix[bufpos + (k+1)*3 +1] + 	pmatrix[bufpos + (k+1)*3 + 2] -
				 (pmatrix[bufpos + k*3] + pmatrix[bufpos + k*3 +1] + pmatrix[bufpos + k*3 + 2]);
				if (diff > largestdiff) {
					largestdiff = diff;
					largestk = k;
				}							
			}						
			aRI.idx = largestk;
			
			if (bisleft)	
				pri_1_left.push_back(aRI);
			else
				pri_1_right.push_back(aRI);
					
			// mark first pass pixels red
			pmatrix[bufpos + largestk*3] = 0;
			pmatrix[bufpos + largestk*3 + 1] = 0;
			pmatrix[bufpos + largestk*3 + 2] = 255;	
			//printf("red %s y = %d idx = %d largestdiff = %d\n", bisleft ? "left" : "right", aRI.row, aRI.idx, largestdiff);
		}
	}
}  
 

 	
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
			} else if ((delta < 0) && (out.size() < 8)) {
				if (delta >= -2) // skip
					continue;
//printf("%s clear delta %d neg row = %d idx = %d size = %d\n", bisleft ? "L" : "R", delta, in[i].row, in[i].idx, out.size());							
				out.clear();
				newline = true;
			} else if (delta < 0) {
				break;
			}
		} else {
			if (out.size() >= 20) // was 8
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
		pmatrix[(height - out[i].row)*3*width + out[i].idx*3] = 255;
		pmatrix[(height - out[i].row)*3*width + out[i].idx*3 + 1] = 0;
		pmatrix[(height - out[i].row)*3*width + out[i].idx*3 + 2] = 0;
	}
}

