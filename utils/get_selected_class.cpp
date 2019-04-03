#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

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

	std::vector<int32_t> classes = { 65, 80, 31, 34, 42, 57, 82, 59, 84, 87, 81, 32, 79, 39, 76 };

	try
	{
		while (std::getline(infile, line))
		{
			std::vector<std::string> vec = split(line, ';');

			auto i = find( classes.begin(), classes.end(),  std::stoi( vec.at(CLASS) ) );

		    if ( i != classes.end() )
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