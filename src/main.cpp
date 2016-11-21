#include <fstream>
#include <iostream>
#include <cstring>
#include <string.h>
#include <string>
#include <stdint.h>
#include <vector>

#include "Board.hpp"

void Print_Supported_Commands()
{
    std::cout << "Supported Commands are:"       << std::endl;
    std::cout << "\t load <Path To Board File> " << std::endl;
    std::cout << "\t save <Path To Board File> " << std::endl;
    std::cout << "\t display"                    << std::endl;
    std::cout << "\t addl <r><c><letter>"        << std::endl;
    std::cout << "\t exit"                       << std::endl;
}

void Add_Letter( uint32_t r, uint32_t c, char l )
{
    myBoard.Add_Letter(r, c, l);
}

void Execute_Command( std::string CommandLine )
{
    std::cout << "CommandLine = " << CommandLine << std::endl;

    if( strncmp(CommandLine.c_str(), "load", 4 ) == 0 )
    {
        char buf[100];
        char *filePath;
        memset(buf, 0, sizeof (buf ) );
        snprintf( buf, sizeof( buf ), "%s", CommandLine.c_str() );
        filePath = strtok( buf, " " );
        filePath = strtok( NULL, " " );
        printf("filePath = %s\n", filePath);

        myBoard.Parse_Board( filePath );
    }
    else if( strncmp(CommandLine.c_str(), "save", 4 ) == 0 )
    {
        char buf[100];
        char *filePath;
        memset(buf, 0, sizeof (buf ) );
        snprintf( buf, sizeof( buf ), "%s", CommandLine.c_str() );
        filePath = strtok( buf, " " );
        filePath = strtok( NULL, " " );
        printf("filePath = %s\n", filePath);

        myBoard.Serialize( filePath );
    }
    else if( CommandLine == "display" )
    {
        myBoard.Display();
    }
    else if( strncmp(CommandLine.c_str(), "addl", 4 ) == 0 )
    {
        uint32_t r = 0;
        uint32_t c = 0;
        char l = 0;
        
        sscanf( CommandLine.c_str(), "addl %d %d %c", &r, &c, &l );

        myBoard.Add_Letter( r, c, l );
    }
    else if( CommandLine == "exit" )
    {
        //do nothing, handled by caller
    }
    else
    {
        std::cout << "ERROR: Unrecognized Command" << std::endl;
    }
}

int main(int argc, char **argv)
{
    if( argc != 1 )
    {
        std::cout << "ERROR: Usage is: " << argv[0] << std::endl;
        return -1;
    }

    std::cout << "aword" << std::endl;

    Print_Supported_Commands();
   
    std::string CommandLine;

    while( CommandLine != "exit" )
    {
        getline( std::cin, CommandLine );

        Execute_Command( CommandLine );
    }

	return 0;
}
