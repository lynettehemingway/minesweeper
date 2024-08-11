#include <iostream>
#include <fstream>
#include <chrono>
#include <sstream>
#include <SFML/Graphics.hpp>
#include "board.h"
#include "button.h"

void setText(sf::Text &text, float x, float y) {
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    text.setPosition(sf::Vector2f(x, y));
}


bool readConfigFile(const std::string& filename, int& columns, int& rows, int& numMines) {
    std::ifstream configFile(filename);
    if (!configFile.is_open()) {
        std::cerr << "Failed to open configuration file: " << filename << std::endl;
        return false;
    }

    std::string line;
    int lineCount = 0;
    while (std::getline(configFile, line)) {
        std::istringstream iss(line);
        if (lineCount == 0) {
            if (!(iss >> columns)) {
                std::cerr << "Invalid number of columns" << std::endl;
                return false;
            }
        } else if (lineCount == 1) {
            if (!(iss >> rows)) {
                std::cerr << "Invalid number of rows" << std::endl;
                return false;
            }
        } else if (lineCount == 2) {
            if (!(iss >> numMines)) {
                std::cerr << "Invalid number of mines" << std::endl;
                return false;
            }
        } else {
            std::cerr << "Invalid number of lines in configuration file" << std::endl;
            return false;
        }
        ++lineCount;
    }

    int maxMines = columns * rows;
    if (numMines >= maxMines) {
        std::cerr << "Invalid number of mines: exceeds total number of tiles" << std::endl;
        return false;
    }

    return true;
}

bool readLeaderboardFile(const std::string& filename, std::vector<int>& times, std::vector<std::string>& names){
    std::ifstream leaderboardfile(filename);
    if (!leaderboardfile.is_open()){
        return false;
    }
    std::string leaderboardline;
    for (int i = 0; i < 5; ++i){
        std::getline(leaderboardfile, leaderboardline);
        int time = stoi(leaderboardline.substr(0,2)) * 60 + stoi(leaderboardline.substr(3,2));
        times.push_back(time);
        names.push_back(leaderboardline.substr(7));
    }
    leaderboardfile.close();
    return true;
}

bool writeLeaderboardFile(const std::string& filename, std::vector<int>& times, std::vector<std::string>& names){
    std::ofstream leaderboardfile(filename);
    if (!leaderboardfile.is_open()) return false;
    for (int i = 0; i < 5; ++i){
        std::string writeline = "";
        if (times[i]/60 < 10) writeline += "0";
        writeline += std::to_string(times[i]/60) + ":";
        if (times[i]%60 < 10) writeline += "0";
        writeline += std::to_string(times[i]%60) + ", " + names[i];
        writeline += "\n";


        leaderboardfile << writeline;
    }
    leaderboardfile.close();
    return true;
}

std::string showWelcomeWindow(int row, int col) {
    int windowWidth = col * 32 + 100;
    int windowHeight = row * 32 + 100;
    sf::RenderWindow welcomeWindow(sf::VideoMode(windowWidth, windowHeight), "Welcome Window");

    int maxNameLength = 10;
    sf::Font font;
    if (!font.loadFromFile("photos/files/font.ttf")) {
        std::cout << "Failed to load font file!" << std::endl;
    }
    sf::Text welcome("WELCOME TO MINESWEEPER!", font, 24);
    welcome.setFillColor(sf::Color::White);
    welcome.setStyle(sf::Text::Bold | sf::Text::Underlined);
    setText(welcome, welcomeWindow.getSize().x / 2.f, welcomeWindow.getSize().y / 2.f - 150);

    sf::Text message("Enter your name:", font, 20);
    message.setFillColor(sf::Color::White);
    setText(message, welcomeWindow.getSize().x / 2.f, welcomeWindow.getSize().y / 2.f - 75);

    sf::Text inputText("", font, 18);
    inputText.setFillColor(sf::Color::Yellow);
    setText(inputText, welcomeWindow.getSize().x / 2.f, welcomeWindow.getSize().y / 2.f - 45);

    sf::Clock cursorClock;
    bool showCursor = true;
    std::string playerName;

    while (welcomeWindow.isOpen()) {
        sf::Event event;
        while (welcomeWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                welcomeWindow.close();
                return "0";
            } else if (event.type == sf::Event::TextEntered) {
                if (std::isalpha(event.text.unicode) && playerName.size() < maxNameLength) {
                    if (playerName.empty() || playerName.back() == ' ') {
                        playerName += static_cast<char>(std::toupper(event.text.unicode));
                    } else {
                        playerName += static_cast<char>(std::tolower(event.text.unicode));
                    }
                    inputText.setString(playerName);
                    setText(inputText, welcomeWindow.getSize().x / 2.f, welcomeWindow.getSize().y / 2.f - 45);
                } else if (event.text.unicode == '\b' && !playerName.empty()) {
                    playerName.pop_back();
                    inputText.setString(playerName);
                    setText(inputText, welcomeWindow.getSize().x / 2.f, welcomeWindow.getSize().y / 2.f - 45);
                } else if (event.text.unicode == '\r' || event.text.unicode == '\n') {
                    if (!playerName.empty()) {
                        welcomeWindow.close();
                    }
                }
            }
        }

        if (cursorClock.getElapsedTime().asSeconds() >= 0.5) {
            showCursor = !showCursor;
            cursorClock.restart();
        }

        welcomeWindow.clear(sf::Color::Blue);
        welcomeWindow.draw(welcome);
        welcomeWindow.draw(message);
        welcomeWindow.draw(inputText);
        if (showCursor) {
            sf::RectangleShape cursor(sf::Vector2f(2.f, 18.f));
            cursor.setPosition(inputText.getPosition().x + inputText.getLocalBounds().width/2.f, inputText.getPosition().y - inputText.getLocalBounds().height/2);
            welcomeWindow.draw(cursor);
        }
        welcomeWindow.display();
    }
    return playerName;
}

