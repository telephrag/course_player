# course_player
Простейший музыкальный плеер для проигрывания музыки с Ютюб. Написан для Linux.
Писался в качестве курсовой работы, где по заданию нужно было использовать БД.

# Используемые технологии
- g++
- cmake 3.19
- libvlc, libvlcpp
- sqlite3.34
- python C-API

# Установка
Установить соответсвующие библиотеки, необходимые для сборки.
А именно: sqlite3, libvlc, libvlcpp, libpython.
```
git clone https://github.com/trofchik/course_player
cd course_player
cp threads_src/get_str.py ~/usr/lib64/python3.9/
cd build
```
Удалить из папки `build` всё кроме `test.db`
```
cmake ..
make
```
Для запуска `./coursework` (будет лежать в папке build)
