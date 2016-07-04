/*
  Boykov-Kolmogorov's Graph Cut Algorithm
 


*/

#include <cstdio>
#include <cmath>

#include <Image.h>
#include <GridCut/GridGraph_2D_4C.h>  // minimal image segmentation code based on GridCut solver for 2D images

#define K 1000
#define SIGMA2 0.012f
#define WEIGHT(A) (short)(1+K*std::exp((-(A)*(A)/SIGMA2)))

#define RED  RGB(1,0,0)
#define BLUE RGB(0,0,1)
#define WHITE RGB(1,1,1)
#define BLACK RGB(0,0,0)

// the image slice and the scribble(mask ) are the input arguments
int main(int argc,char** argv)
{
	
  typedef GridGraph_2D_4C<short,short,int> Grid;
  for (int index = 1; index < 21; index++)
  {
	  // The induvidual slices of the images are read and compared to the mask in every iteration of the for loop
	  std::string filename = "image3slices_" + std::to_string(index) + ".png";
	   
	  const Image<float> image = imread<float>(filename);
	  const Image<RGB> scribbles = imread<RGB>("image3scribble.png");

	  const int width = image.width();
	  const int height = image.height();

	  Grid* grid = new Grid(width, height);

	  for (int y = 0; y < height; y++)
	  {
		  for (int x = 0; x < width; x++)
		  {
			  grid->set_terminal_cap(grid->node_id(x, y),
				  scribbles(x, y) == BLUE ? K : 0,// Red color is the Region of Interest known by the Mask
				  scribbles(x, y) == RED ? K : 0); // Blue is the unwanted region in the segmentation as per the mask

			  if (x < width - 1)
			  {
				  const short cap = WEIGHT(image(x, y) - image(x + 1, y));

				  grid->set_neighbor_cap(grid->node_id(x, y), +1, 0, cap);
				  grid->set_neighbor_cap(grid->node_id(x + 1, y), -1, 0, cap);
			  }

			  if (y < height - 1)
			  {
				  const short cap = WEIGHT(image(x, y) - image(x, y + 1));

				  grid->set_neighbor_cap(grid->node_id(x, y), 0, +1, cap);
				  grid->set_neighbor_cap(grid->node_id(x, y + 1), 0, -1, cap);
			  }
		  }
	  }


	  grid->compute_maxflow();


	  Image<RGB> output(width, height);

	  for (int y = 0; y < height; y++)
	  {
		  for (int x = 0; x < width; x++)
		  {
			  output(x, y) = image(x, y)*(grid->get_segment(grid->node_id(x, y)) ? RED : BLUE);
			  output(x, y) =             (grid->get_segment(grid->node_id(x, y)) ? WHITE: BLACK);// Output image in Binary
		  }
	  }

	  delete grid;
	  
	  std::string outFilename = "out3binary_" + std::to_string(index) + ".png";
	  imwrite(output, outFilename);
	  	  
  }
  return 0;
}
