# Сравнение работы хэш-функций и оптимизация работы поиска хэш-таблицы

***В рамках проекта были исследованы распределения различных хэш-функций и оптимизирован поиск по хэш-таблице с помощью различных приемов***

# Исследование хэш-функций

## Введение 

`Хэш-таблица` - структура данных, позволяющая хранить пары ключ-значение и выполнять три основные операции: вставку, поиск и удаление (в рамках исследования операция удаления не была реализована, так как в ней не было необходимости). Мы реализовали и использовали так называемую хэш-таблицу с цепочками, представляющую собой массив списков, в который помещали данные произвольного значения и длины, а именно слова из "Войны и Мира" Льва Толстого. `Хэш-функция` в свою очередь по переданному ей значению выдает ключ, соответствующий номеру списка, в котором он находится или в который его следует поместить. В общем смысле, `хэш-функция` - функция, сопоставляющая некоторому множеству байт значение.

В большинстве случаев недостаточно в качестве `хэш-функции` взять абсолютно любое отображение байт на числа. Так, в случае `хэш-таблицы`, учитывая специфику ее использования, поиск по элементам списка должен быть максимально быстрым, поэтому распределение элементов по спискам должно быть равномерным. Это означает, что "хорошая" (дающая равномерное распределение) `хэш-функция` определяется уникальностью полученных значений, их независимостью.

## Методы исследования 

В рамках исследования распределения на вход каждой функции было подано 24709 уникальных слова, а результат их выполнения был взят по модулю количества списков внутри хэш-таблицы, равному 499. В ходе работы со строками были рассмотрены следующие хэш-функции:
1. `size_t hash_ret_1 (char *string)` - `хэш-функция`, всегда возвращающая единицу;
2. `size_t hash_first_ch (char *string)` - `хэш-функция`, возвращающая ASCII-код первого символа;
3. `size_t hash_strlen (char *string)` - `хэш-функция`, возвращающая длину поданной на вход строки;
4. `size_t hash_ch_sum (char *string)` - `хэш-функция`, возвращающая сумму  ASCII-кодов строки;
5. `size_t hash_rotr (char *string)` - `хэш-функция`, которая начиная с нулевого значения возвращаемой переменной выполняет для каждого символа строки циклический сдвиг этой переменной вправо и ее побитовое отрицание с этим символом;
6. `size_t hash_rotl (char *string)` - аналогичная предыдущей `хэш-функция`, но выполняющая побитовый циклический сдвиг влево;
7. `size_t hash_crc64 (char *string)` - `хэш-функция` crc32 с 64-битной маской.

## Результаты 

*Замечание: на графиках представлено точное распределение хэш-функций в рамках исследования.*

Применение первой `хэш-функции` (см. рис 1) очевидно бесполезно, так как оно приводит к проведению операции поиска (и удаления) за линейное время, что можно наглядно увидеть на графике.

![](/data/histograms/histogram_of_1_func.png?raw=true)

`Хэш-функции`, возвращающие код первого символа и длину(см. рис 2), выдают на выходе больше значений, но все также приводят к большому количеству коллизий (несколько тысяч при количестве слов в ~25 тысяч слов) и, соответственно, к долгому поиску в ряде случаев.

![](/data/histograms/histogram_of_2_3_funcs.png?raw=true)

4-ая и 5-ая `хэi-функции` (см. рис 3) распределяют строки заметно лучше, чем ранее рассмотренные: хоть отклонения от среднего значения в ~50 значений на лист являются частыми, но все они небольшие относительно ранее увиденных, а именно, максимальное отклонение - примерно в 2 раза.

![](/data/histograms/histogram_of_4_5_funcs.png?raw=true)

Последние 2 `хэш-функции`(см. рис 4) показали наиболее хорошие распределения, которые в разы лучше чем у ранее рассмотренных: в худшем случае длина списка при их использовании не превышает ~1,4 от среднего количества слов на список, а в подавляющем большинстве списков лежит до 40-60 слов при среднем значении в 50 слов. Можно также заметить, что отклонения в большинстве случаев меньше в случае последней, `crc-64`, `хэш-функции` (поэтому поиск элементов в следующей части работы был произведен именно с ней).

![](/data/histograms/histogram_of_6_7_funcs.png?raw=true)


## Обсуждение и выводы 

