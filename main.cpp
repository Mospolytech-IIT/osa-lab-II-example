#include <iostream>
#include <stdio.h>
#include <string.h>
#include "Database.cpp"

// Так, сначала идите читать всё, что написано в Database.cpp, сюда вернётесь потом! 

// Я создаю свою структуру по варианту, и наследую её от того самого Data из файла Database.cpp
struct MyMegaCoolStructure : Data
{
    int Age;
    char Name[50];
    int YearBorn;
};

int main() {
    int dbSize = 10;
    std::string dbName = "myCoolDatabase.bin";

    Database<MyMegaCoolStructure> *database = new Database<MyMegaCoolStructure>(dbName, dbSize);


    // Пример ввода данных
    for (size_t i = 0; i < 3; i++)
    {
        MyMegaCoolStructure structure;
        std::cout << "Age: "; std::cin >> structure.Age;
        std::cout << "Name: "; std::cin >> structure.Name;
        std::cout << "YearBorn: "; std::cin >> structure.YearBorn;
        database->Add(i, structure);    
        std::cout << "Added! " << std::endl;
    }

    // Пример сохранения в файл
    database->Save();

    // Пример уничнтожения экземпляра Database и восстановления из файла
    delete database;
    database = new Database<MyMegaCoolStructure>(dbName, dbSize);
    database-> Restore();
    
    // Пример вывода данных
    std::vector<MyMegaCoolStructure> all = database->GetAll();
    for (size_t i = 0; i < all.size(); i++)
        std::cout << all[i].Name << " " << all[i].Age << " " << all[i].YearBorn << std::endl;

    // Пример поиска наиболее близкого по числу
    int ageToSearch; 
    std::cout << "Age to search: "; std::cin >> ageToSearch;
    MyMegaCoolStructure * resByInt = database->FindNearestInt([](MyMegaCoolStructure p){ return p.Age; }, ageToSearch);
    if(resByInt == NULL) 
        std::cout << "NotFound!" << std::endl;
    else    
        std::cout << resByInt->Name << " " << resByInt->Age << " " << resByInt->YearBorn << std::endl;

    // Пример поиска наиболее близкого по строке
    std::string name; 
    std::cout << "Name to search: "; std::cin >> name;
    MyMegaCoolStructure * resByStr = database->FindNearestString([](MyMegaCoolStructure p){ return std::string(p.Name); }, name);
    if(resByStr == NULL) 
        std::cout << "NotFound!" << std::endl;
    else    
        std::cout << resByStr->Name << " " << resByStr->Age << " " << resByStr->YearBorn << std::endl;

    // Пример минимального по строке
    MyMegaCoolStructure * minByName = database->FindMin<std::string>([](MyMegaCoolStructure p){ return std::string(p.Name); });
     std::cout << "Min by Name: " << minByName->Name << " " << minByName->Age << " " << minByName->YearBorn << std::endl;

    // Пример минимального по числу
    MyMegaCoolStructure * minByYearBorn = database->FindMin<int>([](MyMegaCoolStructure p){ return p.YearBorn; });
    std::cout << "Min by Year: " << minByYearBorn->Name << " " << minByYearBorn->Age << " " << minByYearBorn->YearBorn << std::endl;

    // Пример сортировки по возрастанию по строковому полю
    database->SortBy([](MyMegaCoolStructure p1, MyMegaCoolStructure p2){ return std::string(p1.Name) > std::string(p2.Name); });
    std::cout << "Sorted by Name asc: " << std::endl;
    std::vector<MyMegaCoolStructure> allAfterSortByName = database->GetAll();
    for (size_t i = 0; i < allAfterSortByName.size(); i++)
        std::cout << allAfterSortByName[i].Name << " " << allAfterSortByName[i].Age << " " << allAfterSortByName[i].YearBorn << std::endl;

    // Пример сортировки по убыванию по числовому полю
    database->SortBy([](MyMegaCoolStructure p1, MyMegaCoolStructure p2){ return p1.Age < p2.Age; });
    std::cout << "Sorted by Age desc: " << std::endl;
    std::vector<MyMegaCoolStructure> allAfterSortByAge = database->GetAll();
    for (size_t i = 0; i < allAfterSortByAge.size(); i++)
        std::cout << allAfterSortByAge[i].Name << " " << allAfterSortByAge[i].Age << " " << allAfterSortByAge[i].YearBorn << std::endl;

    return 0;
}