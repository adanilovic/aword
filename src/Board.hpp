#ifndef BOARD_HPP
#define BOARD_HPP

#define ELEM_COUNT( t ) ( sizeof( t ) / sizeof( t[0] ) )

#define TILE_TOP_BOTTOM '-'
#define TILE_DELIMITER  '|'
#define TILE_EMPTY      ' '

class Tile;
class Board;
extern Board myBoard;
extern int32_t LoadEnglishWordList();
extern void CloseEnglishWordList();

enum Multiplier
{
    NONE,
    DL,
    DW,
    TL,
    TW,
    NUM_MULTIPLIER_TYPES
};

struct letter_score
{
    char l;
    uint32_t score;
};


class Tile
{
public:
    Tile();
    void Display();
    void set_letter( char arg ) { letter = arg; }
    char get_letter() { return letter; }
    bool has_latter() { return ( get_letter() != TILE_EMPTY ); }
    uint32_t get_score() { return score; }
    void Serialize( std::ofstream &board_file );

    void set_multiplier( enum Multiplier arg ) { mul = arg; }
    enum Multiplier get_multiplier() { return mul; }

private:
    char letter;
    enum Multiplier mul;
    uint32_t score;
};

class Board
{
public:
    Board();
    void Add_Row();
    void Add_Tile_To_Current_Row( );
    void Add_Multiplier_To_Current_Tile( enum Multiplier mu );
    void Add_Letter_To_Current_Tile( char l );
    void Add_Letter( uint32_t r, uint32_t c, char l );
    int32_t Parse_Board( std::string board_file_name );
    void Display();
    uint32_t ComputeScore();
    bool isCurrentWordAWord( );
    void deleteCurrentWord() { current_word.clear(); }
    int32_t Serialize( std::string file_path );

private:
    std::vector< std::vector< Tile > > board;
    std::string current_word;
};

#endif // #ifndef BOARD_HPP