void showLeaderboardWindow(int row, int col, std::vector<int>& times, std::vector<std::string>& names, bool& isopen, int new_winner){
    int windowWidth = col * 16;
    int windowHeight = row * 16 + 50;
    sf::RenderWindow leaderboard(sf::VideoMode(windowWidth, windowHeight), "Leaderboard");
    sf::Font font;
    if (!font.loadFromFile("photos/files/font.ttf")) {
        std::cout << "Failed to load font file!" << std::endl;
    }

    std::string display_txt = "";

    for (int i = 0; i < 5; ++i){
        display_txt += std::to_string(i + 1) + ".\t";
        if (times[i]/60 < 10) display_txt += "0";
        display_txt += std::to_string(times[i]/60) + ":";
        if (times[i]%60 < 10) display_txt += "0";
        display_txt += std::to_string(times[i]%60) + "\t" + names[i];

        if (new_winner == i){
            display_txt += "*";
        }
        display_txt += "\n\n";
    }

    sf::Text welcome("LEADERBOARD", font, 20);
    welcome.setFillColor(sf::Color::White);
    welcome.setStyle(sf::Text::Bold | sf::Text::Underlined);
    setText(welcome, leaderboard.getSize().x / 2.f, leaderboard.getSize().y / 2.f - 120);

    sf::Text leaderboardtxt(display_txt, font, 18);
    leaderboardtxt.setFillColor(sf::Color::White);
    leaderboardtxt.setStyle(sf::Text::Bold);
    setText(leaderboardtxt, leaderboard.getSize().x / 2.f, leaderboard.getSize().y / 2.f + 20);


    while (leaderboard.isOpen()){
        sf::Event event1;
        while (leaderboard.pollEvent(event1)){
            if (event1.type == sf::Event::Closed){
                leaderboard.close();
                isopen = false;
                return;
            }
        }
        leaderboard.clear(sf::Color::Blue);
        leaderboard.draw(welcome);
        leaderboard.draw(leaderboardtxt);
        leaderboard.display();
    }
}

