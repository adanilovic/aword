#include <string>
#include <stdint.h>
#include <string>
#include <cstring>
#include <vector>
#include <fstream>
#include <iostream>

#include "Board.hpp"

Board myBoard;

constexpr char multiplier_chars[4] = 
{
    'D',
    'T',
    'W',
    'L'
};

struct letter_score values[] = 
{
    {'a', 1,},
    {'b', 4,},
    {'c', 4,},
    {'d', 2,},
    {'e', 1,},
    {'f', 4,},
    {'g', 3,},
    {'h', 3,},
    {'i', 1,},
    {'j', 10,},
    {'k', 5,},
    {'l', 2,},
    {'m', 4,},
    {'n', 2,},
    {'o', 1,},
    {'p', 4,},
    {'q', 10,},
    {'r', 1,},
    {'s', 1,},
    {'t', 1,},
    {'u', 2,},
    {'v', 5,},
    {'w', 4,},
    {'x', 8,},
    {'y', 3,},
    {'z', 10,}
};

enum Multiplier getMultiplierFromString( char * ms )
{
    enum Multiplier mu;

    if( strncmp( ms, "DL", 2 ) == 0 )
    {
        mu = DL;
    }
    else if( strncmp( ms, "DW", 2 ) == 0 )
    {
        mu = DW;
    }
    else if( strncmp( ms, "TL", 2 ) == 0 )
    {
        mu = TL;
    }
    else if( strncmp( ms, "TW", 2 ) == 0 )
    {
        mu = TW;
    }
    else
    {
        mu = NONE;
    }

    return mu;
}

std::string getStringFromMultiplier( enum Multiplier mu )
{
    std::string ms;

    switch( mu )
    {
        case NONE:
            ms = "  ";
            break;

        case DL:
            ms = "DL";
            break;

        case DW:
            ms = "DW";
            break;

        case TL:
            ms = "TL";
            break;

        case TW:
            ms = "TW";
            break;

        default:
            ms = "ER";
            break;            
    }

    return ms;
}

Tile::Tile( void ) : letter( TILE_EMPTY ),
                     mul(NONE),
                     score(0)
{

}

void Tile::Display()
{
    std::cout << letter << " " << (uint32_t)mul;
    //return letter;
}

void Tile::Serialize( std::ofstream &board_file )
{
    board_file << letter << " " << getStringFromMultiplier( mul ).c_str();
}

Board::Board()
{

}

void Board::Display()
{
    if( board.size() )
    {
        std::cout << "Board::Display(), size of board is " << board.size() << " x " << board.back().size() << std::endl;

        //std::cout << "----------------------------------------------" << std::endl;

        for( uint32_t i = 0; i < board.size(); ++i )
        {
            for( uint32_t j = 0; j < board[i].size(); ++j )
            {
                board[i][j].Display();
                std::cout << "|";
            }

            //std::cout << std::endl << "----------------------------------------------" << std::endl;
            std::cout << std::endl;
        }

    }
    else
    {
        std::cout << "Board::Display(), size of board is " << board.size() << std::endl;
    }
}

void Board::Add_Row()
{
    board.emplace_back();
}

void Board::Add_Tile_To_Current_Row( )
{
    board.back().emplace_back();
}

void Board::Add_Multiplier_To_Current_Tile( enum Multiplier mu )
{
    board.back().back().set_multiplier( mu );
}

void Board::Add_Letter_To_Current_Tile( char l )
{
    board.back().back().set_letter( l );
}

void Board::Add_Letter( uint32_t r, uint32_t c, char l )
{
    board[r][c].set_letter( l );
}

uint32_t Board::ComputeScore()
{
    uint32_t score = 0;

    for( uint32_t i = 0; i < board.size(); ++i )
    {
        for( uint32_t j = 0; j < board[i].size(); ++j )
        {
            board[i][j].Display();
        }
    }

    return score;
}