Очевидно не всякое распределение можно назвать хэш-функцией, что мы наглядно показали, используя реальные данные и собственные хэш-таблицы.
От распределения хэш-функции напрямую зависит скорость работы хэш-таблицы, поэтому ее распределение должно стремится к равномерному (горизонтальной линии на графике) и поэтому в следующей части работы мы будем работать с наилучшей по результатам исследования хэш-функцией.

Можно сказать, что распределение первых 4 функций (см. рис. 1-2) сильно плохо, в то время как у остальных оно приемлемо (см. рис. 3) либо близко к равномерному (см. рис. 4). То есть все такие функции можно поделить на несколько групп в зависимости от равномерности их распределения, что мы и сделали  ранее.

# Оптимизация поиска по хэш-таблице

## Введение 

Поскольку хэш-таблицы используются для быстрого поиска среди большого количеству данных, то даже при использовании наилучшей хэш-функции необходимо оптимизировать работу хэш-таблицы во времязатратных местах. 

Для того чтобы найти те функции и команды, на выполнение которых затрачивается наибольшее время при поиске данных, воспользуемся профайлером `callgrind`, позволяющим увидеть время работы каждой вызванной функции в процентах от общего времени работы. На рис. 5 представлен результат работы профайлера для текущей версии программы.

- Отметим, что хэш-таблица не подразумевает частого добавления данных и используется в основном именно для поиска, поэтому мы произвели 5 миллионов итераций поиска случайных элементов, чтобы не учитывать время работы функций по заполнению таблицы. 
- Также теперь мы рассматриваем только одну хэш-таблицу с  `crc-64` `хэш-функцией`.
- Количество списков в `хэш-таблице` мы увеличили до 4999, чтобы в среднем в каждом из них лежало ~5 элементов и поиск по таблице производился быстрее.
- Так как проект является учебным, то мы выполним не менее трех оптимизаций с использованием различных методов: применение интринсик-функций, ассемблерная вставка, написание функции в отдельном ассемблерном файле. 
- Мы также провели "нулевую оптимизацию" программы, преобразовав все строки из `char *` в `char [32]` для более быстрого обращения к ним за счет их нахождения в кэше (предварительно выяснив что длина слова не превышает 25 символов).

|![](/data/dump_data/valgrind/start_valgrind_output.png?raw=true)|
|:--------------------------------------------------------------:|
|*рис.5 вывод программы `valgrind` до оптимизаций*                 |

## Методы исследования и результаты

Очевидно, что необходимо оптимизировать работу хэш-функции, так как на ее выполнение занимает более 60% всего времени. Заметим, что среди интринсик-функций существует функция `unsigned __int64 _mm_crc32_u64 (unsigned __int64 crc, unsigned __int64 v)`, преобразующая 8-байтное значение v по алгоритму crc32 (начиная со значения crc). Поэтому мы можем значительно ускорить работу нашей хэш-функции, переписав ее на основе `_mm_crc32_u64`, обрабатывая по алгоритму crc32 блоки по 8 байт одновременно.

Результат этой оптимизации в рамках профайлера представлен на рис.6: `хэш-функция` стала занимать более чем в три раза меньшую часть от времени работы всей программы.

|![](/data/dump_data/valgrind/valgrind_hash_optimized.png?raw=true)|
|:----------------------------------------------------------------:|
|*рис.6 вывод программы `valgrind` после оптимизации хэш-функции*  |

Проанализировав работу самой времязатратной на данный момент функции `hash_find` можно понять, что почти все время занимает линейный поиск по элементам списка. Ассемблерный код цикла после компиляции программы представлен на рис. 7. Видно, что при использовании флага -O0, этот цикл занимает большое количество кода и создает множество меток, перемещения между которыми заметно замедляют работу поиска в цикле. Для оптимизации небольшой части функции мы выбрали оптимизацию с помощью ассемблерной вставки: мы переписали код цикла, учитывая логику его работы и тем самым сократили количество прыжков по меткам и значительно сократили количество обращений к памяти (рис.8). Увидеть формальный результат можно применив профайлер к новой программе (рис.9): функция стала занимать 29% времени работы программы вместо прежних 36%.

|![](/data/dump_data/godbolt_hash_find.png?raw=true)      |
|:-------------------------------------------------------:|
|*рис.7 ассемблерный код цикла поиска функции `hash_find`*|

рис.8 код ассемблерной вставки

|![](/data/dump_data/asm_inline_code.png?raw=true)                   |
|:------------------------------------------------------------------:|
|*рис.8 код цикла поиска функции `hash_find` с ассемблерной вставкой*|

