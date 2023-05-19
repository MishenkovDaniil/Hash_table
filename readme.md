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

По полученным распределениям хэш-функций были построены графики зависимости количества элементов от значения хэша.

## Обсуждение и результаты 

Применение функции `hash_ret_1` (см. рис. 1) очевидно бесполезно, так как оно приводит к проведению операции поиска (и удаления) за линейное время, что можно наглядно увидеть на графике.

![](/data/histograms/histogram_of_1_func.png?raw=true)

Применение функций `hash_first_ch` и `hash_strlen` (см. рис. 2) приводят к большому количеству коллизий (несколько тысяч при количестве слов в ~25 тысяч слов) и, соответственно, к недопустимо долгому поиску в ряде случаев.

![](/data/histograms/histogram_of_2_3_funcs.png?raw=true)


Функция `hash_ch_sum` в силу особенностей своей работы распределяет значения преимущественно в небольшом диапазоне (рис. 3), а значит непригодна при больших размерах хэш-таблиц. Но так как даже в этом диапазоне данные сильно разнятся, то функция не подойдет и в случае малого размера хэш-таблицы. 

Распределение функции `hash_rotr` лучше чем у ранее рассмотренных функций, но оно все также очень неравномерное.

![](/data/histograms/histogram_of_4_5_funcs.png?raw=true)


Последние 2 `хэш-функции` (см. рис. 4) показали наиболее хорошие распределения. При этом  у `crc-64` наиболее равномерный график, где количество элементов на значение практически нигде не превышает 10. У функции `hash_rotl` большая часть графика также является равномерной, но все же присутствуют нередкие отклонения от среднего значения (в 5 элементов) в 3 - 4 раза и более.

![](/data/histograms/histogram_of_6_7_funcs.png?raw=true)


##  Выводы 

Очевидно не всякое распределение можно назвать хэш-функцией, что мы наглядно показали, используя реальные данные и собственные хэш-таблицы.
От распределения хэш-функции напрямую зависит скорость работы хэш-таблицы, поэтому ее распределение должно стремится к равномерному (горизонтальной линии на графике) и поэтому в следующей части работы мы будем работать с наилучшей по результатам исследования хэш-функцией.


# Оптимизация поиска по хэш-таблице

Поскольку хэш-таблицы используются для быстрого поиска среди большого количества данных, то даже при использовании "хорошей" хэш-функции необходимо оптимизировать работу хэш-таблицы во времязатратных местах. 

## Методы исследования и результаты

***Замечание: на протяжении всего исследования при компиляции использовался флаг оптимизации -O1.***

Для того чтобы найти те функции и команды, на выполнение которых затрачивается наибольшее время при поиске данных, воспользуемся профайлером `callgrind`, позволяющим увидеть время работы каждой вызванной функции в процентах от общего времени работы. На рис. 5 представлен результат работы профайлера для текущей версии программы.

- Отметим, что `хэш-таблица` не подразумевает частого добавления данных и используется в основном именно для поиска, поэтому мы произвели 5 миллионов итераций поиска случайных элементов, чтобы не учитывать время заполнения таблицы. 
- Также теперь мы рассматриваем только одну хэш-таблицу с  `crc-64` `хэш-функцией`.
- Количество списков в `хэш-таблице` мы оставили равным 4999, так как в этом случае в каждом из них будет лежать ~5 элементов и поиск по ним будет быстрым.
- Так как проект является учебным, то мы выполним не менее трех оптимизаций с использованием различных методов: применение интринсик-функций, ассемблерная вставка, написание функции в отдельном ассемблерном файле. 
- Мы также провели "нулевую оптимизацию" программы, преобразовав все строки из `char *` в `char [32]` (предварительно выяснив что длина слова не превышает 31 символ) для более быстрого обращения к ним за счет их нахождения в кэше и для будущего применения этого факта в наших оптимизациях. 

|![](/data/dump_data/valgrind/start_valgrind_output.png?raw=true)|
|:--------------------------------------------------------------:|
|*Рис.5 вывод программы `valgrind` до оптимизаций*               |


