#include "button.h"


Button::Button(const sf::Texture& texture, const sf::Vector2f& pos, const sf::Vector2f& size)
        : position(pos) {
    sprite.setTexture(texture);
    sprite.setPosition(position);
    sprite.setScale(size.x / texture.getSize().x, size.y / texture.getSize().y);
}

void Button::setPosition(const sf::Vector2f& pos) {
    position = pos;
    sprite.setPosition(position);
}

void Button::setTexture(sf::Texture& texture) {
    sprite.setTexture(texture);
}

void Button::draw(sf::RenderWindow& window) const {
    window.draw(sprite);
    window.draw(digitSprite);
}

void Button::setDigit(int digit) {
    this->digit = digit;
    digitSprite.setTextureRect(sf::IntRect(digit * 21, 0, 21, 32));
}

bool Button::contains(const sf::Vector2f& point) const {
    return sprite.getGlobalBounds().contains(point);
}


bool Button::isMouseOverButton(const sf::Vector2f& mousePos, const sf::Sprite& button) {
    sf::FloatRect bounds = button.getGlobalBounds();
    return bounds.contains(mousePos);
}

bool Button::handleClick(float mouseX, float mouseY) const {
    if (sprite.getGlobalBounds().contains(mouseX, mouseY)) {
        return true;
    }
    return false;
}
