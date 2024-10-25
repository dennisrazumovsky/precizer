<p width="100%" height="100%">
<img width="20%" height="20%" src="icon/micrometer.svg">
</p>

# Precizer
This program is distributed under the [CC0 (Creative Commons Share Alike) license](https://creativecommons.org/publicdomain/zero/1.0/). The author is not responsible for any use of the source code or the entire program. Anyone who uses the code or the program uses it at their own risk and responsibility.

Автор приложения [Денис Разумовский](https://github.com/dennisrazumovsky)

## КРАТКО

**precizer** — это консольное приложение предназначенное для проверки целостности файлов после синхронизации. Программа рекурсивно обходит каталоги и создает базу данных файлов и их контрольных сумм с последующим быстрым сравнением.

**precizer** ориентирован на работу с файловыми системами гигантского размера. С помощью программы можно найти ошибки синхронизации, сравнивая данные с файлами и их контрольными суммами из разных источников. Или **precizer** можно использовать для исследования исторических изменений путем сравнения баз данных из одних и тех же источников за разное время.

## ПРОСТОЙ ПРИМЕР

Допустим, есть две машины у которых в /mnt1 и в /mnt2 соответственно, примонтированы диски большого объёма с идентичным содержимым. Стои́т задача проверить действительно ли содержимое абсолютно идентично или есть различия.

1. Запустить программу на первой машине с host name, например «host1»:

```sh
precizer --progress /mnt1
```
В результате работы программы будут рекурсивно исследованы все директории начиная с /mnt1 и создана база данных host1.db в текущей директории. Параметр _--progress_ визуализирует прогресс и покажет объем пространства и количество исследуемых файлов.

2. Запустить программу на второй машине с host name, например «host2»:

```sh
precizer --progress /mnt2
```

В результате будет создана база данных host2.db в текущей директории.

3. Cкопировать файлы с базами данных host1.db и host2.db на одну из машин и запустить программу с соответствующими параметрами для сравнения баз данных:

```sh
precizer --compare host1.db host2.db
```

На экран будет выведена следующая информация:
* Какие файлы отсутствуют на «host1» но при этом присутствуют на «host2» и наоборот.
* Для каких файлов, присутствующих и на обеих хостах, контрольные суммы НЕ совпадают.

Следует обратить внимание, что **precizer** записывает в базу данных только относительные пути. Файл «/mnt1/abc/def/aaa.txt» из приведённого примера будет записан в базу данных как  «abc/def/aaa.txt» без _/mnt1_. То же самое произойдёт с файлом «/mnt2/abc/def/aaa.txt». Несмотря на разные точки монтирования и разные источники файлы можно будет сравнить между собой под одинаковыми именами «abc/def/aaa.txt» с соответствующими контрольными суммами.

## ТЕХНИЧЕСКИЕ ПОДРОБНОСТИ

Рассмотрим сценарий, когда имеется основное дисковое хранилище и его копия. Например, это может быть хранилище датацентра и его Disaster Recovery копия. Периодически происходит синхронизация с основного хранилища на резервное, но по причине огромных объёмов данных, скорее всего, синхронизация происходит не побайтово, а за счёт вычисления изменений среди метаданных файлов на файловой системе. В таких случаях учитывается размер файла и время модификации, но изменившееся содержимое бай за байтом не исследуется. В этом есть смысл, потому что между основным датацентром и резервным Disaster Recovery центром, как правило, хорошие каналы связи, но полная побайтовая синхронизация может занять нецелесообразно большое количество времени. Такие инструменты как rsync позволяют производить синхронизацию по обеим методикам: как с учётом изменившихся файлов так и побайтово, но у них есть один серьёзный недостаток — состояние не сохраняется между сессиями. Что это значит рассмотрим в сценарии:

* Даны сервер «A» и сервер «B» (основной датацентр и резервный Disaster Recovery)
* На сервере «A» изменились некоторые файлы.
* Алгоритм rsync их определил за счёт изменившегося размера и времени модификации файла и синхронизировал на сервер «B».
* Во время синхронизации между основным датацентром и Disaster Recovery происходили многократные сбои связи.
* Для проверки целостности данных (эквивалентности сохранённых файлов на «A» и «B» байт в байт) обычно используют тот же rsync только с включением побайтного сравнения. Для этого:
* rsync запускается на сервере «A» в режиме _--checksum_ и во время одного сеанса пытается подсчитать контрольные суммы последовательно сначала на «A», и затем на «B».
* Этот процесс занимает неимоверно большое время для огромных дисковых массивов
* Так как rsync не позволяет сохранять состояние уже подсчитанных контрольных сумм между сеансами, то возникает целый ряд технических сложностей. А именно:
* В случае разрыва соединения rsуnc завершает сеанс и в следующий запуск всё нужно начинать сначала. С учётом огромных объёмов побайтовая проверка данных на консистентность таким образом превращается в нереализуемую задачу.
* Со временем ошибки накапливаются и появляется угроза получить некосистентную копию системы «A» на системе «B», что сводит на нет все усилия и затраты по поддержанию Disaster Recovery. При этом стандартные утилиты не обладают возможностями проверок и технический персонал даже не будет знать о накопившихся проблемах с неэквивалентным содержанием дисковых массивов на Disaster Recovery центре.
* Для устранения вышеописанных недостатков создана программа **precizer**. Программа  позволяет выявить какие именно файлы отличаются между «A» и «B» для проведения повторной синхронизации с устранением отличий. Программа работает максимально быстро (практически на грани аппаратных возможностей) за счёт того, что написана на чистом Си и использует современные алгоритмы оптимизированные под высокую производительность. Программа предназначена для работы как с мелкими файлами так и с объёмами данных измеряемыми петабайтами и не ограничена этими цифрами.
* Название программы **precizer** происходит от слова precision (точность) и означает что-то, что увеличивает точность.
* Программа с высокой точностью исследует содержимое директорий, субдиректорий и подсчитает контрольные суммы для каждого встреченного файла, при этом сохраняя данные в SQLite базе (обычный бинарный файл).
* **precizer** устойчива к сбоям и умеет продолжить работу с того момента, где была прервана. Например, если программа была остановлена нажатием Ctrl+C в момент анализа файла петабайтного объёма, то при повторном запуске она НЕ будет анализировать его заново, а продолжит именно с того момента, о котором уже есть запись в базе данных. Это позволит сэкономить ресурсы и время.
* Работа этой программы может быть прервана в любой момент любым способом и это безопасно как для исследуемых данных, как и для БД, созданной самой программой.
* В случае умышленной или случайной остановки работы программы можно не беспокоиться о результатах сбоя. Результат работы будет полностью сохранён и повторно использован при следующих запусках.
* Для подсчёта контрольных сумм используется надёжный и быстрый алгоритм SHA512 полностью исключающий ошибки даже в случае анализа единичного файла петабайтного объёма. Если есть два полностью идентичных файла огромного объёма, различающихся только в один байт, то алгоритм SHA512 это отразит и контрольные суммы будут различаться, что не может быть гарантировано в случае использования более простых хеш-функций типа SHA1 или CRC32.
* Алгоритмы программы **precizer** разработаны так, что очень просто поддерживать актуальность содержащихся данных в созданной базе с путями к файлам и их контрольными суммами без пересчёта всего с самого начала. Достаточно запустить программу с параметром _--update_ чтобы в БД попали новые файлы, была удалена информация о стёртых с диска файлах, а для тех файлов, которые подверглись модификациям и их время создания или размер изменились будет пересчитана контрольная сумма SHA512 и сохранена в БД.
* Сравнивая базы данных из одних и тех же источников за разное время **precizer** может служить инструментом контроля безопасности, определяя последствия вторжения за счёт выявления несанкционированно изменённых файлов, у которых могло быть модифицировано содержимое но метаданные остаться прежними.
* Программа никогда не меняет, не удаляет, не перемещает и не копирует ни файлы, ни исследуемые директории. Всё что она делает: составляет списки файлов и актуализирует их в базе данных. Все изменения происходят исключительно в границах этой базы данных.
* Производительность программы в основном упирается в производительность дисковой подсистемы. Каждый файл считывается побайтно и таким образом для каждого файла формируется контрольная сумма с использованием алгоритма SHA512.
* Программа работает очень быстро благодаря библиотекам SQLite и FTS ([man 3 fts](https://man7.org/linux/man-pages/man3/fts.3.html)).
* Разбор параметров строки реализован через библиотеку ARGP
* Программа безопасна для случаев с огромными количествами файлов, директорий и субдиректорий любой вложенности. Благодаря библиотеке FTS рекурсия не используется, поэтому не произойдёт переполнения стека.
* За счёт своей компактности и переносимости кода программа может использоваться даже на специализированных устройствах типа NAS или любых embedded или IoT устройствах.

## ВОПРОСЫ И БАГРЕПОРТЫ

* В случае возникающих вопросов вызывайте справочную информацию используя _--help_ Справка сделана максимально подробной специально для помощи пользователям, не обладающих специализированными техническими знаниями.
* Обратиться к автору можно [через форму github](https://github.com/dennisrazumovsky). Там же можно [опубликовать багрепорт](https://github.com/dennisrazumovsky/precizer/issues/new).
* При возникновении вопросов по использованию программы можно задать вопрос на stackoverflow используя тег **precizer**. Автор следит за такими вопросами и будет рад дать свой ответ.

## СБОРКА И УСТАНОВКА

### Distributives Packaging

* Автор был рад подготовить и будет в дальнейшем поддерживать собранные бинарные пакеты под Flatpak и AppImage.
* Автор НЕ готов самостоятельно готовить и поддерживать в будущем опакечивание программы **precizer** под все существующие дистрибутивы.
* Если Вы горите желанием создать пакет под любой дистрибутив и столкнулись с непреодолимыми трудностями по адаптации кода программы, то именно в этом случае автор будет очень рад оказать всю необходимую помощь в поддержке инициативы и оптимизации кода программы под конкретный дистрибутив или пакетный менеджер. Как связаться с автором описано в разделе "Вопросы и багрепорты"

### Portable

Скачать готовое решение

#### Flatpak
TODO!

#### AppImage
TODO!

### Самостоятельная сборка

В программу интегрированы почти все используемые библиотеки и по умолчанию программа собирается как статический исполняемый файл. Это сделано для увеличения переносимости и уменьшению зависимостей. Благодаря вышеописанному программу можно легко собрать на большинстве современных платформ выполнив несколько команд:

1. Install build and compile tools on Linux

#### Arch

```sh
sudo pacman -S --noconfirm base-devel cmake git unzip
```

#### Debian/Ubuntu

```sh
sudo apt -y install build-essential cmake git unzip
```

2. Get source code

```sh
git clone https://github.com/dennisrazumovsky/precizer.git
```

3. Build

```sh
cd precizer
make
```

4. Скопируйте получившийся исполняемый файл **precizer** в любое место, прописанное в системной переменной $PATH для быстрого вызова.

5. Clean everything and update

```sh

# Clean
make clean

# Update
git pull
make

# Перейти к пункту 4.
```

## ПРИМЕРЫ ИСПОЛЬЗОВАНИЯ
Для проверки возможностей программы можно использовать наборы тестов из директории tests/examples/ в исходном коде программы

### Пример 1
Добавить файлы в две базы данных и сравнить их между собой:

```sh
precizer --progress --database=database1.db tests/examples/diffs/diff1

precizer --progress --database=database2.db tests/examples/diffs/diff2

precizer --compare database1.db database2.db
```
<sup>Comparison of databases database1.db and database2.db is starting...  
**These files no longer exist against database1.db but still present against database2.db**  
path1/AAA/BCB/CCC/a.txt  
path1/AAA/BCB/CCC/b.txt  
**These files no longer exist against database2.db but still present against database1.db**  
path2/AAA/ZAW/D/e/f/b_file.txt  
**The SHA512 checksums of these files do not match between database1.db and database2.db**  
1/AAA/BCB/CCC/a.txt  
2/AAA/BBB/CZC/a.txt  
3/AAA/BBB/CCC/a.txt  
4/AAA/BBB/CCC/a.txt  
path1/AAA/ZAW/D/e/f/b_file.txt  
path2/AAA/BCB/CCC/a.txt  
</sub>

### Пример 2
Актуализация базы данных

Попробуем использовать предыдущий пример ещё раз. Первая попытка. Сообщение с предупреждением.

```sh
precizer --progress --database=database1.db tests/examples/diffs/diff1
```

<sub>Database file name: database1.db  
The database database1.db has been created in the past and already contains data with files and their checksums. Use the --update option if there is full confidence that update the content of the database is really need and the information about those files which was changed, removed or added should be deleted or updated against DB.  
The precizer unexpectedly ended due to an error.  
</sub>

Должен быть добавлен параметр **--update** _--update_ необходим для защиты базы данных от потери информации из-за случайного запуска.

```sh
precizer --update --progress --database=database1.db tests/examples/diffs/diff1
```

<sub>Database file name: database1.db  
Starting of database file database1.db integrity check...  
The database database1.db has been verified and is in good condition  
total size: 45B, total items: 58, dirs: 46, files: 12, symlnks: 0  
total size: 45B, total items: 58, dirs: 46, files: 12, symlnks: 0  
Start vacuuming...  
The database has been vacuumed  
**Nothing have been changed since the last probe (neither added nor updated or deleted files)**  
</sub>

Внесём некоторые изменения:

```sh
# Modify a file
echo -n "  " >> tests/examples/diffs/diff1/1/AAA/BCB/CCC/a.txt

# Add a new file
touch tests/examples/diffs/diff1/1/AAA/BCB/CCC/c.txt

# Remove a file
rm tests/examples/diffs/diff1/path2/AAA/ZAW/D/e/f/b_file.txt

```

и запустим **precizer** ещё раз:

```sh
precizer --update --progress --database=database1.db tests/examples/diffs/diff1
```

<sub>Database file name: database1.db  
Starting of database file database1.db integrity check...  
The database database1.db has been verified and is in good condition  
total size: 43B, total items: 58, dirs: 46, files: 12, symlnks: 0  
The **--update** option has been used, so the information about files will be updated against the database database1.db  
**These files have been added or changed and those changes will be reflected against the DB database1.db:**  
1/AAA/BCB/CCC/a.txt changed size & ctime & mtime  
1/AAA/BCB/CCC/c.txt adding  
total size: 43B, total items: 58, dirs: 46, files: 12, symlnks: 0  
**These files are ignored or no longer exist and will be deleted against the DB database1.db:**  
path2/AAA/ZAW/D/e/f/b_file.txt  
Start vacuuming...  
The database has been vacuumed  
</sub>

При каждом запуске **precizer** обходит файловую систему после этого проверяя, есть ли запись об определенном файле в базе данных или нет. Другими словами, приоритет для программы имеет состояние файловой системы на диске.

Обход каталогов **precizer** работает очень похоже на работу rsync, поскольку использует похожий алгоритм.

Стоит обратить внимание, что **precizer** не будет пересчитывать контрольные суммы SHA512 для файлов, которые уже были записаны в базу данных и для которых метаданные файла (такие как время создания, время модификации и размер) остаются прежними.

Любые новые файлы, удаленные файлы или файлы, которые изменились между запусками приложения, будут обработаны, и все изменения будут отражены в базе данных, если указан параметр _--update_.

### Пример 3
Использование режима _--silent_ При включении этого режима программа ничего не выводит на экран. Это имеет смысл при использовании программы внутри скриптов.

Добавим параметр **--silent** к предыдущему примеру:

```sh
precizer --silent --update --progress --database=database1.db tests/examples/diffs/diff1
```

В результате на экране ничего не отобразится.

### Пример 4
Дополнительная информация в режиме _--verbose_ Может быть полезна для отладки.

Добавим параметр **--verbose** к предыдущему примеру:

```sh
precizer --verbose --update --progress --database=database1.db tests/examples/diffs/diff1
```
<sub>2024-03-09 22:56:49:748 src/parse_arguments.c:230:parse_arguments:Configuration: paths=tests/examples/diffs/diff1; verbose=yes; silent=no; force=no; update=yes; progress=yes; compare=no  
2024-03-09 22:56:49:748 src/init_signals.c:033:init_signals:Set signal SIGUSR2 OK:pid:462822  
2024-03-09 22:56:49:748 src/init_signals.c:045:init_signals:Set signal SIGINT OK:pid:462822  
2024-03-09 22:56:49:748 src/init_signals.c:057:init_signals:Set signal SIGTERM OK:pid:462822  
2024-03-09 22:56:49:748 src/db_init.c:045:db_init:The database has been successfully initialized  
2024-03-09 22:56:49:748 src/db_init.c:057:db_init:Opened database successfully  
2024-03-09 22:56:49:748 src/db_already_exists.c:054:db_already_exists:The database has already been created in the past  
2024-03-09 22:56:49:748 src/db_check_up_paths.c:144:db_check_up_paths:The paths written against the database and the paths passed as arguments are completely identical. Nothing will be lost  
2024-03-09 22:56:49:749 src/file_list.c:244:file_list:total size: 43B, total items: 55, dirs: 44, files: 11, symlnks: 0  
2024-03-09 22:56:49:749 src/file_list.c:244:file_list:total size: 43B, total items: 55, dirs: 44, files: 11, symlnks: 0  
2024-03-09 22:56:49:749 src/db_vacuum.c:021:db_vacuum:Start vacuuming...  
2024-03-09 22:56:49:750 src/db_vacuum.c:035:db_vacuum:The database has been vacuumed  
2024-03-09 22:56:49:750 src/status_of_changes.c:015:status_of_changes:**Nothing have been changed since the last probe (neither added nor updated or deleted files)**  
2024-03-09 22:56:49:750 src/exit_status.c:026:exit_status:The precizer completed its execution without any issues.  
2024-03-09 22:56:49:750 src/exit_status.c:027:exit_status:Enjoy your life!  
</sub>

### Пример 5
Исследование без рекурсии с помощью параметра _--maxdepth_

```sh
tree tests/examples/4
tests/examples/4
├── AAA
│   ├── BBB
│   │   ├── CCC
│   │   │   └── a.txt
│   │   └── uuu.txt
│   └── tttt.txt
└── sss.txt

3 directories, 4 files
```

Параметр _--maxdepth_ со значением _=0_ полностью отключает рекурсию.

```sh
precizer --maxdepth=0 tests/examples/4
```
<sub>Database file name: myhost.db  
Recursion depth limited to: 0  
**These files will be added against the DB myhost.db:**  
sss.txt  
Start vacuuming...  
The database has been vacuumed  
The precizer completed its execution without any issues.  
</sub>

### Пример 6

Относительный путь который следует игнорировать. Регулярные выражения PCRE2 можно использовать для указания шаблона игнорирования файлов или каталогов. Внимание! Все пути в регулярном выражении должны быть указаны как **относительные**.

Чтобы проверить и протестировать регулярные выражения PCRE2 можно использовать ресурс https://regex101.com/

Чтобы понять как выглядит относительный путь достаточно запустить сканирование директорий без опции _--ignore_ и посмотреть, как терминал будет отображать относительные пути, записываемые в базу данных:

```sh
% tree -L 3 tests/examples/diffs
tests/examples/diffs
├── diff1
│   ├── 1
│   │   └── AAA
│   ├── 2
│   │   └── AAA
│   ├── 3
│   │   └── AAA
│   ├── 4
│   │   └── AAA
│   ├── path1
│   │   └── AAA
│   └── path2
│       └── AAA
└── diff2
    ├── 1
    │   └── AAA
    ├── 2
    │   └── AAA
    ├── 3
    │   └── AAA
    ├── 4
    │   └── AAA
    ├── path1
    │   └── AAA
    └── path2
        └── AAA

26 directories, 0 files
```

```sh
precizer --ignore="diff2/1/.*" tests/examples/diffs
```

В этом примере начальный путь сканирования — ./tests/examples/diffs, а сформированный путь для игнорирования — ./tests/examples/diffs/diff2/1/ со всеми подкаталогами (/*).

<sub>Database file name: myhost.db  
Starting of database file myhost.db integrity check...  
The database myhost.db is in good condition  
**These files will be added against the DB myhost.db:**  
diff1/3/AAA/BBB/CCC/a.txt  
diff1/path2/AAA/BCB/CCC/a.txt  
diff1/path2/AAA/ZAW/A/b/c/a_file.txt  
diff1/path2/AAA/ZAW/D/e/f/b_file.txt  
diff1/1/AAA/BCB/CCC/a.txt  
diff1/1/AAA/ZAW/A/b/c/a_file.txt  
diff1/1/AAA/ZAW/D/e/f/b_file.txt  
diff1/4/AAA/BBB/CCC/a.txt  
diff1/2/AAA/BBB/CZC/a.txt  
diff1/path1/AAA/ZAW/A/b/c/a_file.txt  
diff1/path1/AAA/ZAW/D/e/f/b_file.txt  
diff2/3/AAA/BBB/CCC/a.txt  
diff2/path2/AAA/BCB/CCC/a.txt  
diff2/path2/AAA/ZAW/A/b/c/a_file.txt  
**ignored** diff2/1/AAA/BCB/CCC/a.txt  
**ignored** diff2/1/AAA/ZAW/A/b/c/a_file.txt  
**ignored** diff2/1/AAA/ZAW/D/e/f/b_file.txt  
diff2/4/AAA/BBB/CCC/a.txt  
diff2/2/AAA/BBB/CZC/a.txt  
diff2/path1/AAA/BCB/CCC/b.txt  
diff2/path1/AAA/BCB/CCC/a.txt  
diff2/path1/AAA/ZAW/A/b/c/a_file.txt  
diff2/path1/AAA/ZAW/D/e/f/b_file.txt  
Start vacuuming...  
The database has been vacuumed  
</sub>

Повторим тот же пример, но без опции _--ignore_, чтобы добавить три ранее проигнорированных файла:

```sh
precizer --update tests/examples/diffs
```

<sub>Database file name: myhost.db  
The database has already been created in the past  
Starting of database file myhost.db integrity check...  
The database myhost.db is in good condition  
The **--update** option has been used, so the information about files will be updated against the database myhost.db  
**These files have been added or changed and those changes will be reflected against the DB myhost.db:**  
diff2/1/AAA/BCB/CCC/a.txt adding  
diff2/1/AAA/ZAW/A/b/c/a_file.txt adding  
diff2/1/AAA/ZAW/D/e/f/b_file.txt adding  
Start vacuuming...  
The database has been vacuumed  
</sub>

### Пример 7

Продолжение предыдущего примера [Пример 6](#пример-6).

Несколько регулярных выражений для игнорирования можно использовать с помощью опций _--ignore_ указав их одновременно:

```sh
precizer --ignore="diff2/1/.*" --ignore="diff2/2/.*" tests/examples/diffs
```

### Пример 8

База данных будет очищена от упоминаний файлов, соответствующих регулярным выражениям из аргументов --ignore: "diff2/1/.\*" и "diff2/2/.\*"

Параметр _--db-clean-ignored_ должен быть указан дополнительно чтобы удалить из базы данных упоминание файлов, соответствующих регулярным выражениям, переданным через опции _--ignore_.


```sh
# Удалим старую базу данных и создадим новую, наполним ее данными:

rm "${HOST}.db"

precizer tests/examples/diffs
```

<sub>Database file name: myhost.db  
Starting of database file myhost.db integrity check...  
The database myhost.db is in good condition  
**These files will be added against the DB myhost.db:**  
diff1/3/AAA/BBB/CCC/a.txt  
diff1/path2/AAA/BCB/CCC/a.txt  
diff1/path2/AAA/ZAW/A/b/c/a_file.txt  
diff1/path2/AAA/ZAW/D/e/f/b_file.txt  
diff1/1/AAA/BCB/CCC/a.txt  
diff1/1/AAA/ZAW/A/b/c/a_file.txt  
diff1/1/AAA/ZAW/D/e/f/b_file.txt  
diff1/4/AAA/BBB/CCC/a.txt  
diff1/2/AAA/BBB/CZC/a.txt  
diff1/path1/AAA/ZAW/A/b/c/a_file.txt  
diff1/path1/AAA/ZAW/D/e/f/b_file.txt  
diff2/3/AAA/BBB/CCC/a.txt  
diff2/path2/AAA/BCB/CCC/a.txt  
diff2/path2/AAA/ZAW/A/b/c/a_file.txt  
diff2/1/AAA/BCB/CCC/a.txt  
diff2/1/AAA/ZAW/A/b/c/a_file.txt  
diff2/1/AAA/ZAW/D/e/f/b_file.txt  
diff2/4/AAA/BBB/CCC/a.txt  
diff2/2/AAA/BBB/CZC/a.txt  
diff2/path1/AAA/BCB/CCC/b.txt  
diff2/path1/AAA/BCB/CCC/a.txt  
diff2/path1/AAA/ZAW/A/b/c/a_file.txt  
diff2/path1/AAA/ZAW/D/e/f/b_file.txt  
Start vacuuming...  
The database has been vacuumed  
</sub>

```sh

# Обновить базу данных, удалив информацию о тех файлах, которые были указаны как игнорируемые:

precizer --update --db-clean-ignored \
	--ignore="diff2/1/.*" \
	--ignore="diff2/2/.*" \
	tests/examples/diffs
```
<sub>Database file name: myhost.db  
The database has already been created in the past  
Starting of database file myhost.db integrity check...  
The database myhost.db is in good condition  
**These files are ignored or no longer exist and will be deleted against the DB myhost.db:**  
clean ignored diff2/1/AAA/BCB/CCC/a.txt  
clean ignored diff2/1/AAA/ZAW/A/b/c/a_file.txt  
clean ignored diff2/1/AAA/ZAW/D/e/f/b_file.txt  
clean ignored diff2/2/AAA/BBB/CZC/a.txt  
Start vacuuming...  
The database has been vacuumed  
</sub>

### Example 9

Использование параметров _--ignore_ вместе с _--include_

```sh
# Удалим старую базу данных и создадим новую, наполним ее данными:

rm "${HOST}.db"

precizer tests/examples/diffs
```

<sub>Database file name: myhost.db  
Starting of database file myhost.db integrity check...  
The database myhost.db is in good condition  
**These files will be added against the DB myhost.db:**  
diff1/3/AAA/BBB/CCC/a.txt  
diff1/path2/AAA/BCB/CCC/a.txt  
diff1/path2/AAA/ZAW/A/b/c/a_file.txt  
diff1/path2/AAA/ZAW/D/e/f/b_file.txt  
diff1/1/AAA/BCB/CCC/a.txt  
diff1/1/AAA/ZAW/A/b/c/a_file.txt  
diff1/1/AAA/ZAW/D/e/f/b_file.txt  
diff1/4/AAA/BBB/CCC/a.txt  
diff1/2/AAA/BBB/CZC/a.txt  
diff1/path1/AAA/ZAW/A/b/c/a_file.txt  
diff1/path1/AAA/ZAW/D/e/f/b_file.txt  
diff2/3/AAA/BBB/CCC/a.txt  
diff2/path2/AAA/BCB/CCC/a.txt  
diff2/path2/AAA/ZAW/A/b/c/a_file.txt  
diff2/1/AAA/BCB/CCC/a.txt  
diff2/1/AAA/ZAW/A/b/c/a_file.txt  
diff2/1/AAA/ZAW/D/e/f/b_file.txt  
diff2/4/AAA/BBB/CCC/a.txt  
diff2/2/AAA/BBB/CZC/a.txt  
diff2/path1/AAA/BCB/CCC/b.txt  
diff2/path1/AAA/BCB/CCC/a.txt  
diff2/path1/AAA/ZAW/A/b/c/a_file.txt  
diff2/path1/AAA/ZAW/D/e/f/b_file.txt  
Start vacuuming...  
The database has been vacuumed  
</sub>

Регулярные выражения PCRE2 для относительных путей, которые необходимо включить. Включите указанные относительные пути, даже если они были исключены с помощью опции(ов) --ignore. Несколько регулярных выражений могут быть указаны с помощью --include

Чтобы проверить и протестировать регулярные выражения PCRE2 можно использовать ресурс https://regex101.com/

База данных будет очищена от упоминаний файлов, соответствующих регулярным выражениям из аргументов --ignore: "^.\*/path2/.\*" и "diff2/.\*" но пути, соответствующие шаблонам из _--include_ останутся в базе данных.

Параметр _--db-clean-ignored_ должен быть указан дополнительно чтобы удалить из базы данных упоминание файлов, соответствующих регулярным выражениям, переданным через опции _--ignore_.

```sh

# Обновить базу данных, удалив информацию о тех файлах, которые были указаны как игнорируемые за исключением шаблонов путей из _--include_

precizer --update --db-clean-ignored \
	--ignore="^.*/path2/.*" \
	--ignore="diff2/.*" \
	--include="diff2/1/AAA/ZAW/A/b/c/.*" \
	--include="diff2/path1/AAA/ZAW/.*" \
	tests/examples/diffs
```

<sub>Database file name: myhost.db  
Starting of database file myhost.db integrity check...  
The database myhost.db has been verified and is in good condition  
The **--update** option has been used, so the information about files will be updated against the database myhost.db  
**These files are ignored or no longer exist and will be deleted against the DB myhost.db:**  
clean ignored diff1/path2/AAA/BCB/CCC/a.txt  
clean ignored diff1/path2/AAA/ZAW/A/b/c/a_file.txt  
clean ignored diff1/path2/AAA/ZAW/D/e/f/b_file.txt  
clean ignored diff2/1/AAA/BCB/CCC/a.txt  
clean ignored diff2/1/AAA/ZAW/D/e/f/b_file.txt  
clean ignored diff2/2/AAA/BBB/CZC/a.txt  
clean ignored diff2/3/AAA/BBB/CCC/a.txt  
clean ignored diff2/4/AAA/BBB/CCC/a.txt  
clean ignored diff2/path1/AAA/BCB/CCC/a.txt  
clean ignored diff2/path1/AAA/BCB/CCC/b.txt  
clean ignored diff2/path2/AAA/BCB/CCC/a.txt  
clean ignored diff2/path2/AAA/ZAW/A/b/c/a_file.txt  
Start vacuuming...  
The database has been vacuumed  
</sub>
