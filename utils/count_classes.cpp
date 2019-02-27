// STD; STL
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <unordered_map>

int main(int argc, char **argv)
{
	// Open csv with annotation
	std::ifstream infile( "classes.txt" );
	std::string   line;
	std::unordered_map<std::string, int> classes;
	std::vector<int> v;

	try
	{
		// Loop over records
		while (std::getline(infile, line))
		{
			classes[ line ]++;
		}

		for ( const auto& i : classes )
		{
        	v.push_back(std::stoi(i.first));
    	}

    	sort(v.begin(), v.end());


    	for ( size_t i = 0; i < v.size(); i++ )
    	{
    		std::cout << v.at( i ) << std::endl; 
    	}
	}
	catch (std::exception& e)
	{
		std::cerr << "Anntotaion has incorrect format. ("
					+ std::string(e.what()) + ")" << std::endl;
	}

	return 0;
}