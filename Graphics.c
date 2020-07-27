/*
 * Graphics.c
 *
 *  Created on: Apr 17, 2020
 *      Author: Yaswanth
 */

#include "Graphics.h"
#include "DE1SoC_LT24/DE1SoC_LT24.h"
#include "HPS_Watchdog/HPS_Watchdog.h"
#include <math.h>
bool graphics_initialised = false;
//function to initialise graphics engine
signed int Graphics_initialise(unsigned int lcd_pio_base, unsigned int lcd_hw_base ){
	if(!LT24_isInitialised()){
		 LT24_initialise(lcd_pio_base,lcd_hw_base);
	}
	graphics_initialised = true;
	return GRAPHICS_SUCCESS;
}
//Check if driver initialised
bool Graphics_isInitialised() {
	return graphics_initialised;
}
//function to draw box with argument vertices as diagonal
signed int Graphics_drawBox(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned short colour, bool noFill, unsigned short fillColour){

	//Sanity checks
	if(!Graphics_isInitialised()) return GRAPHICS_ERRORNOINIT;
	//border of box
	//lines are drawn between vertices using Graphics_drawLine() function
	Graphics_drawLine(x1,y1,x1,y2,colour);
	Graphics_drawLine(x1,y2,x2,y2,colour);
	Graphics_drawLine(x2,y2,x2,y1,colour);
	Graphics_drawLine(x2,y1,x1,y1,colour);
	//if box should be filled
	if(!noFill){
		int tempx;
		int tempy;
		int y;
		int x;
		//vertices are sorted
		if(y1>y2){
			//coordinates are stored in temporary variables and reassigned
			tempy=y2;
			tempx=x2;
			y2=y1;
			x2=x1;
			y1=tempy;
			x1=tempx;
		}
		//y-coordinate is incremented
		for(y=y1+1;y<=y2-1;y++){
			//for each value of y-coordinate pixels in between (x1,y) and (x2,y)
			//are plotted
			if(x1<x2){
				for(x=x1+1;x<=x2-1;x++){
					LT24_drawPixel(fillColour,x,y);
				}
			}
			else{
				for(x=x2+1;x<=x1-1;x++){
					LT24_drawPixel(fillColour,x,y);
				}
			}
		}
	}
	return GRAPHICS_SUCCESS;
}

