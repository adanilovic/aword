#include <string>
#include <stdint.h>
#include <string>
#include <cstring>
#include <vector>
#include <fstream>
#include <iostream>
#include <iomanip>

#include "Board.hpp"
#include "myMath.hpp"

static std::string path_to_english_word_list = "/usr/share/dict/american-english";
static std::ifstream english_word_list_file;

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

int32_t LoadEnglishWordList( void )
{
    int32_t ret = 0;

    english_word_list_file.open( path_to_english_word_list.c_str(), std::ifstream::in );

    if( english_word_list_file.is_open() )
    {
        //success
    }
    else
    {
        ret = -1;
        std::cout << "ERROR: LoadEnglishWordList, error loading english word list file" << std::endl;
    }

    return ret;
}

void CloseEnglishWordList( void )
{
    if( english_word_list_file.is_open() )
    {
        english_word_list_file.close();
    }
}

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

Tile::Tile( const Tile &t )
{
    letter = t.letter;
    mul = t.mul;
    score = t.score;
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

void Board::Display( )
{
    if( board.size() )
    {
        std::cout << "Board::Display(), size of board is "  << board.size() << " x " << board.back().size() << std::endl;
        std::cout << "Board::Display(), current_word = "    << current_word << std::endl;
        std::cout << "Board::Display(), current_tileBag = " << current_tileBag << std::endl;

        //std::cout << "----------------------------------------------" << std::endl;

        //std::cout << std::setw( offset );

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

uint32_t Board::getNumCol()
{
    if( board.size() == 0 )
    {
        return 0;
    }
    else
    {
        //assume all rows are the same length
        return board.back().size();
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
    //TODO: Add boundary checks
    board[r][c].set_letter( l );
    current_word.push_back( l );
}

void Board::Add_Word( uint32_t r, uint32_t c, uint32_t d, std::string word )
{
    if(    ( r > board.size() )
        || ( c > board.back().size() )
      )
    {
        std::cout << "ERROR: Add_Word, r " << r << " or c " << c << " outside of board dimensions" << std::endl;
        return;
    }

    if( d == 0  )   // (0=down, 1=right)
    {
        if( ( r + word.length() ) > board.size() )
        {
            std::cout << "ERROR: Add_Word, r " << r << " + word.length() " << word.length() << " outside of board dimensions" << std::endl;
            return;        
        }
    }
    else if( d == 1 )
    {
        if( ( c + word.length() ) > board.size() )
        {
            std::cout << "ERROR: Add_Word, c " << c << " + word.length() " << word.length() << " outside of board dimensions" << std::endl;
            return;        
        }
    }
    else
    {
        std::cout << "ERROR: Add_Word, illegal value for d, d = " << d << std::endl;
    }

    uint32_t offset = 0;

    //reject if the current_word will overlap with any existing letters
    for ( std::string::iterator it = word.begin(); it != word.end(); ++it)
    {
        uint32_t tmp_r = r + ( d ? 0 : offset );
        uint32_t tmp_c = c + ( d ? offset : 0 );

        if( board[ tmp_r ][ tmp_c ].get_letter() != TILE_EMPTY )
        {
            std::cout << "ERROR: Add_Word, current_word overlaps with (r, c) = (" << tmp_r << ", " << tmp_c << ")" << std::endl;
            return;
        }

        ++offset;
    }

    //TODO: reject if the current_word will form invalid English words with touching characters

    current_word = word;

    offset = 0;

    std::cout << "AD - current_word = " << current_word << std::endl;

    for ( std::string::iterator it = current_word.begin(); it != current_word.end(); ++it)
    {
        board[ r + ( d ? 0 : offset ) ][ c + ( d ? offset : 0 ) ].set_letter( *it );

        //Add_Letter( r + ( d ?      0 : offset ), 
        //            c + ( d ? offset :      0 ), 
        //            *it );

        ++offset;
    }
}

bool Board::isCurrentWordAWord( void )
{
    bool ret = false;

    if( current_word.length() == 0 )
    {
        return ret;
    }

    while (     ( english_word_list_file.good() )
            &&  ( ret == false )
          )
    {
        char line[256];
        memset( line, 0, sizeof( line ) );
        english_word_list_file.getline( line, sizeof( line ) );

        if( !english_word_list_file.eof() )
        {
            std::string tmp = line;

            if( current_word.compare( line ) == 0 )
            {
                ret = true;
            } 
        }
    }

    return ret;
}

uint32_t Board::countPossibleWordsFromBag()
{
    //n items total, and number of ways k items can be ordered
    //P(n, k) = n! / (n - k)!

    uint32_t count = 0;

    if( current_tileBag.length() == 0 )
    {
        return count;
    }

    const uint32_t n = current_tileBag.length();
    const uint32_t n_fact = factorial( n );

    uint32_t numPermutations = 0;

    for( uint32_t k = 1; k <= n; ++k )
    {
        uint32_t n_minus_k_fact = factorial( n - k );

        numPermutations += n_fact / n_minus_k_fact;
    }

    for( uint32_t i = 0; i < numPermutations; ++i )
    {
        //is();
    }

    /*while ( english_word_list_file.good() )
    {
        char line[256];
        memset( line, 0, sizeof( line ) );
        english_word_list_file.getline( line, sizeof( line ) );

        if( !english_word_list_file.eof() )
        {
            std::string tmp = line;

            if( current_word.compare( line ) == 0 )
            {
                ret = true;
            } 
        }
    }*/

    return numPermutations;
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

void Board::Set_TileBag( std::string bag )
{
    current_tileBag = bag;
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
    board_file.open( file_path.c_str(), std::ofstream::out );

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
