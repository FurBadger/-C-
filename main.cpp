#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
#include <windows.h>
#include <cctype>
#include <iomanip>
#include "Team.h"

// Глобальный вектор для хранения команд
std::vector<Team> teams;

// Функция для проверки, содержит ли строка только английские буквы и пробелы
bool isEnglishString(const std::string& str) {
    if (str.empty()) return false;
    return std::all_of(str.begin(), str.end(), [](unsigned char c) {
        return (std::isalpha(c) && c < 128) || std::isspace(c);
    });
}

// Функция для установки русской локализации
void setRussianLocale() {
    SetConsoleCP(65001);        // Установка кодировки ввода UTF-8
    SetConsoleOutputCP(65001);  // Установка кодировки вывода UTF-8
    
    // Установка кодировки консоли в UTF-8
    system("chcp 65001");
}

// Функция для сортировки команд по очкам
void sortTeams() {
    std::sort(teams.begin(), teams.end(), 
        [](const Team& a, const Team& b) {
            return a.getPoints() > b.getPoints();
        });
    
    // Обновление позиций после сортировки
    for (size_t i = 0; i < teams.size(); ++i) {
        teams[i].setPosition(i + 1);
    }
}

// Функция для сохранения данных в файл
void saveToFile(const std::string& filename) {
    std::ofstream file(filename);
    if (!file) {
        std::cout << "Ошибка при открытии файла для сохранения!\n";
        return;
    }

    for (const auto& team : teams) {
        file << team.getName() << ","
             << team.getGames() << ","
             << team.getWins() << ","
             << team.getDraws() << ","
             << team.getLosses() << "\n";
    }
    std::cout << "Данные успешно сохранены в файл.\n";
}

// Функция для проверки корректности данных команды
bool validateTeamData(const Team& team) {
    // Проверка на отрицательные значения
    if (team.getGames() < 0 || team.getWins() < 0 || 
        team.getDraws() < 0 || team.getLosses() < 0) {
        std::cout << "Ошибка: отрицательные значения в данных команды \"" << team.getName() << "\"\n";
        return false;
    }
    
    // Проверка на соответствие количества игр
    if (team.getGames() != team.getWins() + team.getDraws() + team.getLosses()) {
        std::cout << "Ошибка: несоответствие количества игр у команды \"" << team.getName() << "\"\n";
        std::cout << "Игры: " << team.getGames() << ", Победы + Ничьи + Поражения = " 
                 << (team.getWins() + team.getDraws() + team.getLosses()) << "\n";
        return false;
    }
    
    // Проверка на пустое название команды
    if (team.getName().empty()) {
        std::cout << "Ошибка: пустое название команды\n";
        return false;
    }
    
    // Проверка на допустимые символы в названии (только английские буквы и пробелы)
    if (!isEnglishString(team.getName())) {
        std::cout << "Ошибка: недопустимые символы в названии команды \"" << team.getName() << "\"\n";
        return false;
    }
    
    return true;
}