signed int Graphics_drawCircle(unsigned int x, unsigned int y, unsigned int r, unsigned short colour, bool noFill, unsigned short fillColour){
	int height,i;
	//Sanity checks
	if(!Graphics_isInitialised()) return GRAPHICS_ERRORNOINIT;
	if(!noFill){//circle is filled leaving border pixels
		//lines are drawn using similarity property of circle
		//points are chosen in quadrants and lines are drawn joining them
		Graphics_drawLine(x, y+r, x, y>r? y-r : 0, fillColour);
		Graphics_drawLine(x+r, y, x>r? x-r : 0, y, fillColour);
		i=1;
		height = sqrt((r*r)*1.0) + 0.5 ; //adding 0.5 will round value to nearest int
		while (i < (0.72*r)) {
			height=height-1;//to leave border pixels
			//lines are drawn parallel to diameter through center of circle with constant x-coordinate
			//lines are drawn from x=center of circle to x=center of circle+radius
			//lines are also drawn from x=center of circle+radius to x=center of circle
			Graphics_drawLine(x+i, y+height, x+i, y>height? y-height : 0, fillColour);
		    Graphics_drawLine(x>i? x-i : 0, y+height, x>i? x-i : 0, y>height? y-height : 0, fillColour);
		    Graphics_drawLine(x+height, y+i, x+height, y>i? y-i : y, fillColour);
		   	Graphics_drawLine(x>height? x-height : 0, y+i, x>height? x-height : 0, y>i? y-i : 0, fillColour);
		   	i += 1;
	        height = sqrt(((r*r)-(i*i))*1.0) + 0.5 ;
        }
	}
	//circle border
	LT24_drawPixel(colour, x, y+r);
	LT24_drawPixel(colour, x, y-r);
	LT24_drawPixel(colour, x+r, y);
	LT24_drawPixel(colour, x-r, y);
	i=1;
    height = sqrt((r*r)*1.0) + 0.5 ;
    while (i < r) {
       	//statements to start plotting points on circle
       	//with x-coordinate similar to that of center
        LT24_drawPixel(colour, x+i, y+height);
        LT24_drawPixel(colour, x+i, y-height);
        LT24_drawPixel(colour, x-i, y+height);
        LT24_drawPixel(colour, x-i, y-height);
        //statements to start plotting points on circle
      	//with y-coordinate similar to that of center
        LT24_drawPixel(colour, x+height, y+i);
        LT24_drawPixel(colour, x+height, y-i);
        LT24_drawPixel(colour, x-height, y+i);
        LT24_drawPixel(colour, x-height, y-i);
        i += 1;
        height = sqrt(((r*r)-(i*i))*1.0) + 0.5 ;
    }
    //if i=r
    LT24_drawPixel(colour, x+r, y+height);
    LT24_drawPixel(colour, x+r, y-height);
    LT24_drawPixel(colour, x-r, y+height);
    LT24_drawPixel(colour, x-r, y-height);
    return GRAPHICS_SUCCESS;
}
signed int Graphics_drawLine(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned short colour){
	unsigned int i;
	int dx=(x1-x2);
	int dy=(y1-y2);
	//Sanity checks
	if(!Graphics_isInitialised()) return GRAPHICS_ERRORNOINIT;
	//if line is more vertical
	if(abs(dy)>abs(dx)){
		if(y1<y2){
			for(i=y1;i<=y2;i++){
				LT24_drawPixel(colour,(x1+(((dx *1.0* (i-y1))/(dy))))+0.5,i);
			}
		}
		else{
			for(i=y2;i<=y1;i++){
				LT24_drawPixel(colour,(x2+(((dx *1.0* (i-y2))/(dy))))+0.5,i);
			}
		}
	}
	//if line is more horizontal or diagonal
	else {	//if(abs(dy)<=abs(dx))
		if(x1<x2){
			for(i=x1;i<=x2;i++){
				LT24_drawPixel(colour,i,(y1+(((dy *1.0* (i-x1))/(dx))))+0.5);
			}
		}
		else{
			for(i=x2;i<=x1;i++){
				LT24_drawPixel(colour,i,(y2+(((dy *1.0* (i-x2))/(dx))))+0.5);
			}
		}
	}
	return GRAPHICS_SUCCESS;
}