int main() {
    int columns, rows, numMines;
    if (!readConfigFile("photos/files/config.cfg", columns, rows, numMines)) {
        return 1;
    }
    std::string playername = showWelcomeWindow(rows, columns);
    if (playername == "0") return 1;

    int windowWidth = columns * 32;
    int windowHeight = rows * 32 + 100;

    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Game Window", sf::Style::Close);
    auto time1 = std::chrono::high_resolution_clock::now();
    double time_duration = 0;

    int game_time = 0;
    sf::Texture debugTexture, playTexture, leaderboardTexture, pauseTexture;
    if (!debugTexture.loadFromFile("photos/files/images/debug.png") ||
        !playTexture.loadFromFile("photos/files/images/play.png") ||
        !leaderboardTexture.loadFromFile("photos/files/images/leaderboard.png") ||
        !pauseTexture.loadFromFile("photos/files/images/pause.png")) {
        std::cerr << "Failed to load texture file!" << std::endl;
        return EXIT_FAILURE;
    }

    TileTextures textures;

    HappyFaceButton happyface;
    happyface.setPosition((columns / 2.0f * 32.0f) - 32.0f, 32.0f * (rows + 0.5));

    Board board(rows, columns, numMines, textures, happyface);
    board.draw(window, windowWidth / columns);

    sf::Vector2f buttonSize(debugTexture.getSize().x, debugTexture.getSize().y);
    sf::Vector2f buttonSize2(playTexture.getSize().x, playTexture.getSize().y);
    sf::Vector2f buttonSize3(leaderboardTexture.getSize().x, leaderboardTexture.getSize().y);
    Button debugButton(debugTexture, sf::Vector2f((columns * 32) - 304, 32 * (rows + 0.5)), buttonSize);
    Button playButton(pauseTexture, sf::Vector2f((columns * 32) - 240, 32 * (rows + 0.5)), buttonSize2);
    Button leaderboardButton(leaderboardTexture, sf::Vector2f((columns * 32) - 176, 32 * (rows + 0.5)), buttonSize3);
    Digit digit("photos/files/images/digits.png");

    std::vector<int> times;
    std::vector<std::string> names;
    int changed_pos = -1;
    readLeaderboardFile("photos/files/leaderboard.txt", times, names);

    while (window.isOpen()) {
        if (happyface.leaderboard_isopen){
            showLeaderboardWindow(rows, columns, times, names, happyface.leaderboard_isopen, changed_pos);
        }


        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::MouseButtonPressed) {
                float mouseX = sf::Mouse::getPosition(window).x;
                float mouseY = sf::Mouse::getPosition(window).y;
                if (event.mouseButton.button == sf::Mouse::Left) {
                    if (happyface.handleClick(mouseX, mouseY)) {
                        Board new_board(rows, columns, numMines, textures, happyface);
                        board = new_board;
                        happyface.setDefaultFace();
                        game_time = 0;
                        happyface.paused = false;
                        playButton.setTexture(pauseTexture);
                        happyface.game_state = 0;
                    }
                    else if (debugButton.handleClick(mouseX, mouseY)){
                        if (happyface.game_state == 0) happyface.game_state = 2;
                        else if (happyface.game_state == 2) happyface.game_state = 0;
                    }
                    else if (playButton.handleClick(mouseX, mouseY)){
                        if (happyface.paused) {
                            playButton.setTexture(pauseTexture);
                            happyface.paused = false;
                        }
                        else {
                            auto time2 = std::chrono::high_resolution_clock::now();
                            std::chrono::duration<double, std::milli> duration = time2 - time1;
                            time_duration += duration.count();

                            playButton.setTexture(playTexture);

                            happyface.paused = true;
                        }
                    }
                    else if (leaderboardButton.handleClick(mouseX, mouseY)){
                        happyface.leaderboard_isopen = not happyface.leaderboard_isopen;
                    }
                    else {

                        if(board.leftClick(mouseX/32, mouseY/32)){
                            happyface.leaderboard_isopen = true;
                            for (int i = 0; i < 5; ++i){
                                if (game_time < times[i]){
                                    times.insert(times.begin()+i, game_time);
                                    times.erase(times.end()-1);
                                    names.insert(names.begin()+i, playername);
                                    names.erase(names.end()-1);
                                    changed_pos = i;
                                    break;
                                }
                            }
                            writeLeaderboardFile("photos/files/leaderboard.txt", times, names);
                        }
                    }
                } else if (event.mouseButton.button == sf::Mouse::Right) {
                    board.rightClick(mouseX/32, mouseY/32);
                }
            }
        }

        //clock
        if (not happyface.paused && happyface.game_state == 0) {
            auto time2 = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> duration = time2 - time1;
            time_duration += duration.count();
            time1 = time2;
            if (time_duration > 1000) {
                game_time += 1;
                time_duration = 0;
                std::cout << game_time << std::endl;
            }
        }



        window.clear(sf::Color::White);
        happyface.draw(window);
        debugButton.draw(window);
        playButton.draw(window);
        leaderboardButton.draw(window);

        //clock display
        digit.setPosition(sf::Vector2f(columns * 32 - 97, 32 * (rows + 0.5) + 16));
        digit.setDigit((game_time/60)/10);
        digit.draw(window);
        digit.setPosition(sf::Vector2f(columns * 32 - 97 + 21, 32 * (rows + 0.5) + 16));
        digit.setDigit((game_time/60)%10);
        digit.draw(window);
        digit.setPosition(sf::Vector2f(columns * 32 - 54, 32 * (rows + 0.5) + 16));
        digit.setDigit((game_time%60)/10);
        digit.draw(window);
        digit.setPosition(sf::Vector2f(columns * 32 - 54+21, 32 * (rows + 0.5) + 16));
        digit.setDigit((game_time%60)%10);
        digit.draw(window);
        int flag_count = numMines - board.getFlagCount();
        if (flag_count < 0){
            digit.setPosition(sf::Vector2f(12, 32 * (rows + 0.5) + 16));
            digit.setDigit(10);
            digit.draw(window);
        }
        flag_count = abs(flag_count);
        digit.setPosition(sf::Vector2f(33, 32 * (rows + 0.5) + 16));
        digit.setDigit((flag_count/100));
        digit.draw(window);
        digit.setPosition(sf::Vector2f(33 + 21, 32 * (rows + 0.5) + 16));
        digit.setDigit((flag_count%100)/10);
        digit.draw(window);
        digit.setPosition(sf::Vector2f(33 + 42, 32 * (rows + 0.5) + 16));
        digit.setDigit((flag_count%100)%10);
        digit.draw(window);
        board.draw(window, windowWidth / columns);
    }
    return 0;
}