int32_t Board::Parse_Board( std::string board_file_name )
{
    int32_t ret = 0;

    std::ifstream board_file;
    board_file.open( board_file_name.c_str(), std::ifstream::in );

    for( uint32_t i = 0; i < board.size(); ++i )
    {
        board[i].clear();
    }

    board.clear();

    if( board_file.is_open() )
    {
        while (     ( board_file.good() )
                &&  ( ret == 0 )
              )
        {
            //Display();

            char line[256];
            memset( line, 0, sizeof( line ) );
            board_file.getline( line, sizeof( line ) );
            if( board_file.eof() )
            {
                std::cout << "Parse_Board(), entire board read in" << std::endl;                
            }
            else 
            {
                if( !( board_file.good() ) )
                {
                    std::cout << "ERROR: Parse_Board(), error reading line" << std::endl;
                    ret = -2;
                }

                bool skip_line = false;
                bool multiplier_start = false;
                bool tile_start = false;
                bool row_start = false;
                char multiplier[2];
                memset( multiplier, 0, sizeof( multiplier ) );

                //std::cout << "AD - Parse_Board, line = " << line << std::endl;

                for( uint32_t i = 0; 
                                        ( i < ELEM_COUNT( line ) )
                                     && ( ret == 0 ) 
                                     && ( !skip_line ); 
                     ++i )
                {                
                    //std::cout << "AD - Parse_Board, line[" << i << "] = " << line[i] << std::endl;
                    //printf("AD - Parse_Board, line[%02x] = %c %08x\n", i, line[i], line[i]);

                    switch ( line[i] )
                    {
                        case TILE_TOP_BOTTOM:
                            //std::cout << "AD - Parse_Board, TILE_TOP_BOTTOM" << std::endl;
                            skip_line = true;
                            row_start = false;
                            break;

                        case TILE_DELIMITER:
                            //std::cout << "AD - Parse_Board, TILE_DELIMITER" << std::endl;

                            if( !row_start )
                            {
                                Add_Row();
                            }

                            multiplier_start = false;
                            tile_start = false;
                            row_start = true;
                            break;

                        case TILE_EMPTY:
                            //std::cout << "AD - Parse_Board, TILE_EMPTY" << std::endl;
                            if( !tile_start )
                            {
                                Add_Tile_To_Current_Row();
                            }

                            tile_start = true;
                            break;
                
                        case 'a':
                        case 'b':
                        case 'c':
                        case 'd':
                        case 'e':
                        case 'f':
                        case 'g':
                        case 'h':
                        case 'i':
                        case 'j':
                        case 'k':
                        case 'l':
                        case 'm':
                        case 'n':
                        case 'o':
                        case 'p':
                        case 'q':
                        case 'r':
                        case 's':
                        case 't':
                        case 'u':
                        case 'v':
                        case 'w':
                        case 'x':
                        case 'y':
                        case 'z':

                            if( !tile_start )
                            {
                                Add_Tile_To_Current_Row();
                            }

                            tile_start = true;

                            Add_Letter_To_Current_Tile( line[i] );

                            break;

                        case multiplier_chars[0]:
                        case multiplier_chars[1]:
                        case multiplier_chars[2]:
                        case multiplier_chars[3]:
                            //std::cout << "AD - Parse_Board, multiplier_chars" << std::endl;
                        
                            if( !tile_start )
                            {
                                Add_Tile_To_Current_Row();
                            }

                            tile_start = true;                       

                            if( multiplier_start )
                            {
                                multiplier[1] = line[i];
                                Add_Multiplier_To_Current_Tile( getMultiplierFromString( multiplier ) );
                                multiplier_start = false;                             
                            }
                            else
                            {   
                                multiplier[0] = line[i];
                                multiplier_start = true;
                            }
                            break;

                        case '\n':
                        case '\r':
                        case '\0':
                            //std::cout << "AD - Parse_Board, end of line" << std::endl;
                            skip_line = true;
                            break;

                        default:
                            std::cout << "ERROR: Parse_Board(), unexpected character hit when parsing a board" << std::endl;
                            ret = -3;
                            break;
                    }
                }
            }            
        }            
        
        board_file.close();
    }
    else
    {
        std::cout << "ERROR: Could not open file" << std::endl;
        ret = -1;
    }

    return ret;
}

int32_t Board::Serialize( std::string file_path )
{
    int32_t ret = 0;

    std::ofstream board_file;
    board_file.open( file_path.c_str(), std::ifstream::out );

    if( board_file.is_open() )
    {
        for( uint32_t i = 0; i < board.size(); ++i )
        {
            board_file << "----------------------------------------------------------------------------" << std::endl;

            board_file << "|";

            for( uint32_t j = 0; j < board[i].size(); ++j )
            {
                board[i][j].Serialize( board_file );
                board_file << "|";

            }
        
            board_file << std::endl;
        }

        board_file << "----------------------------------------------------------------------------" << std::endl;

        board_file.close();
    }
    else
    {
        std::cout << "ERROR: Could not open file" << std::endl;
        ret = -1;
    }

    return ret;
}