signed int Graphics_drawTriangle(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2,unsigned int x3,unsigned int y3,unsigned short colour,bool noFill,unsigned short fillColour){
	int xp,dx1,dx2,dy1,dy2,dx3,dy3;
	volatile double yp=0.0;
	volatile double yp1=0.0;
	volatile double yp2=0.0;
	if(!Graphics_isInitialised()) return GRAPHICS_ERRORNOINIT;
	//if triangle is to be filled
	if(!noFill){
		int y;
		int tempy;
		int tempx;
		float slope1;
		float slope2;
		float slope3;
		//sort vertices such that y1<=y2<=y3
		if(y1>y2){//y1 and y2 are compared
			tempy=y2;
			tempx=x2;
			y2=y1;
			x2=x1;
			y1=tempy;
			x1=tempx;
		}
		if(y2>y3){//y2 and are compared
			tempy=y3;
			tempx=x3;
			y3=y2;
			x3=x2;
			y2=tempy;
			x2=tempx;
		}
		if(y1>y2){//y1 and y2 are compared to finish sorting
			tempy=y2;
			tempx=x2;
			y2=y1;
			x2=x1;
			y1=tempy;
			x1=tempx;
		}
		//slopes are calculated for triangle edges
		dx1=x1-x2;
		dx2=x1-x3;
		dy1=y1-y2;
		dy2=y1-y3;
		dx3=x3-x2;
		dy3=y3-y2;
		slope1=(dy1*1.0)/dx1;
		slope2=(dy2*1.0)/dx2;
		slope3=(dy3*1.0)/dx3;
		HPS_ResetWatchdog();
		//if triangle is above line joining (x1,y1) & (x3,y3)
		if((x3<=x1 && x2<=x1 && slope1>slope2) || ((x1<=x2) && x1<=x3 && slope2<slope1) || ((x2<=x1 && x1<=x3))){
			//pixels part of triangle which lie below y2 are plotted
			y=y1;
			while(y<=y2){
				//x-coordinate on line joing (x1,y1) and (x2,y2)
				yp1=((((dx1)*((y-y1)*1.0))/(dy1)))+x1+0.5;
				//x-coordinate on line joing (x1,y1) and (x3,y3)
				yp2=((((dx2)*((y-y1)*1.0))/(dy2)))+x1+0.5;
				if(y-y1==0){
					LT24_drawPixel(fillColour,x1,y);
				}
				else{
					while(yp1<=yp2){
						LT24_drawPixel(fillColour,yp1,y);
						yp1= yp1 + 1;
					}
				}
				y = y+1;
			}
			//pixels part of triangle which lie above y2 are plotted
			y=y2;
			while(y<=y3){
				//x-coordinate on line joing (x2,y2) and (x3,y3)
				yp1=((((dx3)*((y-y2)*1.0))/(dy3)))+x2+0.5;
				//x-coordinate on line joing (x1,y1) and (x3,y3)
				yp2=((((dx2)*((y-y1)*1.0))/(dy2)))+x1+0.5;
				if(y==y2){
					yp1=x2;
				}
				while(yp1<=yp2){
					LT24_drawPixel(fillColour,yp1,y);
					yp1= yp1 + 1;
				}
				y = y+1;
			}
		}
		//if triangle is above line joining (x1,y1) & (x3,y3)
		else{
			//pixels part of triangle which lie below y2 are plotted
			y=y1;
			while(y<=y2){
				//x-coordinate on line joing (x1,y1) and (x2,y2)
				yp1=((((dx1)*((y-y1)*1.0))/(dy1)))+x1+0.5;
				//x-coordinate on line joing (x1,y1) and (x3,y3)
				yp2=((((dx2)*((y-y1)*1.0))/(dy2)))+x1+0.5;
				if(y-y1==0){
					LT24_drawPixel(fillColour,x1,y);
				}
				while(yp2<=yp1){
					LT24_drawPixel(fillColour,yp2,y);
					yp2= yp2 + 1;
				}
				y = y+1;
			}
			//pixels part of triangle which lie above y2 are plotted
			y=y2;
			while(y<=y3){
				//x-coordinate on line joing (x3,y3) and (x2,y2)
				yp1=((((dx3)*((y-y2)*1.0))/(dy3)))+x2+0.5;
				//x-coordinate on line joing (x1,y1) and (x3,y3)
				yp2=((((dx2)*((y-y1)*1.0))/(dy2)))+x1+0.5;
				if(y-y1==0){
					LT24_drawPixel(fillColour,x2,y);
				}
				while(yp2<=yp1){
					LT24_drawPixel(fillColour,yp2,y);
					yp2= yp2 + 1;
				}
				y = y+1;
			}
		}

	}
	//vertices joined by drawing lines using Graphics_drawLine() function
	Graphics_drawLine(x1,y1,x2,y2,colour);
	Graphics_drawLine(x3,y3,x2,y2,colour);
	Graphics_drawLine(x3,y3,x1,y1,colour);
	return GRAPHICS_SUCCESS;
}
signed int Graphics_drawRandom(unsigned int n, unsigned short colour){
	int i;
	int x1,x2,y1,y2;
	int firstx,firsty;
	if(!Graphics_isInitialised()) return GRAPHICS_ERRORNOINIT;
	x1=rand()%240;
	y1=rand()%320;
	//values of first point are stored
	firstx=x1;
	firsty=y1;
	//lines are continuously drawn between random points
	//n is number of edges of shape
	for(i=n-1;i!=0;i--){
		x2=rand()%240;
		y2=rand()%320;
		Graphics_drawLine(x1,y1,x2,y2,colour);
		x1=x2;
		y1=y2;
	}
	//last edge is drawn between first point and last point
	Graphics_drawLine(firstx,firsty,x2,y2,colour);
	return GRAPHICS_SUCCESS;
}

