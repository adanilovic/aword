#include <fstream>
#include <iostream>
#include <cstring>
#include <string.h>
#include <string>
#include <stdint.h>
#include <vector>

#include "Board.hpp"

Board myCurrentBoard;
Board myFutureBoard;

void Print_Supported_Commands()
{
    std::cout << "Supported Commands are:"                          << std::endl;
    std::cout << "\t help (displays this output)"                   << std::endl;
    std::cout << "\t load <Path To Board File> "                    << std::endl;
    std::cout << "\t save <Path To Board File> "                    << std::endl;
    std::cout << "\t display"                                       << std::endl;
    std::cout << "\t addl <r><c><letter>"                           << std::endl;
    std::cout << "\t addw <r><c><direction (0=down, 1=right)><word>"<< std::endl;
    std::cout << "\t bag <letters>"                                 << std::endl;
    std::cout << "\t count_bag"                                     << std::endl;
    std::cout << "\t end_turn"                                      << std::endl;
    std::cout << "\t testp"                                         << std::endl;
    std::cout << "\t exit"                                          << std::endl;
}

void Execute_Command( std::string CommandLine )
{
    std::cout << "CommandLine = " << CommandLine << std::endl;

    if( strncmp(CommandLine.c_str(), "help", 4 ) == 0 )
    {
        Print_Supported_Commands();
    }
    else if( strncmp(CommandLine.c_str(), "load", 4 ) == 0 )
    {
        char buf[100];
        char *filePath;
        memset(buf, 0, sizeof (buf ) );
        snprintf( buf, sizeof( buf ), "%s", CommandLine.c_str() );
        filePath = strtok( buf, " " );
        filePath = strtok( NULL, " " );
        printf("filePath = %s\n", filePath);

        myFutureBoard.Parse_Board( filePath );
        myCurrentBoard = myFutureBoard;
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

        myCurrentBoard.Serialize( filePath );
    }
    else if( CommandLine == "display" )
    {
        myFutureBoard.Display();
        myCurrentBoard.Display();
    }
    else if( strncmp(CommandLine.c_str(), "addl", 4 ) == 0 )
    {
        uint32_t r = 0;
        uint32_t c = 0;
        char l = 0;
        
        sscanf( CommandLine.c_str(), "addl %d %d %c", &r, &c, &l );

        myFutureBoard.Add_Letter( r, c, l );
    }
    else if( strncmp(CommandLine.c_str(), "addw", 4 ) == 0 )
    {
        uint32_t r = 0;
        uint32_t c = 0;
        uint32_t d = 0;
        char buf[256];
        memset( buf, 0, sizeof( buf) );
        
        sscanf( CommandLine.c_str(), "addw %d %d %d %s", &r, &c, &d, buf );

        myFutureBoard.Add_Word( r, c, d, buf );
    }
    else if( strncmp(CommandLine.c_str(), "bag", 3 ) == 0 )
    {
        char buf[256];
        memset( buf, 0, sizeof( buf ) );

        sscanf( CommandLine.c_str(), "bag %s", buf );

        myFutureBoard.Set_TileBag( buf );
        myCurrentBoard.Set_TileBag( buf );
    }
    else if( strncmp(CommandLine.c_str(), "count_bag", 9 ) == 0 )
    {
        myFutureBoard.countPossibleWordsFromBag();
    }
    else if( strncmp(CommandLine.c_str(), "end_turn", 8 ) == 0 )
    {
        if( !myFutureBoard.isCurrentWordAWord( ) )
        {
            std::cout << "ERROR: Current word is unacceptable!" << std::endl;
            myFutureBoard.deleteCurrentWord();
            myFutureBoard = myCurrentBoard;
        }
        else
        {
            std::cout << "Word is Accepted!" << std::endl;
            myFutureBoard.deleteCurrentWord();
            myCurrentBoard = myFutureBoard;
        }
    }
    else if( strncmp(CommandLine.c_str(), "testp", 5 ) == 0 )
    {
        std::string myS = "abcd";

        uint32_t n = myS.length();

        //ex: abcd
        //a, b, c, d
        //
        //ab, ac, ad
        //ba, bc, bd
        //ca, cb, cd
        //da, db, dc
        //
        //abc, abd
        //acb, acd
        //abd, adc
        //
        //abcd, abdc 
        //acbd, acdb
        //adbc, adcb

        //abcd          bacd
        //abdc          badc

        //bcd           acd
        //bdc           adc

        //cd            cd
        //dc            dc

        //Heap's algorithm
        abc
        n = 3
        i = 0
        
            n = 2
            i = 0

                n = 1
                        abc
            
            swap( A[0], A[1] ) = bac

            n = 2
            i = 1
        
                n = 1    


        //Steinhaus-Johnson-Trotter algorithm
        abcd

        Step 1
            d           c           b           a

        Step 2
            cd          bc          ab          ad?
            dc          cb          ba

        Step 3

            cdb         bca         acd?
            cbd         bac
            bcd         abc
            
            bdc         acb
            dbc         cab
            dcb         cba

        Step 4

            cdba
            cdab
            cadb
            acdb

            acbd
            cabd
            cbad
            cbda
           
            bcda
            bcad
            bacd
            abcd

            ... etc
        //
        abcd
        
        Step 1
            a                           b                   c               d

        Step 2
            ab      ac      ad          bc      bd          cd
            ba      ca      da          cb      db          dc

        Step 3
            abc     acd                 bcd     bda
            acb     adc                 bdc     bad
            cab     dac                 dbc     abd
            cba     dca                 dcb     adb
            bca     cda                 cdb     dab
            bac     cad                 cbd     dba
        //-------------------------------------------------------------------------
        abcd
        
        Step 1
            a                           b                   c               d

    left:   b, c, d                     A, c, d             A, B, d         A, B, C

        Step 2
            ab      ac      ad          bc      bd          cd
            ba      ca      da          cb      db          dc

    left:   c, d    B, d    B, C        A, d    A, C        A, B

        Step 3
            abc     acd                 bcd
            acb     adc                 bdc
            cab     dac                 dbc
            cba     dca                 dcb
            bca     cda                 cdb
            bac     cad                 cbd

            abd
            adb
            dab
            dba
            bda
            bad

        //
        uint32_t myNumPerm = 0;

        for( uint32_t i = 0; i < n; ++i )
        {
            for(  )
            {

            }
        }

    }
    else if( CommandLine == "exit" )
    {
        //do nothing, handled by caller
    }
    else
    {
        std::cout << "ERROR: Unrecognized Command" << std::endl;
        Print_Supported_Commands();
    }
}

int main(int argc, char **argv)
{
    int32_t ret = 0;

    if( argc != 1 )
    {
        std::cout << "ERROR: Usage is: " << argv[0] << std::endl;
        return -1;
    }

    std::cout << "aword" << std::endl;

    Print_Supported_Commands();
   
    std::string CommandLine;

    ret = LoadEnglishWordList();
    if( ret ) { return ret; }
    
    while( CommandLine != "exit" )
    {
        getline( std::cin, CommandLine );

        Execute_Command( CommandLine );
    }

    CloseEnglishWordList();

	return 0;
}