// Функция для загрузки данных из файла
bool loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cout << "Ошибка при открытии файла для чтения!\n";
        return false;
    }

    std::vector<Team> tempTeams; // Временный вектор для загрузки
    std::string line;
    int lineNumber = 0;
    bool hasErrors = false;

    while (std::getline(file, line)) {
        lineNumber++;
        size_t pos = 0;
        std::string token;
        std::vector<std::string> tokens;
        
        // Разбиваем строку по запятым
        while ((pos = line.find(",")) != std::string::npos) {
            token = line.substr(0, pos);
            tokens.push_back(token);
            line.erase(0, pos + 1);
        }
        tokens.push_back(line);

        // Проверка количества полей
        if (tokens.size() != 5) {
            std::cout << "Ошибка в строке " << lineNumber << ": неверное количество полей\n";
            hasErrors = true;
            continue;
        }

        try {
            Team team(tokens[0]);
            int wins = std::stoi(tokens[2]);
            int draws = std::stoi(tokens[3]);
            int losses = std::stoi(tokens[4]);

            // Добавляем статистику
            for (int i = 0; i < wins; ++i) team.addWin();
            for (int i = 0; i < draws; ++i) team.addDraw();
            for (int i = 0; i < losses; ++i) team.addLoss();

            // Проверяем корректность данных
            if (!validateTeamData(team)) {
                std::cout << "Ошибка в строке " << lineNumber << "\n";
                hasErrors = true;
                continue;
            }

            // Проверяем уникальность названия команды
            auto it = std::find_if(tempTeams.begin(), tempTeams.end(),
                [&](const Team& t) { return t.getName() == team.getName(); });
            if (it != tempTeams.end()) {
                std::cout << "Ошибка в строке " << lineNumber << ": команда \"" 
                         << team.getName() << "\" уже существует\n";
                hasErrors = true;
                continue;
            }

            tempTeams.push_back(team);
        }
        catch (const std::invalid_argument& e) {
            std::cout << "Ошибка в строке " << lineNumber << ": некорректные числовые данные\n";
            hasErrors = true;
            continue;
        }
        catch (const std::out_of_range& e) {
            std::cout << "Ошибка в строке " << lineNumber << ": числовые данные слишком большие\n";
            hasErrors = true;
            continue;
        }
    }

    if (hasErrors) {
        std::cout << "\nФайл содержит ошибки. Загрузить данные? (y/n): ";
        char choice;
        std::cin >> choice;
        std::cin.ignore();
        
        if (choice != 'y' && choice != 'Y') {
            std::cout << "Загрузка отменена.\n";
            return false;
        }
    }

    // Если все проверки пройдены или пользователь согласился загрузить данные с ошибками
    teams = tempTeams;
    sortTeams();
    std::cout << "Данные успешно загружены из файла.\n";
    return true;
}

// Функция для отображения всех команд
void displayAllTeams() {
    if (teams.empty()) {
        std::cout << "База данных пуста!\n";
        return;
    }

    // Выводим заголовок таблицы с фиксированным форматированием
    std::cout << "Название              № Игры Поб  Нич  Пор  Очки\n";
    std::cout << std::string(49, '-') << std::endl;

    for (const auto& team : teams) {
        team.display();
    }
}

// Функция для получения названия команды с проверкой на английские буквы
std::string getTeamNameFromUser(const std::string& prompt) {
    std::string teamName;
    bool validName = false;
    
    while (!validName) {
        std::cout << prompt;
        std::getline(std::cin, teamName);
        
        if (!isEnglishString(teamName)) {
            std::cout << "Ошибка: используйте только английские буквы!\n";
        } else {
            validName = true;
        }
    }
    return teamName;
}

// Функция для добавления результата матча
void addMatchResult() {
    char result;
    std::string winner, loser;

    std::cout << "Введите результат матча (V - победа, D - ничья): ";
    std::cin >> result;
    std::cin.ignore();

    if (result != 'V' && result != 'D') {
        std::cout << "Неверный формат результата!\n";
        return;
    }

    if (result == 'V') {
        winner = getTeamNameFromUser("Введите название команды-победителя (на английском): ");
        loser = getTeamNameFromUser("Введите название проигравшей команды (на английском): ");

        auto winTeam = std::find_if(teams.begin(), teams.end(),
            [&](const Team& t) { return t.getName() == winner; });
        auto loseTeam = std::find_if(teams.begin(), teams.end(),
            [&](const Team& t) { return t.getName() == loser; });

        if (winTeam == teams.end() || loseTeam == teams.end()) {
            std::cout << "Одна или обе команды не найдены в базе данных!\n";
            return;
        }

        winTeam->addWin();
        loseTeam->addLoss();
    }
    else { // Ничья
        winner = getTeamNameFromUser("Введите название первой команды (на английском): ");
        loser = getTeamNameFromUser("Введите название второй команды (на английском): ");

        auto team1 = std::find_if(teams.begin(), teams.end(),
            [&](const Team& t) { return t.getName() == winner; });
        auto team2 = std::find_if(teams.begin(), teams.end(),
            [&](const Team& t) { return t.getName() == loser; });

        if (team1 == teams.end() || team2 == teams.end()) {
            std::cout << "Одна или обе команды не найдены в базе данных!\n";
            return;
        }

        team1->addDraw();
        team2->addDraw();
    }

    sortTeams();
    std::cout << "Результат матча успешно добавлен.\n";
    std::cout << "Не забудьте сохранить изменения через меню.\n";
}

