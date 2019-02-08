#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

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

enum AnnotationFormat
{
	NAME   = 0,
	CLASS  = 5
};
    
int main(int argc, char **argv)
{
	if (argc < 3)
	{
		std::cerr << "Usage: " << argv[0] << " annotation_file output_file" << std::endl;
		return 1;
	}

	// Open annotation file
	std::ifstream infile(argv[1]);
	std::ofstream outfile(argv[2]);
	std::string   line;

	try
	{
		while (std::getline(infile, line))
		{
			std::vector<std::string> vec = split(line, ';');

			if ( vec.at(CLASS) == "31" || vec.at(CLASS) == "39" || vec.at(CLASS) == "42" )
			{
				outfile << vec.at(NAME) << std::endl;
			}
		}
	}
	catch (std::exception& e)
	{
		std::cerr << "Anntotaion has incorrect format. ("
					+ std::string(e.what()) + ")" << std::endl;
	}

	return 0;
}