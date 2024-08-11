#include "board.h"
#include <random>
#include <iostream>

Tile::Tile() {
    hasMine = false;
    revealed = false;
    flagged = false;
    TileState new_state = TileState::Hidden;
    state = new_state;
    adjacentMines = 0;
}

TileState Tile::getState() const {
    return state;
}

void Tile::setMine(bool value) {
    hasMine = value;
}

void Tile::setAdjacentMines(int value) {
    adjacentMines = value;
}

void Tile::setFlagged(bool value) {
    flagged = value;
    if (value) state = TileState::Flagged;
    else state = TileState::Hidden;
}

bool Tile::hasaMine() const {
    return hasMine;
}

bool Tile::isRevealed() const {
    return revealed;
}

bool Tile::isFlagged() const {
    return flagged;
}

int Tile::getAdjacentMines() const {
    return adjacentMines;
}

void Tile::reveal(int& revealcount) {
    if (not revealed and not flagged){
        revealcount += 1;
    }
    revealed = true;
    state = TileState::Revealed;
    if (adjacentMines == 0 && not hasaMine()){
        for (auto a : adjacent_tiles){
            if (not a->isRevealed() && not a->isFlagged()) a->reveal(revealcount);
        }
    }
}

void Tile::loadTextures() {
    hiddenTexture.loadFromFile("photos/files/images/tile_hidden.png");
    revealedTexture.loadFromFile("photos/files/images/tile_revealed.png");
    mineTexture.loadFromFile("photos/files/images/mine.png");
    flagTexture.loadFromFile("photos/files/images/flag.png");
    for (int i = 1; i < 9; ++i) {
        numberTextures[i].loadFromFile("photos/files/images/number_" + std::to_string(i) + ".png");
    }
}

void Tile::setTexture(sf::Texture& texture) {
    sprite.setTexture(texture);
}


Board::Board(int numRows, int numCols, int numMines, TileTextures& textures, HappyFaceButton& happyface) : rows(numRows), columns(numCols), numMines(numMines) {
    this->textures = &textures;
    this->happyface = &happyface;
    for (int i = 0; i < rows; ++i) {
        std::vector<Tile*> row;
        std::vector<bool> flag_row;
        for (int j = 0; j < columns; ++j){
            Tile* new_tile = new Tile;
            row.push_back(new_tile);
            flag_row.push_back(false);
        }
        tiles.push_back(row);
        flags.push_back(flag_row);
    }

    for(int i = 0; i < numMines; ++i){
        int x_cord = rand() % (numCols - 1);
        int y_cord = rand() % (numRows - 1);

        while (tiles[y_cord][x_cord]->hasaMine()){
            x_cord = rand() % (numCols - 1);
            y_cord = rand() % (numRows - 1);
        }

        tiles[y_cord][x_cord]->setMine(true);
    }

    for (int i = 0; i < rows; ++i){
        for (int j = 0; j < columns; ++j){
            tiles[i][j]->setAdjacentMines(getAdjacentMineCount(i, j));
        }
    }
}

int Board::getFlagCount() {
    int count = 0;
    for (auto a : flags){
        for (auto b: a){
            if (b) ++count;
        }
    }
    return count;
}

void Board::draw(sf::RenderWindow& window, int tileSize){

    float tileWidth = static_cast<float>(window.getSize().x) / columns;
    float tileHeight = static_cast<float>(window.getSize().y - 100) / rows;

    sf::Sprite tileSprite;
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < columns; ++col) {
            sf::Vector2f tilePosition(col * tileWidth, row * tileHeight);

            tileSprite.setPosition(tilePosition);
            TileState state = tiles[row][col]->getState();
            if (happyface->paused || (happyface->leaderboard_isopen && happyface->game_state == 0)){
                tileSprite.setTexture(*textures->revealedTexture);
                window.draw(tileSprite);
            }
            else {
                switch (state) {
                    case TileState::Hidden:
                        tileSprite.setTexture(*textures->hiddenTexture);
                        window.draw(tileSprite);
                        if (happyface->game_state == 2) {
                            if (tiles[row][col]->hasaMine()) {
                                tileSprite.setTexture(*textures->mineTexture);
                                window.draw(tileSprite);
                            }
                        }
                        break;
                    case TileState::Revealed:
                        tileSprite.setTexture(*textures->revealedTexture);
                        window.draw(tileSprite);
                        if (tiles[row][col]->hasaMine()) {
                            tileSprite.setTexture(*textures->mineTexture);
                            window.draw(tileSprite);
                        } else if (tiles[row][col]->getAdjacentMines() > 0) {
                            tileSprite.setTexture(textures->numberTextures[tiles[row][col]->getAdjacentMines()]);
                            window.draw(tileSprite);
                        }
                        break;
                    case TileState::Flagged:
                        tileSprite.setTexture(*textures->hiddenTexture);
                        window.draw(tileSprite);
                        tileSprite.setTexture(*textures->flaggedTexture);
                        window.draw(tileSprite);
                        break;
                }
            }
        }
    }

    window.display();

}

