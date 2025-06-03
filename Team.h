#pragma once
#include <string>
#include <iostream>
#include <iomanip>

// Функция для подсчета реального количества символов в UTF-8 строке
int utf8_strlen(const std::string& str) {
    int length = 0;
    for (char c : str) {
        if ((c & 0xC0) != 0x80) { // Подсчитываем только начальные байты UTF-8 символов
            length++;
        }
    }
    return length;
}

class Team {
private:
    std::string name;        // Название команды
    unsigned int position;    // Место в турнирной таблице
    unsigned int games;      // Количество сыгранных игр
    unsigned int wins;       // Количество побед
    unsigned int draws;      // Количество ничьих
    unsigned int losses;     // Количество поражений

public:
    // Конструктор по умолчанию
    Team() : name(""), position(0), games(0), wins(0), draws(0), losses(0) {}

    // Конструктор с параметрами
    Team(const std::string& n) : name(n), position(0), games(0), wins(0), draws(0), losses(0) {}

    // Геттеры
    std::string getName() const { return name; }
    unsigned int getPosition() const { return position; }
    unsigned int getGames() const { return games; }
    unsigned int getWins() const { return wins; }
    unsigned int getDraws() const { return draws; }
    unsigned int getLosses() const { return losses; }
    
    // Вычисляемое поле - очки (победа = 3 очка, ничья = 1 очко)
    unsigned int getPoints() const { return wins * 3 + draws; }

    // Сеттеры
    void setName(const std::string& n) { name = n; }
    void setPosition(unsigned int pos) { position = pos; }
    
    // Методы для обновления статистики
    void addWin() { wins++; games++; }
    void addDraw() { draws++; games++; }
    void addLoss() { losses++; games++; }

    // Метод для вывода информации о команде
    void display() const {
        const size_t MAX_NAME_LENGTH = 100; // Максимальная длина названия
        const size_t DISPLAY_WIDTH = 20;    // Ширина колонки для отображения
        
        // Безопасно обрезаем слишком длинные названия
        std::string displayName = name.length() > MAX_NAME_LENGTH ? name.substr(0, MAX_NAME_LENGTH) : name;
        
        if (utf8_strlen(displayName) > DISPLAY_WIDTH) {
            size_t splitPos = 0;
            size_t bytePos = 0;
            int charCount = 0;
            
            // Находим позицию для разделения строки с учетом UTF-8
            for (size_t i = 0; i < displayName.length() && charCount < DISPLAY_WIDTH; ++i) {
                if ((displayName[i] & 0xC0) != 0x80) { // Начальный байт UTF-8
                    charCount++;
                    if (charCount <= DISPLAY_WIDTH) {
                        bytePos = i + 1;
                    }
                }
            }
            
            // Разделяем строку на части
            std::string firstLine = displayName.substr(0, bytePos);
            std::string remainingName = displayName.substr(bytePos);
            
            // Выводим первую строку
            int firstLineLength = utf8_strlen(firstLine);
            std::cout << std::left << firstLine << std::string(DISPLAY_WIDTH - firstLineLength, ' ') << " "
                      << std::right 
                      << std::setw(2) << position << " "
                      << std::setw(4) << games << " "
                      << std::setw(4) << wins << " "
                      << std::setw(4) << draws << " "
                      << std::setw(4) << losses << " "
                      << std::setw(4) << getPoints()
                      << std::endl;
            
            // Выводим оставшуюся часть названия, если она есть
            if (!remainingName.empty()) {
                // Ограничиваем длину второй строки
                if (utf8_strlen(remainingName) > DISPLAY_WIDTH) {
                    size_t secondBytePos = 0;
                    charCount = 0;
                    for (size_t i = 0; i < remainingName.length() && charCount < DISPLAY_WIDTH - 3; ++i) {
                        if ((remainingName[i] & 0xC0) != 0x80) {
                            charCount++;
                            if (charCount <= DISPLAY_WIDTH - 3) {
                                secondBytePos = i + 1;
                            }
                        }
                    }
                    remainingName = remainingName.substr(0, secondBytePos) + "...";
                }
                
                int remainingLength = utf8_strlen(remainingName);
                std::cout << std::left << remainingName << std::string(DISPLAY_WIDTH - remainingLength, ' ') 
                          << std::string(29, ' ') // Пробелы для выравнивания с остальными колонками
                          << std::endl;
            }
        } else {
            // Для коротких названий оставляем прежний формат вывода
            int nameLength = utf8_strlen(displayName);
            std::cout << std::left << displayName << std::string(DISPLAY_WIDTH - nameLength, ' ') << " "
                      << std::right 
                      << std::setw(2) << position << " "
                      << std::setw(4) << games << " "
                      << std::setw(4) << wins << " "
                      << std::setw(4) << draws << " "
                      << std::setw(4) << losses << " "
                      << std::setw(4) << getPoints()
                      << std::endl;
        }
    }
}; 