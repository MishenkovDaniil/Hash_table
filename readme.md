# Сравнение работы хэш-функций и оптимизация работы поиска хэш-таблицы

***В рамках проекта были исследованы распределения различных хэш-функций и оптимизирован поиск по хэш-таблице с помощью различных приемов***

# Исследование хэш-функций

## Введение 

`Хэш-таблица` - структура данных, позволяющая хранить пары ключ-значение и выполнять три основные операции: вставку, поиск и удаление. В работе мы реализовали и использовали так называемую хэш-таблицу с цепочками, представляющую собой массив списков. Обращение к элементам хэш-таблицы происходит с помощью хэш-функции. `Хэш-функция`  по переданному ей значению выдает ключ, соответствующий номеру списка, в котором находится искомый объект или в который его следует поместить. В общем смысле, `хэш-функция` - функция, сопоставляющая некоторому множеству байт значение.

В большинстве случаев недостаточно в качестве `хэш-функции` взять абсолютно любое отображение байт на числа. Так, в случае `хэш-таблицы`, учитывая специфику ее использования, поиск по элементам списка должен быть максимально быстрым, поэтому распределение элементов по спискам должно быть равномерным. Это означает, что "хорошая" (дающая равномерное распределение) `хэш-функция` определяется уникальностью полученных значений, их независимостью.

## Методы исследования

Для качественного анализа распределения хэш-функций мы подавали им на вход данные произвольного значения и длины, а именно слова из "Войны и Мира" Льва Толстого. 

В рамках исследования на вход каждой функции было подано 24709 уникальных слова, а результат их выполнения был взят по модулю количества списков внутри `хэш-таблицы`, равному 4999. В ходе работы со строками были рассмотрены следующие хэш-функции:
1. `size_t hash_ret_1 (char *string)` - `хэш-функция`, возвращающая единицу;
2. `size_t hash_first_ch (char *string)` - `хэш-функция`, возвращающая ASCII-код первого символа;
3. `size_t hash_strlen (char *string)` - `хэш-функция`, возвращающая длину поданной на вход строки;
4. `size_t hash_ch_sum (char *string)` - `хэш-функция`, возвращающая сумму  ASCII-кодов строки;
5. `size_t hash_rotr (char *string)` - `хэш-функция`, которая сочетает циклический сдвиг вправо с побитовым отрицанием для каждого элемента строки;
6. `size_t hash_rotl (char *string)` - аналогичная предыдущей `хэш-функция`, но выполняющая побитовый циклический сдвиг влево;
7. `size_t hash_crc64 (char *string)` - `хэш-функция` crc32 с 64-битной маской.

По полученным распределениям хэш-функций были построены графики зависимости количества элементов от значения хэша, а также составлена сравнительная таблица, показывающая среднеквадратическое отклонение количества элементов на лист.

## Обсуждение и результаты 

Применение функции `hash_ret_1` (см. рис. 1) очевидно бесполезно, так как оно приводит к проведению операции поиска (и удаления) за линейное время, что можно наглядно увидеть на графике (справа на рис. 1 и рис. 2 показана наиболее значимая часть распределения в увеличенном масштабе).

| Хэш-функция     | Среднеквадратическое отклонение |
|:---------------:|:-------------------------------:|
| `hash_ret_1`    | 349.4                           |

![](/data/histograms/histogram_of_1_func.jpg?raw=true)

Применение функций `hash_first_ch` и `hash_strlen` (см. рис. 2) приводит к большому количеству коллизий (несколько тысяч при количестве слов в ~25 тысяч слов) и, соответственно, к недопустимо долгому поиску в ряде случаев, что подтверждается измерениями.

| Хэш-функция     | Среднеквадратическое отклонение |
|:---------------:|:-------------------------------:|
| `hash_ret_1`    | 349.4                           |
| `hash_first_ch` | 70.5                            |
| `hash_strlen`   | 117.3                           |

![](/data/histograms/histogram_of_2_3_funcs.jpg?raw=true)

Функция `hash_ch_sum` в силу особенностей своей работы распределяет значения преимущественно в небольшом диапазоне (рис. 3), а значит непригодна при больших размерах хэш-таблиц. Но так как даже в этом диапазоне данные сильно разнятся, то функция не подойдет и в случае малого размера хэш-таблицы. 

