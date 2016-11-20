#include <fstream>
#include <iostream>
#include <cstring>
#include <string.h>
#include <string>
#include <stdint.h>
#include <vector>

#define ELEM_COUNT( t ) ( sizeof( t ) / sizeof( t[0] ) )

#define TILE_TOP_BOTTOM '-'
#define TILE_DELIMITER  '|'
#define TILE_EMPTY      ' '

constexpr char multiplier_chars[4] = 
{
    'D',
    'T',
    'W',
    'L'
};

enum Multiplier
{
    NONE,
    DL,
    DW,
    TL,
    TW,
    NUM_MULTIPLIER_TYPES
};

static enum Multiplier getMultiplierFromString( char * ms )
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

class Tile
{
public:
    Tile();
    void Display();
    void set_letter( char arg ) { letter = arg; }
    char get_letter() { return letter; }
    bool has_latter() { return ( get_letter() != TILE_EMPTY ); }

    void set_multiplier( enum Multiplier arg ) { mul = arg; }
    enum Multiplier get_multiplier() { return mul; }

private:
    char letter;
    enum Multiplier mul;
};

Tile::Tile( void ) : letter( TILE_EMPTY ),
                     mul(NONE)
{

}

void Tile::Display()
{
    std::cout << letter << " " << (uint32_t)mul;
    //return letter;
}

class Board
{
public:
    Board();
    void Add_Row();
    void Add_Tile_To_Current_Row( );
    void Add_Multiplier_To_Current_Tile( enum Multiplier mu );
    void Display();

private:
    std::vector< std::vector< Tile > > board;
};

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

int32_t Parse_Board( std::string board_file_name, Board &myBoard )
{
    uint32_t ret = 0;

    std::ifstream board_file;
    board_file.open(board_file_name.c_str(), std::ifstream::in);

    if( board_file.is_open() )
    {
        while (     ( board_file.good() )
                &&  ( ret == 0 )
              )
        {
            myBoard.Display();

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

                std::cout << "AD - Parse_Board, line = " << line << std::endl;

                for( uint32_t i = 0; 
                                        ( i < ELEM_COUNT( line ) )
                                     && ( ret == 0 ) 
                                     && ( !skip_line ); 
                     ++i )
                {                
                    //std::cout << "AD - Parse_Board, line[" << i << "] = " << line[i] << std::endl;
                    printf("AD - Parse_Board, line[%02x] = %c %08x\n", i, line[i], line[i]);

                    switch ( line[i] )
                    {
                        case TILE_TOP_BOTTOM:
                            std::cout << "AD - Parse_Board, TILE_TOP_BOTTOM" << std::endl;
                            skip_line = true;
                            row_start = false;
                            break;

                        case TILE_DELIMITER:
                            std::cout << "AD - Parse_Board, TILE_DELIMITER" << std::endl;

                            if( !row_start )
                            {
                                myBoard.Add_Row();
                            }

                            multiplier_start = false;
                            tile_start = false;
                            row_start = true;
                            break;

                        case TILE_EMPTY:
                            std::cout << "AD - Parse_Board, TILE_EMPTY" << std::endl;
                            if( !tile_start )
                            {
                                myBoard.Add_Tile_To_Current_Row();
                            }

                            tile_start = true;
                            break;

                        case multiplier_chars[0]:
                        case multiplier_chars[1]:
                        case multiplier_chars[2]:
                        case multiplier_chars[3]:
                            std::cout << "AD - Parse_Board, multiplier_chars" << std::endl;
                        
                            if( !tile_start )
                            {
                                myBoard.Add_Tile_To_Current_Row();
                            }

                            tile_start = true;                       

                            if( multiplier_start )
                            {
                                multiplier[1] = line[i];
                                myBoard.Add_Multiplier_To_Current_Tile( getMultiplierFromString( multiplier ) );
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
                            std::cout << "AD - Parse_Board, end of line" << std::endl;
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
        ret = -1;
    }

    return ret;
}

int main(int argc, char **argv)
{
    if( argc != 2 )
    {
        std::cout << "ERROR: Usage is: " << argv[0] << " <path_to_board_file>" << std::endl;
        return -1;
    }

    std::cout << "<path_to_board_file> = " << argv[1] << std::endl;

    Board myBoard;
   
    Parse_Board( argv[1], myBoard );

    myBoard.Display();

	return 0;
}
