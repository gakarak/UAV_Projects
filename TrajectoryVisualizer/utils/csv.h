//
//  csv.h
//

#ifndef CSV_H
#define CSV_H

#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <exception>

namespace utils{
    namespace csvtools
    {

        static std::vector< std::vector<std::string> > read_csv(std::string filename)
        {
            std::ifstream input(filename);

            if (!input)
            {
                throw std::runtime_error("read_csv: file doesn't exist");
            }

            std::vector<std::vector<std::string>> result;

            std::string row;
            while ( getline(input, row) )
            {
                //init new vector of parameters
                std::stringstream row_stream(row);
                result.push_back(std::vector<std::string>());


                //split row in parameters with delimiter ','
                std::string token;
                while ( getline(row_stream, token, ',') )
                {
                    result.back().push_back(token);
                }
            }

            return result;
        }

    };
};

#endif /* csv_h */
