# Установка
## Зависимости
1) Git
2) Cmake (3.22 и выше)
3) Ninja

## Установка
> [!IMPORTANT]
> Выполнять в терминале

Клонирование репозитория и создание папки с билдом
```bash
git clone --recursive https://github.com/megonilus/reactor

cd reactor

mkdir build
```

Конфигурация Cmake
```bash
cmake -B build -G Ninja
```

Сборка симуляции
```bash
cd build && ninja
```

Запуск симуляции (только в папке с билдом)
```bash
./reactor
```
