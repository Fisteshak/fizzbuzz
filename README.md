# fizzbuzz
Попытка написать fizzbuzz для n = 1e9 за 0.1 сек.
Вместо `/usr/bin/time` можно использовать любой другой способ засечь время
# Сборка и запуск
```
clang++ mt2.cpp -o mt2 -Ofast -march=native
/usr/bin/time ./mt2 > /dev/null
```

