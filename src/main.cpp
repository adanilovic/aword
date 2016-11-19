#include <fstream>
#include <iostream>
#include <cstring>
#include <string>
#include <stdint.h>
#include <list>

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

class Tile
{
public:
    Tile();
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


class Board
{
public:
    Board();
    void Set_Size( uint32_t a_num_rows, uint32_t a_num_cols, uint32_t a_num_row_tiles, uint32_t a_num_col_tiles )
    {
        num_rows = a_num_rows;
        num_cols = a_num_cols;
        num_rows = a_num_row_tiles;
        num_rows = a_num_col_tiles;
    }
    void Add_Col_Tile( Tile til );
    void Add_Row_Tile( Tile til );
    void Display();

private:
    list <Tile> row_tiles;
    list <Tile> col_tiles;
    uint32_t num_rows;
    uint32_t num_cols;
    uint32_t num_row_tiles;
    uint32_t num_col_tiles;
};

Board::Board() : row_tiles(NULL),
                 col_tiles(NULL),
                 num_rows(0),
                 num_cols(0),
                 num_row_tiles(0),
                 num_col_tiles(0)
{

}

int32_t Parse_Board( std::string board_file_name, Board myBoard )
{
    uint32_t ret = 0;
    bool multiplier_start = false;
    char multiplier[2];
    char line[256];
    memset( multiplier, 0, sizeof( multiplier ) );
    memset( line, 0, sizeof( line ) );

    std::ifstream board_file;
    board_file.open(board_file_name.c_str(), std::ifstream::in);

    if( board_file.is_open() )
    {
        while (     ( board_file.good() )
                &&  ( ret == 0 )
              )
        {
            memset( line, 0, sizeof( line ) );
            board_file.getline( line, sizeof( line ) );
            if( !( board_file.good() ) )
            {
                std::cout << "ERROR: Parse_Board(), error reading line" << std::endl;
                ret = -2;
            }

            bool skip_line = false;

            for( uint32_t i = 0; 
                                    ( i < ELEM_COUNT( line ) )
                                 && ( ret == 0 ) 
                                 && ( !skip_line ); 
                 ++i )
            {
                switch ( line[i] )
                {
                    case TILE_TOP_BOTTOM:
                        skip_line = true;
                        break;

                    case TILE_DELIMITER:
                        multiplier_start = false;
                        break;

                    case TILE_EMPTY:
                        break;

                    case multiplier_chars[0]:
                    case multiplier_chars[1]:
                    case multiplier_chars[2]:
                    case multiplier_chars[3]:
                    
                        if( multiplier_start )
                        {
                            multiplier[1] = line[i];
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
                        break;

                    default:
                        std::cout << "ERROR: Parse_Board(), unexpected character hit when parsing a board" << std::endl;
                        ret = -3;
                        break;
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
        std::cout << "Usage: " << argv[0] << " <path_to_board_file>" << std::endl;
        return -1;
    }

    std::cout << "<path_to_board_file> = " << argv[1] << std::endl;

    Board myBoard;
   
    Parse_Board( argv[1], myBoard ); 

	return 0;
}
