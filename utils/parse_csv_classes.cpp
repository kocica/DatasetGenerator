// STD; STL
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>


std::vector<std::string> split(const std::string &s, char delim)
{
	std::stringstream ss(s);
	std::string item;
	std::vector<std::string> elems;

	while (std::getline(ss, item, delim))
	{
		elems.push_back(std::move(item));
	}

	return elems;
}

/*
	CSV format:
	-----------
	0        1      2      3     4      5
	filename,x_from,y_from,width,height,sign_class
*/
enum AnnotationFormat
{
	NAME   = 0,
	X      = 1,
	Y      = 2,
	W      = 3,
	H      = 4,
	CLASS  = 5
};
    
int main(int argc, char **argv)
{
	// Bad args
	if (argc < 2)
	{
		std::cerr << "Usage: ./" << argv[0] << " annotation_csv" << std::endl;
		return 1;
	}

	// Open csv with annotation
	std::ifstream infile(argv[1]);
	std::string   line;
	std::unordered_map<std::string, int> classes;

	try
	{
		// Loop over records
		while (std::getline(infile, line))
		{
			// Parse comma-separated line
			std::vector<std::string> vec = split(line, ',');

			// Skip format info
			if ( vec.at(NAME) == "filename" )
			{
				continue;
			}

			classes[ vec.at(CLASS) ]++;
		}

		for ( const auto& i : classes )
		{
        	std::cout << i.first << std::endl;
    	}
	}
	catch (std::exception& e)
	{
		std::cerr << "Anntotaion has incorrect format. ("
					+ std::string(e.what()) + ")" << std::endl;
	}

	return 0;
}