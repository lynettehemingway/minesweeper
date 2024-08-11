#ifndef MINESWEEPER_BOARD_H
#define MINESWEEPER_BOARD_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "button.h"

enum class TileState {
    Hidden,
    Revealed,
    Flagged,
};

struct TileTextures {
    sf::Texture* hiddenTexture = new sf::Texture;


    sf::Texture* revealedTexture = new sf::Texture;


    sf::Texture* flaggedTexture = new sf::Texture;


    sf::Texture* mineTexture = new sf::Texture;


    sf::Texture* numberTextures = new sf::Texture[9];


    TileTextures(){
        hiddenTexture->loadFromFile("photos/files/images/tile_hidden.png");
        revealedTexture->loadFromFile("photos/files/images/tile_revealed.png");
        flaggedTexture->loadFromFile("photos/files/images/flag.png");
        mineTexture->loadFromFile("photos/files/images/mine.png");
        for (int i = 1; i < 9; ++i) {
            numberTextures[i].loadFromFile("photos/files/images/number_" + std::to_string(i) + ".png");
        }
    }

    ~TileTextures(){
        delete hiddenTexture;
        delete revealedTexture;
        delete flaggedTexture;
        delete mineTexture;
        delete[] numberTextures;
    }
};

class Tile {
private:
    bool hasMine;
    bool revealed;
    bool flagged;
    int adjacentMines;
    sf::Sprite sprite;
    sf::Texture hiddenTexture;
    sf::Texture revealedTexture;
    sf::Texture flagTexture;
    TileState state;

public:
    std::vector<Tile*> adjacent_tiles;
    sf::Texture mineTexture;
    sf::Texture numberTextures[9];
    Tile();
    void setState(TileState newState);
    void setMine(bool value);
    void setAdjacentMines(int value);
    void setFlagged(bool value);
    bool hasaMine() const;
    bool isRevealed() const;
    bool isFlagged() const;
    int getAdjacentMines() const;
    void reveal(int& revealcount);
    void loadTextures();
    TileState getState() const;
    void setTexture(sf::Texture& texture);
    void draw(sf::RenderWindow& window);
};


class Board {
private:
    int rows;
    TileTextures* textures;
    int columns;
    std::vector<std::vector<Tile*>> tiles;
    std::vector<std::vector<bool>> flags;
    HappyFaceButton* happyface;
    int numMines;
public:
    int revealed = 0;
    int getFlagCount();
    Board(int numRows, int numCols, int numMines, TileTextures& textures, HappyFaceButton& happyface);
    void initialize();
    void draw(sf::RenderWindow& window, int tileSize);
    TileState getTileState(int row, int col) const;
    bool leftClick(int x, int y);
    void rightClick(int x, int y);
    void revealAdjacentTiles(int x, int y);
    int getAdjacentMineCount(int x, int y) const;
    void revealAllMines();
    ~Board();
    Board& operator=(const Board& other_board);
};

#endif
