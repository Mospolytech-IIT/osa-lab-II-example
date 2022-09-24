// Для трюков с ограничением дженериков 
#include <type_traits> 

// Для удобства
#include <string>
#include <vector>

// Для abs
#include <cmath>

// Работа с файлом
#include <stdlib.h>


// Что объединяет все варианты и задание? 
// То, что данные могут быть помечены как "пустые"
// И то, что они все структуры
// Поэтому я создаю базовую структуру Data и вписываю в неё нечто общее для всего 
// А конкретную структуру буду наследовать от Data
struct Data
{
    bool IsDeleted = false;
};

// template - это generic конструкции. Они применяются когда мы заранее не знаем тип, с которым должен работать класс. (или не хотим знать)
// Проще говоря, я ввожу новый "шаблонный" тип данных TStruct, который может быть чем угодно.
// Инициализируя Database, я могу сообщить, чтобы она работала с какой угодно структурой, какого угодно варианта
// Например Database<Person> *database = new Database<Person>(dbName, dbSize); в данном случае Person - это TStruct
// И держу в голове, что TStruct некая структура, которую подставят при создании экземпляра класса
// Что я ещё знаю об этом? TStruct - наследуется от Data, значит, у TStruct есть IsDeleted и я могу этим воспользоваться 
// Всё! Я больше никакой информации не имею. Я не знаю что за пределами этого файла и какая структура подставлена
// Какие поля у структуры? Как по ним искать? Как вводить? Это не важно. Это не нужно.
// Это всё можно запрограммировать, не зная какая именно будет структура.
template<typename TStruct, typename = std::enable_if<std::is_base_of<Data, TStruct>::value>>
class Database
{
private:
    TStruct *_dataArray;
    long _databaseSize;
    char *_databaseName;

    void swap(TStruct *xp, TStruct *yp) 
    { 
        TStruct temp = *xp; 
        *xp = *yp; 
        *yp = temp; 
    } 

public:
    Database(std::string databaseName, long length){
        _databaseName = new char[databaseName.length() + 1]; 
        strcpy(_databaseName, databaseName.c_str()); 

        _dataArray = new TStruct[length];
        _databaseSize = length;

        for (size_t i = 0; i < _databaseSize; i++)
            _dataArray[i].IsDeleted = true;
    }

    ~Database(){
        delete[] _dataArray;
    }

    // 1. Очистка структурных переменных (отметка переменной как не содержащей данных). Функция должна получать в качестве параметра индекс массива.
    void Delete(int index){
        _dataArray[index].IsDeleted = true;
    }

    // 2. Поиск свободной структурной переменной. Функция должна возвращать индекс первого свободного элемента в массиве структур.
    int FindEmpty(){
        for (size_t i = 0; i < _databaseSize; i++)
            if(_dataArray[i].IsDeleted)
                return i;

        return -1;
    }

    // 3. Ввод элементов (полей) структуры с клавиатуры. В качестве параметра в функцию должен передаваться индекс элемента массива.
    // Именно, ввод будет в main, тут у меня просто добавление в базу
    void Add(int index, TStruct data){
        _dataArray[index] = data;
        _dataArray[index].IsDeleted = false;
    }

    // 4. Вывод элементов (полей) структуры на монитор. В функцию должен передаваться индекс элемента массива.
    // Именно, вывод в консоль в main
    TStruct Get(int index){
        return _dataArray[index];
    }

    // 5. Вывод на экран всех заполненных элементов массива структур. 
    // Именно, вывод в консоль в main
    std::vector<TStruct> GetAll(){
        std::vector<TStruct> result;
        for (size_t i = 0; i < _databaseSize; i++)
            if(!_dataArray[i].IsDeleted)
                result.push_back(_dataArray[i]);
        return result;
    }

    // 6. Поиск в массиве структур элемента с заданным значением поля или с наиболее близким к нему по значению. Предусмотреть возможность задания одного числового значения и одного строкового.
    // Начинается хардкор. 
    // Так, что мы видим?
    // Начнём с возвращаемого типа метода - TStruct*. Что это значит? У нас метод поиска. А поиск может ничего и не найти. 
    // Тогда мы вернём указатель в никуда - NULL. А может и найти, тогда вернём структуру TStruct.
    // Это не принципиально, можно возвращать индекс, и -1 если не нашли. Можно кидать ошибку.
    // Так, с возвращаемым типом разобрались, что дальше? 
    // Дальше видим два аргумента int (*selectorFunction)(TStruct) и int searchElement
    // Давайте с простого, int searchElement - это просто число, которое мы передадим, чтоб нам нашли похожее.
    // Но по какому полю искать? Мы ничего не знаем о структуре, ни о её полях. Она шаблон.
    // Раз уж у нас такая универсальная база, давайте мы попросим того, кто эту базу дёргает, говорить нам по чему искать.
    // Удобно. Универсально. Но как?
    // Все мы привыкли, что аргументы функции это там, число, строка, что-то простенькое.
    // Однако, аргументом функции может быть другая функция. Т.е. мы можем передать функцию в функцию. 
    // И int (*selectorFunction)(TStruct) - это некая функция selectorFunction. 
    // Эта функция принимает TStruct и возвращает int. Проще говоря, int selectorFunction(TStruct struct)
    // Мы на этом этапе не знаем что эта функция делает, как она внутри устроена.
    // Мы её можем вызвать. И она вернёт нам int. А мы в неё передадим i-й элемент массива.
    // Т.е., получается, что мы можем сделать selectorFunction(_dataArray[i]) и получить число. А потом, полученное число сравнить с searchElement.
    // Нам этого достаточно для того, чтобы всё запрограммировать, делаем цикл, каждый i-й элемент массива через эту функцию превращаем в число, и сравниваем с searchElement.
    // Настало время поговорить о том, что внутри себя делает selectorFunction. А её реализация такая: 
    // int selectorFunction(TStruct data) { return data.IntField } 
    // И вызов этого метода database->FindNearestInt([](ConcreteStructure structure){ return p.IntField; }, intFieldToSearch)
    // Здесь мы передаём [](ConcreteStructure structure){ return p.IntField; } - это то, что к нам придёт в качестве selectorFunction
    // Когда мы её передаём, мы не указываем имя функции, просто [](аргумент) { return аргумент.что-то }. 
    // Такая функция называется анонимной функцией. А ещё её называют лямбдой.
    TStruct* FindNearestInt(int (*selectorFunction)(TStruct), int searchElement){
        int indexOfNearestElement = -1;
        int minimalDiff = INT_MAX;

        for (size_t i = 0; i < _databaseSize; i++)
            if(!_dataArray[i].IsDeleted){
                int valueDiff = abs(selectorFunction(_dataArray[i]) - searchElement);

                if(valueDiff < minimalDiff){
                    minimalDiff = valueDiff;
                    indexOfNearestElement = i;
                }
            }

        return indexOfNearestElement == -1 ? (TStruct*)NULL : &_dataArray[indexOfNearestElement];
    }

