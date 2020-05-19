pic-cutter
==========

Приложение для сокращения ширины изображения.

Зависимости для сборки (Linux)
------------------------------

1. gcc 5.5.0+
2. libpng-dev


Инструкция для сборки
---------------------

Склонировать репозиторий.

В корне репозитория выполнить:
```
gcc -o ./bin/pic-cutter ./src/main.c -lpng
```

[Скачать](https://drive.google.com/drive/folders/1tDSbGuJF8YoWFjlVid0BxNcEUbJf09Y8?usp=sharing) собранную программу для `linux_64`.

Использование
-------------

```
./bin/pic-cutter [picture.png] [new_picture.png] [new_width]
```
Программа создаст файл с названием `new_picture.png`, в котором будет содержаться результат.

Комментарии
-----------
[Описание решения](./ABOUT_SOLUTION.md)