Распределение функции `hash_rotr` лучше чем у ранее рассмотренных функций, но оно все также очень неравномерное.

| Хэш-функция     | Среднеквадратическое отклонение |
|:---------------:|:-------------------------------:|
| `hash_ret_1`    | 349.4                           |
| `hash_first_ch` | 70.5                            |
| `hash_strlen`   | 117.3                           |
| `hash_ch_sum`   | 13.1                            |
| `hash_rotr`     | 10.7                            |

![](/data/histograms/histogram_of_4_5_funcs.png?raw=true)

Последние 2 `хэш-функции` (см. рис. 4) показали наиболее хорошие распределения. При этом  у `crc-64` наиболее равномерный график, где количество элементов на значение практически нигде не превышает 10. У функции `hash_rotl` большая часть графика также является равномерной, но все же присутствуют нередкие отклонения от среднего значения (в 5 элементов) в 3 - 4 раза и более.

| Хэш-функция     | Среднеквадратическое отклонение |
|:---------------:|:-------------------------------:|
| `hash_ret_1`    | 349.4                           |
| `hash_first_ch` | 70.5                            |
| `hash_strlen`   | 117.3                           |
| `hash_ch_sum`   | 13.1                            |
| `hash_rotr`     | 10.7                            |
| `hash_rotl`     | 3.4                             |
| `hash_crc64`    | 2.3                             |

![](/data/histograms/histogram_of_6_7_funcs.png?raw=true)


##  Выводы 

Очевидно не всякое распределение можно назвать хэш-функцией, что мы наглядно показали, используя реальные данные и собственные хэш-таблицы.
От распределения хэш-функции напрямую зависит скорость работы хэш-таблицы, поэтому ее распределение должно стремится к равномерному (горизонтальной линии на графике) и поэтому в следующей части работы мы будем работать с наилучшей по результатам исследования хэш-функцией - `crc-64`.


# Оптимизация поиска по хэш-таблице

Поскольку хэш-таблицы используются для быстрого поиска среди большого количества данных, то даже при использовании "хорошей" хэш-функции необходимо оптимизировать работу хэш-таблицы во времязатратных местах. 

## Методы исследования и результаты

***Замечание: на протяжении всего исследования при компиляции использовался флаг оптимизации `-O2`.***

Для того чтобы найти те функции и команды, на выполнение которых затрачивается наибольшее время при поиске данных, воспользуемся профайлером `callgrind`, позволяющим увидеть время работы каждой вызванной функции в процентах от общего времени работы. На рис. 5 представлен результат работы профайлера для текущей версии программы.

- Отметим, что наша `хэш-таблица` не подразумевает частого добавления данных и используется в основном именно для поиска, поэтому мы произвели 5 миллионов итераций поиска случайных элементов, чтобы не учитывать время заполнения таблицы. 
- Также теперь мы рассматриваем только одну хэш-таблицу с  `crc-64` `хэш-функцией`.
- Количество списков в `хэш-таблице` мы оставили равным 4999, так как в этом случае в каждом из них будет лежать ~5 элементов и поиск по ним будет быстрым.
- Так как проект является учебным, то мы выполним не менее трех оптимизаций с использованием различных методов: применение интринсик-функций, ассемблерная вставка, написание функции в отдельном ассемблерном файле. 
- Мы также провели "нулевую оптимизацию" программы, преобразовав все строки из `char *` в `char [32]` (предварительно выяснив что длина слова не превышает 31 символ) для более быстрого обращения к ним за счет их нахождения в кэше и для будущего применения этого факта в наших оптимизациях. 
- По мере проведения оптимизаций мы будем заполнять таблицу, показывающую ускорение абсолютного времени работы программы, которая представлена ниже (подробные сведения о таблице можно найти в разделе "обсуждение").


| Версия программы                       | Время работы, ms | Ускорение относительно начальной версии | Ускорение относительно предыдущей версии |
|:--------------------------------------:|:----------------:|:---------------------------------------:|:----------------------------------------:|
| Начальная                              | 658.8 ± 2.7      | 1                                       |1                                         |

|![](/data/dump_data/valgrind/start_valgrind_output.png?raw=true)|
|:--------------------------------------------------------------:|
|*Рис.5. Вывод программы `valgrind` до оптимизаций.*             |


