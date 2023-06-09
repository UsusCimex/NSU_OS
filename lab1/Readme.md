# Компиляция, сборка, запуск

## Задание:
### 1. Написать программу hello.c, которая выводит фразу “Hello world”:
#### a. получить исполняемый файл;
#### b. посмотреть unresolved symbols (puts, printf) с помощью nm;
#### c. посмотреть зависимости (ldd);
#### d. запустить.
### 2. Написать статическую библиотеку с функцией hello_from_static_lib() и использовать ее в hello.c:
#### a. посмотреть исполняемый файл на предмет того будет ли функция hello_from_static_lib() unresolved. Почему?
#### b. где находится код этой функции?
### 3. Написать динамическую библиотеку с функцией hello_from_dynamic_lib() и использовать ее с hello.c:
#### a. посмотреть состояние функции hello_from_dynamic_lib в получившимся исполняемом файле. Объяснить увиденное.
### 4. Написать динамическую библиотеку с функцией hello_from_dyn_runtime_lib() и загрузить ее в hello.c с помощью dlopen(3). Объяснить что происходит.

## FAQ
### hello.c - файл с различными методами вывода "hello world!"
### hello_(static,dynamic,runtime).c - библиотечные файлы, компиляция которых происходит следующим образом:
#### static:
```
gcc -c [список *.c файлов]
ar rc <lib*.a> [список *.o файлов]
```

#### dynamic, runtime:
```
gcc -shared -fPIC <*.c> -o <*.so>
```

### Компиляция исходного кода, происводится следующим способом:
```
gcc -L. [список -l<libname> библиотек] <*.c>
```