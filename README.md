# fizzbuzz
Попытка написать fizzbuzz для n = 1e9 за 0.1 сек.
Вместо `/usr/bin/time` можно использовать любой другой способ засечь время
# Сборка и запуск
- вариант №1
```
clang++ main2.cpp -o main2 -Ofast -march=native
/usr/bin/time ./main2 > /dev/null
```