// Функция для отображения топ-3 команд
void displayTop3() {
    if (teams.empty()) {
        std::cout << "База данных пуста!\n";
        return;
    }

    std::cout << "\nТоп-3 команды:\n";
    std::cout << std::string(78, '-') << std::endl;
    
    for (size_t i = 0; i < std::min(size_t(3), teams.size()); ++i) {
        teams[i].display();
    }
}

// Функция для вывода заголовка таблицы
void displayTableHeader() {
    std::cout << "Название              № Игры Поб  Нич  Пор  Очки\n";
    std::cout << std::string(49, '-') << std::endl;
}

// Функция для поиска команды
void searchTeam() {
    std::string searchQuery = getTeamNameFromUser("Введите название команды для поиска (на английском): ");
    bool found = false;
    
    std::cout << "\nРезультаты поиска:\n";
    displayTableHeader();
    
    for (const auto& team : teams) {
        // Поиск подстроки в названии команды (нечувствительный к регистру)
        std::string teamName = team.getName();
        std::string query = searchQuery;
        
        // Преобразование к нижнему регистру для сравнения
        std::transform(teamName.begin(), teamName.end(), teamName.begin(), ::tolower);
        std::transform(query.begin(), query.end(), query.begin(), ::tolower);
        
        if (teamName.find(query) != std::string::npos) {
            team.display();
            found = true;
        }
    }
    
    if (!found) {
        std::cout << "Команды с таким названием не найдены.\n";
    }
}

// Функция для выбора способа сортировки
void sortTeamsBy() {
    std::cout << "\nВыберите способ сортировки:\n";
    std::cout << "1. По очкам (по убыванию)\n";
    std::cout << "2. По победам (по убыванию)\n";
    std::cout << "3. По названию (по алфавиту)\n";
    std::cout << "Выберите действие: ";
    
    int choice;
    std::cin >> choice;
    std::cin.ignore();
    
    switch (choice) {
        case 1:
            // Сортировка по очкам (уже реализована в sortTeams())
            sortTeams();
            break;
        case 2:
            // Сортировка по победам
            std::sort(teams.begin(), teams.end(),
                [](const Team& a, const Team& b) {
                    return a.getWins() > b.getWins();
                });
            break;
        case 3:
            // Сортировка по названию
            std::sort(teams.begin(), teams.end(),
                [](const Team& a, const Team& b) {
                    return a.getName() < b.getName();
                });
            break;
        default:
            std::cout << "Неверный выбор. Используется сортировка по очкам.\n";
            sortTeams();
            return;
    }
    
    // Обновление позиций после сортировки
    for (size_t i = 0; i < teams.size(); ++i) {
        teams[i].setPosition(i + 1);
    }
    
    std::cout << "\nТаблица после сортировки:\n";
    displayAllTeams();
}

