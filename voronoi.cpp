#include "matrix-library/include/led-matrix.h"

#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <signal.h>
#include <vector>
#include <cstdlib>

using rgb_matrix::GPIO;
using rgb_matrix::RGBMatrix;
using rgb_matrix::Canvas;

using namespace std;

struct Color{
	int r,g,b;
};

struct Pixel{
	int x,y;
	Color c;
};

std::vector<Pixel> VoronoiDots;	

volatile bool interrupt_received = false;
static void InterruptHandler(int signo) {
  interrupt_received = true;
}

void DrawVoronoiOnMatrix(Canvas *canvas, vector<Pixel> &dots){
	for(int y=0;y<32;y++){
		for(int x=0; x<32; x++){
			float minDistance=999999;
			Color c;
			for(unsigned int i=0; i< dots.size(); i++){
				Pixel d = dots[i];
				if(((x - d.x )*(x - d.x) + (y - d.y)*(y - d.y)) < minDistance){
					minDistance = ((x-d.x)*(x-d.x) + (y-d.y)*(y-d.y));
					c = d.c;
				}
			}
			canvas->SetPixel(x,y,c.r,c.g,c.b);
		}
	}

}

static void DrawOnCanvas(Canvas *canvas) {
  /*
   * Let's create a simple animation. We use the canvas to draw
   * pixels. We wait between each step to have a slower animation.
   */
  canvas->Fill(0, 0, 255);
	
	Pixel p;
	p.x = 15; p.y = 15; p.c.r = 255; p.c.g = 0; p.c.b = 0;
	VoronoiDots.push_back(p);
	p.x = 20; p.y = 10; p.c.r = 0; p.c.g = 255; p.c.b = 0;
	VoronoiDots.push_back(p);
	p.x = 0; p.y = 0; p.c.r = 0; p.c.g = 100; p.c.b = 100;
	VoronoiDots.push_back(p);
	p.x = 16; p.y = 16; p.c.r = 0; p.c.g = 0; p.c.b = 0;
	VoronoiDots.push_back(p);
	VoronoiDots.push_back(p);
	VoronoiDots.push_back(p);
	VoronoiDots.push_back(p);
  
    while(true){
		if(interrupt_received)
			return ;
		int swap = rand() % VoronoiDots.size();
		Pixel p;
		p.x = rand() % 32; p.y = rand() % 32; p.c.r = rand() % 255; p.c.g = rand() % 255; p.c.b = rand() % 255;
		VoronoiDots[swap] = p;	
		
		DrawVoronoiOnMatrix(canvas, VoronoiDots);
		usleep(1 * 1000 * 100);  // wait a little to slow down things.
	}
}

int main(int argc, char *argv[]) {
  RGBMatrix::Options defaults;
  defaults.hardware_mapping = "regular";  // or e.g. "adafruit-hat"
  defaults.rows = 32;
  defaults.chain_length = 1;
  defaults.parallel = 1;
  defaults.show_refresh_rate = true;
  Canvas *canvas = rgb_matrix::CreateMatrixFromFlags(&argc, &argv, &defaults);
  if (canvas == NULL)
    return 1;

  // It is always good to set up a signal handler to cleanly exit when we
  // receive a CTRL-C for instance. The DrawOnCanvas() routine is looking
  // for that.
  signal(SIGTERM, InterruptHandler);
  signal(SIGINT, InterruptHandler);

  DrawOnCanvas(canvas);    // Using the canvas.

  // Animation finished. Shut down the RGB matrix.
  canvas->Clear();
  delete canvas;

  return 0;
}