Очевидно, что необходимо оптимизировать работу хэш-функции, так как ее выполнение занимает более 80% всего времени. Заметим, что среди интринсик-функций существует функция `unsigned __int64 _mm_crc32_u64 (unsigned __int64 crc, unsigned __int64 v)`, преобразующая 8-байтное значение v по алгоритму crc32 (начиная со значения crc). Поэтому мы можем значительно ускорить работу нашей хэш-функции, переписав ее на основе `_mm_crc32_u64`, обрабатывая по алгоритму crc32 блоки по 8 байт одновременно.

Результат этой оптимизации в рамках профайлера представлен на рис.6: `хэш-функция` стала занимать менее 10% от времени работы всей программы, а абсолютное время работы уменьшилось почти вдвое.

| Версия программы                       | Время работы, ms | Ускорение относительно начальной версии | Ускорение относительно предыдущей версии |
|:--------------------------------------:|:----------------:|:---------------------------------------:|:----------------------------------------:|
| Начальная                              | 658.8 ± 2.7      | 1                                       |1                                         |
| С оптимизированной хэш-функцией        | 361.0 ± 3.5      | 1.82                                    |1.82                                      |


|![](/data/dump_data/valgrind/valgrind_hash_optimized.png?raw=true)|
|:----------------------------------------------------------------:|
|*Рис.6. Вывод программы `valgrind` после оптимизации хэш-функции.*|

Следующую оптимизацию проведем с функцией сравнения строк `strcmp`, (занимающую наибольшее время по итогам последнего профайлинга) написав ее самостоятельно в отдельном ассемблерном файле и вызывая вместо стандартной. Учитывая то что мы храним строки в `char [32]`, реализовать функцию можно без подсчета длины слов и сравнивая не каждый байт отдельно, а сразу блоки по 8 байт в регистрах (ниже представлена реализация этой функции на языке ассемблера `nasm`). 

```nasm
m_strcmp: 
            mov rax, qword [rsi] ; rsi = first string addr
            sub rax, qword [rdi] ; rdi = second string addr
            jne .end 

            mov rax, qword [rsi + 8]
            sub rax, qword [rdi + 8]
            jne .end 

            mov rax, qword [rsi + 16]
            sub rax, qword [rdi + 16]
            jne .end
            
            mov rax, qword [rsi + 24]
            sub rax, qword [rdi + 24]

.end:
            ret             ;return rax
```


По результатам профайлинга (рис.7) наша функция действительно работает быстрее стандартной, а абсолютное время выполнения всей программы уменьшилось на 30%.

| Версия программы                       | Время работы, ms | Ускорение относительно начальной версии | Ускорение относительно предыдущей версии |
|:--------------------------------------:|:----------------:|:---------------------------------------:|:----------------------------------------:|
| Начальная                              | 658.8 ± 2.7      | 1                                       |1                                         |
| С оптимизированной хэш-функцией        | 361.0 ± 3.5      | 1.82                                    |1.82                                      |
| С оптимизированной функцией `strcmp`   | 278.3 ± 3.2      | 2.37                                    |1.30                                      |

|![](/data/dump_data/valgrind/valgrind_strcmp_optimized.png?raw=true)      |
|:------------------------------------------------------------------------:|
|*Рис.7. Вывод программы `valgrind` после оптимизации функции `strcmp`*
*(выделенная функция и является нашей функцией `strcmp`).*    |

Проанализировав работу самой времязатратной на данный момент функции `hash_find` можно понять, что почти все время занимает линейный поиск по элементам списка. Чтобы ускорить ее мы с помощью ассемблерной вставки заинлайним написанную ранее функцию сравнения строк, тем самым избавившись от медленных операций `call` и `ret` (`call` - медленная, так как помимо перехода по адресу выполняет `push` возвращаемого значения в стэк, а  `ret` - так как приостанавливает работу конвейера до момента вычисления адреса возврата).

С помощью программы `godbolt` удостоверимся в том что наша функция станет работать быстрее: рассмотрим ассемблерный код функции поиска до оптимизации (рис. 8) и после нее (рис. 9). Мы видим, что: 
- Структура функции не ухудшилась после вставки.
- Мы смогли уменьшить количество пересылок между регистрами за счет появления возможности хранения строк в любых регистрах. 
- Вставка привела к лучшей оптимизации встроенным компилятором, так как теперь при выходе из программы производится на один `pop` меньше.