// Функция для удаления команды
void deleteTeam() {
    bool tryAgain = true;
    
    while (tryAgain) {
        std::string teamName = getTeamNameFromUser("Введите название или часть названия команды для удаления (на английском): ");
        std::vector<Team*> foundTeams;
        
        // Преобразуем введенное имя в нижний регистр для сравнения
        std::string searchName = teamName;
        std::transform(searchName.begin(), searchName.end(), searchName.begin(), ::tolower);
        
        // Ищем все команды, содержащие введенную подстроку
        for (auto& team : teams) {
            std::string currentName = team.getName();
            std::transform(currentName.begin(), currentName.end(), currentName.begin(), ::tolower);
            if (currentName.find(searchName) != std::string::npos) {
                foundTeams.push_back(&team);
            }
        }
        
        if (foundTeams.empty()) {
            std::cout << "Команды, содержащие \"" << teamName << "\" не найдены.\n";
            std::cout << "Хотите повторить поиск? (y/n): ";
            char retry;
            std::cin >> retry;
            std::cin.ignore();
            tryAgain = (retry == 'y' || retry == 'Y');
            if (!tryAgain) {
                return;
            }
            continue;
        }
        
        // Если найдено несколько команд, даем выбрать нужную
        if (foundTeams.size() > 1) {
            std::cout << "\nНайдено несколько команд:\n";
            displayTableHeader();
            
            // Выводим команды с номерами перед названием команды
            for (size_t i = 0; i < foundTeams.size(); ++i) {
                std::cout << std::right << std::setw(2) << (i + 1) << ". ";
                foundTeams[i]->display();
            }
            
            std::cout << "\nВведите номер команды для удаления (0 для отмены): ";
            size_t choice;
            std::cin >> choice;
            std::cin.ignore();
            
            if (choice == 0 || choice > foundTeams.size()) {
                std::cout << "Удаление отменено.\n";
                return;
            }
            
            Team* selectedTeam = foundTeams[choice - 1];
            
            // Запрашиваем подтверждение удаления
            std::cout << "\nВы уверены, что хотите удалить команду:\n";
            displayTableHeader();
            selectedTeam->display();
            std::cout << "\nПодтвердите удаление (y/n): ";
            char confirm;
            std::cin >> confirm;
            std::cin.ignore();
            
            if (confirm == 'y' || confirm == 'Y') {
                auto it = std::find_if(teams.begin(), teams.end(),
                    [&](const Team& t) { return &t == selectedTeam; });
                if (it != teams.end()) {
                    teams.erase(it);
                    std::cout << "Команда \"" << selectedTeam->getName() << "\" успешно удалена.\n";
                    std::cout << "Не забудьте сохранить изменения через меню.\n";
                    sortTeams();
                }
            } else {
                std::cout << "Удаление отменено.\n";
            }
        } else {
            // Если найдена только одна команда
            Team* selectedTeam = foundTeams[0];
            std::cout << "\nНайдена команда:\n";
            displayTableHeader();
            selectedTeam->display();
            
            std::cout << "\nВы уверены, что хотите удалить эту команду? (y/n): ";
            char confirm;
            std::cin >> confirm;
            std::cin.ignore();
            
            if (confirm == 'y' || confirm == 'Y') {
                auto it = std::find_if(teams.begin(), teams.end(),
                    [&](const Team& t) { return &t == selectedTeam; });
                if (it != teams.end()) {
                    teams.erase(it);
                    std::cout << "Команда \"" << selectedTeam->getName() << "\" успешно удалена.\n";
                    std::cout << "Не забудьте сохранить изменения через меню.\n";
                    sortTeams();
                }
            } else {
                std::cout << "Удаление отменено.\n";
            }
        }
        
        tryAgain = false;
    }
}

int main() {
    setRussianLocale();
    
    // Загрузка данных из файла при запуске
    if (!loadFromFile("football_database.txt")) {
        std::cout << "Не удалось загрузить базу данных. Программа будет завершена.\n";
        return 1;
    }
    
    while (true) {
        std::cout << "\nМеню:\n";
        std::cout << "1. Показать все команды\n";
        std::cout << "2. Добавить результат матча\n";
        std::cout << "3. Показать топ-3 команды\n";
        std::cout << "4. Сохранить данные в файл\n";
        std::cout << "5. Загрузить данные из файла\n";
        std::cout << "6. Удалить команду\n";
        std::cout << "7. Найти команду\n";
        std::cout << "8. Сортировать команды\n";
        std::cout << "0. Выход\n";
        std::cout << "Выберите действие: ";

        int choice;
        std::cin >> choice;
        std::cin.ignore();

        switch (choice) {
            case 1:
                displayAllTeams();
                break;
            case 2:
                addMatchResult();
                break;
            case 3:
                displayTop3();
                break;
            case 4:
                saveToFile("football_database.txt");
                break;
            case 5:
                loadFromFile("football_database.txt");
                break;
            case 6:
                deleteTeam();
                break;
            case 7:
                searchTeam();
                break;
            case 8:
                sortTeamsBy();
                break;
            case 0:
                std::cout << "Программа завершена.\n";
                return 0;
            default:
                std::cout << "Неверный выбор. Попробуйте снова.\n";
        }
    }

    return 0;
} 