Очевидно, что необходимо оптимизировать работу хэш-функции, так как ее выполнение занимает более 80% всего времени. Заметим, что среди интринсик-функций существует функция `unsigned __int64 _mm_crc32_u64 (unsigned __int64 crc, unsigned __int64 v)`, преобразующая 8-байтное значение v по алгоритму crc32 (начиная со значения crc). Поэтому мы можем значительно ускорить работу нашей хэш-функции, переписав ее на основе `_mm_crc32_u64`, обрабатывая по алгоритму crc32 блоки по 8 байт одновременно.

Результат этой оптимизации в рамках профайлера представлен на рис.6: `хэш-функция` стала занимать менее 10% от времени работы всей программы.

|![](/data/dump_data/valgrind/valgrind_hash_optimized.png?raw=true)|
|:----------------------------------------------------------------:|
|*Рис.6 вывод программы `valgrind` после оптимизации хэш-функции*  |

Следующую оптимизацию проведем с функцией сравнения строк `strcmp`, (занимающую наибольшее время по итогам оследнего профайлинга) написав ее самостоятельно в отдельном ассемблерном файле и вызывая вместо стандартной. Учитывая то что мы храним строки в `char [32]`, реализовать функцию можно без подсчета длины слов и сравнивая не каждый байт отдельно, а сразу блоки по 8 байт в регистрах (ниже представлена реализация этой функции на языке ассемблера `nasm`). По результатам профайлинга (рис.7) наша функция действительно работает быстрее стандартной. 


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

|![](/data/dump_data/valgrind/valgrind_strcmp_optimized.png?raw=true)      |
|:------------------------------------------------------------------------:|
|*Рис.7 вывод программы `valgrind` после оптимизации функции `strcmp`*
*(выделенная функция и является нашей функцией `strcmp`).*    |

## Обсуждение 

Очевидно, что для полного понимания изменения времени работы программы работы профайлера недостаточно, также он не позволяет нам увидеть абсолютное время программы во всех случаях, что тоже довольно значимо. Мы измерили эти данные отдельно на каждом шаге, оставив количество итераций поиска равным 5 миллионов и сделав 20 запусков программы для получения более точных данных и подсчета среднеквадратического отклонения результатов. Результаты этих измерений представлены в таблице ниже.

| Версия программы                       | Время работы, ms | Ускорение относительно начальной версии | Ускорение относительно предыдущей версии |
|:--------------------------------------:|:----------------:|:---------------------------------------:|:----------------------------------------:|
| Начальная                              | 650.0 ± 3.3      | 1                                       |1                                         |
| С оптимизированной хэш-функцией        | 351.9 ± 3.2      | 1.85                                    |1.85                                      |
| С оптимизированной функцией `strcmp`   | 273.5 ± 2.9      | 2.38                                    |1.29                                      |
| С оптимизированной функцией поиска     | -                | -                                       |-                                         |

По сравнению с начальной версией программы мы получили ускорение более чем в два раз, что является хорошим результатом.
Табличные данные логически сходятся с данными из программы `valgrind`.


Стоит также заметить, что оптимизация посредством компилятора (с флагом -O3, к примеру) ускоряет программы лишь на несколько процентов, так как проведенные нами оптимизации были не универсальными, как в случае компилятора, а учитывали особенности работы конкретной программы (наиболее значимая из них - использование char[32] вместо char *).

## Результаты 

- Для наилучшего абсолютного ускорения программы необходимо знать, какую часть времени программы занимает каждая функция, ее составляющие и особенности ее работы.
- В различных ситуациях, требующих оптимизации, могут потребоваться различные ее методы(или же они просто будут смотреться лаконичнее), среди которых мы рассмотрели и применили использование интринсик-функций, ассемблерную вставку, написание функции на ассемблере в отдельном файле. 
- В нашем случае благодаря оптимизациям нам удалось ускорить программу практически в 2 с половиной раза. Следовательно, такие оптимизации действительно полезны в случае слабой оптимизации компилятором.