void Tile::setState(TileState newState) {
    state = newState;
}

TileState Board::getTileState(int row, int col) const {
    if (row < 0 || row >= rows || col < 0 || col >= columns) {
        return TileState::Hidden;
    }
    return tiles[row][col]->getState();
}

bool Board::leftClick(int x, int y) {
    if (x < 0 || x >= columns || y < 0 || y >= rows || tiles[y][x]->isFlagged() || happyface->game_state == -1 || happyface->game_state == 1 || happyface->paused) {
        return false;
    }
    TileState clickedTileState = tiles[y][x]->getState();
    switch(clickedTileState) {
        case TileState::Revealed:
            break;
        default:
            if (tiles[y][x]->hasaMine()){
                revealAllMines();
                happyface->setLoseFace();
                happyface->game_state = -1;
                break;
            }
            tiles[y][x]->reveal(revealed);
            if (revealed == rows * columns - numMines) {
                happyface->setWinFace();
                happyface->game_state = 1;
                return true;
            }
            break;
    }
    return false;
}

void Board::rightClick(int x, int y) {
    if (x < 0 || x >= columns || y < 0 || y >= rows || tiles[y][x]->isRevealed() || happyface->game_state == -1 || happyface->game_state == 1 || happyface->paused) {
        return;
    }
    flags[y][x] = not flags[y][x];
    tiles[y][x]->setFlagged(not tiles[y][x]->isFlagged());
}

int Board::getAdjacentMineCount(int x, int y) const {
    static const int dx[] = { -1, 0, 1, -1, 1, -1, 0, 1 };
    static const int dy[] = { -1, -1, -1, 0, 0, 1, 1, 1 };

    int count = 0;
    for (int i = 0; i < 8; ++i) {
        int nx = x + dx[i];
        int ny = y + dy[i];
        if (nx >= 0 && nx < rows && ny >= 0 && ny < columns) {
            tiles[x][y]->adjacent_tiles.push_back(tiles[nx][ny]);
            if (tiles[nx][ny]->hasaMine()){
                ++count;
            }
        }


    }
    return count;
}

void Board::revealAllMines() {
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < columns; ++x) {
            if (tiles[y][x]->hasaMine()) {
                tiles[y][x]->setState(TileState::Revealed);
                tiles[y][x]->reveal(revealed);
            }
        }
    }
}

Board& Board::operator=(const Board& other_board){
    for (auto a : tiles){
        for (auto b : a){
            delete b;
        }
        a.clear();
    }
    tiles.clear();

    for (auto a: flags){
        a.clear();
    }
    flags.clear();

    revealed = 0;
    for (int i = 0; i < rows; ++i) {
        std::vector<Tile*> row;
        std::vector<bool> flag_row;
        for (int j = 0; j < columns; ++j){
            Tile* new_tile = new Tile;

            new_tile->setMine(other_board.tiles[i][j]->hasaMine());
            row.push_back(new_tile);
            flag_row.push_back(false);
        }
        tiles.push_back(row);
        flags.push_back(flag_row);
    }

    for (int i = 0; i < rows; ++i){
        for (int j = 0; j < columns; ++j){
            tiles[i][j]->setAdjacentMines(getAdjacentMineCount(i, j));
        }
    }

    return *this;
}

Board::~Board(){
    for (auto a : tiles){
        for (auto b : a){
            delete b;
        }
        a.clear();
    }
    tiles.clear();
}







