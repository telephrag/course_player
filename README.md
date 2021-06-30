# course_player
Простейший музыкальный плеер для проигрывания музыки с Ютюб. Написан для Linux.
Писался в качестве курсовой работы, где по заданию нужно было использовать БД.
Находится в сотоянии MVP (неудобный и может крашнуться)

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
Чтобы протестировать работу введите команнду: `playlist login tro pass getplaylist 1 quit`

# Команды
## В главном меню
`playlist` - установка плейлиста
`next` - следующий трек
`prev` - предыдущий трек
## В меню управления плейлистами (после ввода команды `playlist`)
`login` - вход в БД как зарегистрированный пользователь
`signup` - регистрация в БД
`quit` - выход в главное меню
## Войдя в БД (после команды `login`)
`addsong` - добавить ссылку на трек в БД
`makeplaylist` - создать плейлист
`addtoplaylist` - добавить песню в плейлист
`pringsongs` - выводит все песни в БД
`printplaylists` - выводит все принадлежащие вам плейлисты
`getplaylist` - выбирает плейлист для проигрывания
`quit` - выход в главное меню
