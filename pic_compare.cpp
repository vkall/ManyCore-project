
extern "C"
{
	#include "puzzle_common.h"
	#include "puzzle.h"
}

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <dirent.h>
#include <set>

#define CLOSE_RESEMBLANCE_THRESHOLD 0.12

using namespace std;

struct img_distance {
	string name;
	double distance;
};

inline bool operator<(const img_distance& lhs, const img_distance& rhs)
{
  return lhs.distance < rhs.distance;
}

void usage(void)
{
    puts("\nUsage: puzzle-diff [-o <filename>] <file> <directory>\n\n"
         "Visually compares images in the directory and returns\n"
         "the images that are closest to the reference image.\n\n"
         "-o <output filename> : print output to a file\n");
    exit(EXIT_SUCCESS);
}

bool isImage(char const *name)
{
    size_t len = strlen(name);
    // check that file extension is jpg, png or gif.
    return (len > 4 && (strcmp(name + len - 4, ".jpg") == 0 || strcmp(name + len - 4, ".png") == 0 || strcmp(name + len - 4, ".gif") == 0 ));
}

int main(int argc, char *argv[])
{

    if (argc < 3) {
        usage();

    } else {

	    char * reference_pic = argv[1];
	    char * folderPath = argv[2];

	    PuzzleContext context;
	    PuzzleCvec cvec_reference, cvec_compare;
	    double distance;
	    char comparison_pic[256];

	    int fix_for_texts = 1;

	    // Init vectors
		puzzle_init_context(&context); 
	    puzzle_init_cvec(&context, &cvec_reference);
	    puzzle_init_cvec(&context, &cvec_compare);

	    // Load reference picture
	    if (puzzle_fill_cvec_from_file(&context, &cvec_reference, reference_pic) != 0) {    
	        fprintf(stderr, "Unable to read [%s]\n", reference_pic);
	        return 1;
	    }

	    DIR *dir;
		struct dirent *fileEnt;
		
		set<img_distance> topSimiliar;
		set<img_distance> topIdentical;
	    set<img_distance>::iterator iter;
		img_distance current_image;

		if ((dir = opendir (folderPath)) != NULL) {
			
			// Loop through files in folder.
			while ((fileEnt = readdir (dir)) != NULL) {
				if (isImage(fileEnt->d_name)) {
					// Combine folder path with the image name
					strcpy(comparison_pic, folderPath);
					strcat(comparison_pic, fileEnt->d_name);

					// Create vector
				    if (puzzle_fill_cvec_from_file(&context, &cvec_compare, comparison_pic) != 0) {
				        fprintf(stderr, "Unable to read [%s]\n", comparison_pic);
				        return 1;
				    }

				    current_image.name = fileEnt->d_name;
				    current_image.distance = puzzle_vector_normalized_distance(&context, &cvec_reference, &cvec_compare, fix_for_texts);
		    		//cout << fileEnt->d_name << " dist: " << distance << "\n";

				    if (current_image.distance <= CLOSE_RESEMBLANCE_THRESHOLD) {
				    	// Identical or close resemblancetopSimiliar.insert(current_image);
			    		topIdentical.insert(current_image);
				    	if (topIdentical.size() > 10) {
				    		// More than 10, remove last.
				    		iter = topIdentical.end();
				    		--iter;
				    		topIdentical.erase(iter);
				    	}
				    } else  {
				    	// Add to top ten similar list
			    		topSimiliar.insert(current_image);
				    	if (topSimiliar.size() > 10) {
				    		// More than 10, remove last.
				    		iter = topSimiliar.end();
				    		--iter;
				    		topSimiliar.erase(iter);
				    	}
				    }
				}
			}
			closedir (dir);
		} else {
			perror ("Could not open directory");
		}

	    puzzle_free_cvec(&context, &cvec_reference);
	    puzzle_free_cvec(&context, &cvec_compare);
	    puzzle_free_context(&context);

	    cout.precision(3);

	    cout << endl << " *** Pictures found to be similar to " << reference_pic << " *** " << endl << endl;
	    for(iter=topSimiliar.begin(); iter!=topSimiliar.end();++iter) {
	    	cout << iter->distance << " " << iter->name << endl;
	    }
		
	    cout << endl << endl << " *** Pictures found to be identical/close resemblance to " << reference_pic << " *** " << endl << endl;
	    for(iter=topIdentical.begin(); iter!=topIdentical.end();++iter) {
	    	cout << iter->distance << " " << iter->name << endl;
	    }
	    cout << endl;
	    return 0;
	}
}