|![](/data/dump_data/godbolt_hash_find_not_opt.png?raw=true)       |
|:--------------------------------------------------------:|
|*Рис.8. Ассемблерный код цикла функции поиска `hash_find`.*|

|![](/data/dump_data/godbolt_hash_find_opt.png?raw=true)                    |
|:-------------------------------------------------------------------:|
|*Рис.9. Ассемблерный код цикла функции поиска `hash_find` с ассемблерной вставкой.*|

Увидеть формальный результат можно применив профайлер к новой программе (рис.10): функция поиска стала занимать 72% времени работы программы вместо прежних 76% (здесь мы просуммировали время работы функции поиска и функции сравнения строк).

| Версия программы                       | Время работы, ms | Ускорение относительно начальной версии | Ускорение относительно предыдущей версии |
|:--------------------------------------:|:----------------:|:---------------------------------------:|:----------------------------------------:|
| Начальная                              | 658.8 ± 2.7      | 1                                       |1                                         |
| С оптимизированной хэш-функцией        | 361.0 ± 3.5      | 1.82                                    |1.82                                      |
| С оптимизированной функцией `strcmp`   | 278.3 ± 3.2      | 2.37                                    |1.30                                      |

|![](/data/dump_data/valgrind/valgrind_hash_find_optimized.png?raw=true)|
|:---------------------------------------------------------------------:|
|*Рис.10. Вывод программы `valgrind` после оптимизации функции поиска.*  |

Так как относительно прежней версии программы мы получили ускорение абсолютного времени работы всего на 4%, то проведение последующих оптимизаций будет нецелесообразно. 

## Обсуждение 

Рассмотрим еще раз таблицу, показывающую абсолютное время работы программы. Мы измеряли данные отдельно на каждом шаге, оставив количество итераций поиска равным 5 миллионов и сделав 20 запусков программы для получения более точных данных и подсчета среднеквадратического отклонения результатов.

| Версия программы                       | Время работы, ms | Ускорение относительно начальной версии | Ускорение относительно предыдущей версии |
|:--------------------------------------:|:----------------:|:---------------------------------------:|:----------------------------------------:|
| Начальная                              | 658.8 ± 2.7      | 1                                       |1                                         |
| С оптимизированной хэш-функцией        | 361.0 ± 3.5      | 1.82                                    |1.82                                      |
| С оптимизированной функцией `strcmp`   | 278.3 ± 3.2      | 2.37                                    |1.30                                      |
| С оптимизированной функцией поиска     | 267.2 ± 1.7      | 2.47                                    |1.04                                      |

Как видно из таблицы, первые две оптимизации ускорили программу относительно предыдущей довольно сильно, на 82 и 30 процентов соответственно, а последняя на четыре. По сравнению с начальной версией программы мы получили ускорение практически в два с половиной раза, что является хорошим результатом.

На самом деле последний результат является ожидаемым, ведь оптимизация не изменяла алгоритм работы в корне, а лишь убрала несколько ассемблерных команд из кода. 

Заметим также, что табличные данные логически сходятся с данными из программы `valgrind`.

Стоит также заметить, что дальнейшая оптимизация посредством компилятора (с флагом `-O3`, к примеру) ускоряет программу лишь на несколько процентов, так как проведенные нами оптимизации были не универсальными, как в случае компилятора, а учитывали особенности работы конкретной программы (наиболее значимая из них - использование `char[32]` вместо `char *`).

## Результаты 

- Для наилучшего абсолютного ускорения программы необходимо знать, какую часть времени программы занимает каждая функция, ее составляющие и особенности ее работы.
- В различных ситуациях, требующих оптимизации, могут потребоваться различные ее методы(или же они просто будут смотреться лаконичнее), среди которых мы рассмотрели и применили использование интринсик-функций, ассемблерную вставку, написание функции на ассемблере в отдельном файле. 
- В нашем случае благодаря оптимизациям нам удалось ускорить программу практически в 2 с половиной раза. Следовательно, такие оптимизации действительно полезны в случае слабой оптимизации компилятором.