|![](/data/dump_data/valgrind/valgrind_hash_find_optimized.png?raw=true)|
|:---------------------------------------------------------------------:|
|*рис.9 вывод программы `valgrind` после оптимизации функции поиска*    |

Последнюю оптимизацию проведем с функцией сравнения строк `strmcp`, написав ее самостоятельной в отдельном ассемблерном файле и вызывая собственную функцию вместо стандартной. Учитывая то что мы храним строки в `char [32]` реализовать функцию можно без подсчета длины слов и также не сравнивая каждый байт отдельно, а сразу блоки по несколько байт в регистрах. По результатам профайлинга (рис.10) наша функция действительно работает быстрее стандартной. 

|![](/data/dump_data/valgrind/valgrind_strcmp_optimized.png?raw=true)      |
|:------------------------------------------------------------------------:|
|*рис.10 вывод программы `valgrind` после оптимизации функции `strcmp`*
(выделенная функция и является нашей функцией `strcmp`)    |

## Обсуждение 

Очевидно, что для полного понимания изменения времени работы программы работы профайлера недостаточно, также он не позволяет нам увидеть абсолютное время программы во всех случаях, что тоже довольно значимо. Мы измерили эти данные отдельно на каждом шаге, оставив количество итераций поиска равным 5 миллионов и сделав 20 запусков программы для получения более точных данных. Результаты этих измерений представлены в таблице ниже.

| Версия программы                       | Время работы, ms | Ускорение относительно начальной версии | Ускорение относительно предыдущей версии |
|:--------------------------------------:|:----------------:|:---------------------------------------:|:----------------------------------------:|
| Начальная                              | 1770 ± 21        | 1                                       |1                                         |
| С оптимизированной хэш-функцией        | 525 ± 3          | 3.37                                    |3.37                                      |
| С оптимизированной функцией поиска     | 509 ± 2          | 3.48                                    |1.03                                      |
| С оптимизированной функцией `strcmp`   | 473 ± 7          | 3.74                                    |1.08                                      |

Первая оптимизация ускорила работу программы в более чем три раза благодаря нахождению хэша сразу для блоков по 8 байт вместо прежнего побайтового.
Следующие оптимизации также ускорили работу всей программы на несколько процентов, что привело к общему ускорению практически в 4 раза. Полученные данные логически сходятся с данными из программы `valgrind`.

Также очень важно сравнить время работы программы, оптимизированной нами и оптимизированной компилятором, поэтому измерим время работы при запуске с флагами -O1, -O2, -O3 и сравним с ранее полученными данными.

| Версия программы                | Время работы | Ускорение/замедление относительно оптимизированной версии |
|:-------------------------------:|:------------:|:---------------------------------------------------------:|
| Оптимизированная нами           |473 ± 7       | 1                                                         |
| Начальная с флагом запуска -O1  |829 ± 5       | 0.57                                                      |
| Начальная с флагом запуска -O2  |650 ± 3       | 0.73                                                      |
| Начальная с флагом запуска -O3  |606 ± 9       | 0.78                                                      |


Видно, что наши оптимизации значительно лучше оптимизаций компилятора, по большей части благодаря оптимизации хэш-функции интринсик-функцией.
Если же запустить программу с флагами оптимизации компилятором с уже оптимизированной хэш-функцией, то получим следующие данные:


| Версия программы                | Время работы | Ускорение/замедление относительно оптимизированной версии |
|:-------------------------------:|:------------:|:---------------------------------------------------------:|
| Оптимизированная нами           |473 ± 7       | 1                                                         |
| Начальная с флагом запуска -O1  |442 ± 10      | 1.07                                                      |
| Начальная с флагом запуска -O2  |442 ± 7       | 1.07                                                      |
| Начальная с флагом запуска -O3  |438 ± 4       | 1.08                                                      |

которые практически не отличаются от полученных нами, но все же на несколько процентов лучше.

## Результаты 

- Для наилучшего абсолютного ускорения программы необходимо знать, какую часть времени программы занимает каждая функция, ее составляющие и особенности ее работы.
- В различных ситуациях оптимизации могут потребоваться различные методы(или просто будут смотреться лаконичнее в той или иной ситуации), среди которых мы рассмотрели и применили использование интринсик-функций, ассемблерную вставку, написание функции на ассемблере в отдельном файле. 
- В нашем случае благодаря оптимизациям нам удалось ускорить программу практически в 4 раза. Следовательно, такие оптимизации действительно полезны в случае слабой оптимизации компилятором, а в исключительных случаях (как в нашем) они могут превзойти и оптимизации O3.




