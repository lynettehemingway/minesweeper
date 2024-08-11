#ifndef MINESWEEPER_BUTTON_H
#define MINESWEEPER_BUTTON_H
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class Button {
public:

    Button(const sf::Texture& texture, const sf::Vector2f& pos, const sf::Vector2f& size);
    void setTexture(sf::Texture& texture);
    void setPosition(const sf::Vector2f& pos);
    bool contains(const sf::Vector2f& point) const;
    void draw(sf::RenderWindow& window) const;
    bool isMouseOverButton(const sf::Vector2f& mousePos, const sf::Sprite& button);
    bool handleClick(float mouseX, float mouseY) const;
    void setDigit(int digit);

private:
    sf::Vector2f position;
    sf::Sprite sprite;
    sf::Texture texture;

    int digit;
    sf::Sprite digitSprite;
};

class HappyFaceButton {
private:
    sf::Texture happyTexture;
    sf::Texture winTexture;
    sf::Texture loseTexture;
    sf::Sprite buttonSprite;


public:
    int game_state = 0;
    bool paused = false;
    bool leaderboard_isopen = false;
    HappyFaceButton() {

        happyTexture.loadFromFile("photos/files/images/face_happy.png");
        winTexture.loadFromFile("photos/files/images/face_win.png");
        loseTexture.loadFromFile("photos/files/images/face_lose.png");

        buttonSprite.setTexture(happyTexture);
    }

    void setPosition(float x, float y) {
        buttonSprite.setPosition(x, y);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(buttonSprite);
    }

    bool handleClick(float mouseX, float mouseY) {
        if (buttonSprite.getGlobalBounds().contains(mouseX, mouseY)) {
            game_state = 0;
            return true;
        }
        return false;
    }

    void setWinFace() {
        buttonSprite.setTexture(winTexture);
    }

    void setLoseFace() {
        buttonSprite.setTexture(loseTexture);
    }

    void setDefaultFace(){
        buttonSprite.setTexture(happyTexture);
    }
};
#include <SFML/Graphics.hpp>

class Digit {
private:
    sf::Texture texture;
    sf::Sprite sprite;
    int digit;

public:
    Digit(const std::string& digitTexturePath) : digit(0) {
        if (!texture.loadFromFile(digitTexturePath)) {
        }
        sprite.setTexture(texture);
        sprite.setTextureRect(sf::IntRect(0, 0, 21, 32));
    }

    void setPosition(const sf::Vector2f& pos) {
        sprite.setPosition(pos);
    }

    void setDigit(int newDigit) {
        if (newDigit >= 0) {
            digit = newDigit;
            sprite.setTextureRect(sf::IntRect(newDigit * 21, 0, 21, 32));
        } else {
        }
    }

    void draw(sf::RenderWindow& window) const {
        window.draw(sprite);
    }
};

#endif