    // Тут то же самое, только логика со строками и сигнатура string func(TStruct). Со строками искать похожие проще. 
    TStruct* FindNearestString(std::string (*selectorFunction)(TStruct), std::string searchElement){
        for (size_t i = 0; i < _databaseSize; i++)
            if(!_dataArray[i].IsDeleted && selectorFunction(_dataArray[i]).find(searchElement) != std::string::npos)
                return &_dataArray[i];
        return NULL;
    }

    // А можно без лямбд, типа 
    TStruct* FindNearestName(std::string searchElement);
    TStruct* FindNearestAge(int searchElement);
    // Но тут будет копипаста ↑, это плохо

    // Можно без копипасты, с такими сигнатурами
    // Как по fieldIndex получить значение поля? https://stackoverflow.com/questions/42550362/how-to-access-c-struct-property-value-using-index
    TStruct* FindNearestString(int fieldIndex, std::string searchElement);
    TStruct* FindNearestAge(int fieldIndex, int searchElement);

    // 7. Поиск в массиве структуры с минимальным значением заданного поля. Поле должно передаваться в поле числом, обозначающим номер поля в структуре.
    // Номер поля в структуре? Не, это слишком просто.
    // Усложняем.
    // Если в предыдущих функциях возвращаемый тип лямбды был известен, то тут поиск минимального. 
    // Поиск минимального может быть по строкам, а может быть по числам.
    // Мы не знаем по числу или по строке. Мы знаем только то, что числа и строки можно сравнивать через >, <, ==. 
    // Лямбда опять занимается тем, что достаёт из экземпляра структуры значение. Но какой тип этого значения? Какой возвращаемый тип лямбды?
    // Получается, возвращаемый тип лямбды может быть и строкой, и числом, и чем угодно. Но нам нужно что-то одно. 
    // Мы вводим новый шаблонный тип! TValue! Он - что угодно. Нам это подходит.
    // Нам главное уметь сделать TValue val1 > TValue val2 и всё
    template<typename TValue>
    TStruct* FindMin(TValue (*selectorFunction)(TStruct)){
        TValue min = selectorFunction(_dataArray[0]);
        int indexOfMinElement = -1;
        for (size_t i = 0; i < _databaseSize; i++)
            if(!_dataArray[i].IsDeleted && selectorFunction(_dataArray[i]) <= min)
            {
                indexOfMinElement = i;
                min = selectorFunction(_dataArray[i]);
            }

        return indexOfMinElement == -1 ? (TStruct*)NULL : &_dataArray[indexOfMinElement];   
    }

    // Вариант по fieldNumber
    TStruct* FindMin(int fieldNumber);
    // ↑ если внутри свичкейс, то что-то не так, попробуйте через std::get<fieldNumber>(std::tie(field1, field2))


    // 8. Сортировка массива структур в порядке возрастания заданного поля (при сортировке можно использовать тот факт, что в Си++ разрешается присваивание структурированных переменных);
    // 9. Сортировка массива структур в порядке убывания заданного поля (при сортировке можно использовать тот факт, что в Си++ разрешается присваивание структурированных переменных);
    // Тут всё хитро, лямбда, которая принимает два элемента и возвращает bool
    // Внутри себя если элемент1.Поле > элемент2.Поле то это по возрастанию 
    //             если элемент1.Поле < элемент2.Поле то это по убыванию
    // вот пример database->SortBy([](ConcreteStructure p1, ConcreteStructure p2){ return p1.SomeField < p2.SomeField; }); 
    // Это просто пузырёк. 
    void SortBy(bool (*f)(TStruct, TStruct)){
        for (int i = 0; i < _databaseSize - 1; i++)     
            for (int j = 0; j < _databaseSize - i - 1; j++) 
                if (f(_dataArray[j], _dataArray[j + 1])) 
                    swap(&_dataArray[j], &_dataArray[j + 1]); 
    }

    // 10. Функция чтения файла с содержимым базы данных (массива структурных элементов).
    void Restore(){
        FILE *file = fopen(_databaseName, "rb");

        for (size_t i = 0; i < _databaseSize; i++)
            fread(&_dataArray[i], sizeof(TStruct), 1, file);

        fclose(file);
    }

    // 11. Функция записи в файл содержимого базы данных (массива структурных элементов).
    void Save(){
        FILE *file = fopen(_databaseName, "wb");

        for (size_t i = 0; i < _databaseSize; i++)
            fwrite(&_dataArray[i], sizeof(TStruct), 1, file);

        fclose(file);
    }
};

