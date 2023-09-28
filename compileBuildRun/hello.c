#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

int main(int argc, char** argv) {
    printf("Hello world!\n");
    // hello_from_static_lib();
    // hello_from_dynamic_lib();

    if (argc != 2) { printf("dlopen won't say hello world D;\n"); return 0; }
    char *libPatch = argv[1];
    void *libHandle = dlopen(libPatch, RTLD_LAZY);

    static const int stat = 10; 
    int a = 12;
    int *b = (int*)malloc(sizeof(int));

    printf("stat(static) = %p\n a(stack) = %p\n b(Heap) = %p\n libHandle = %p\n", &stat, &a, b, libHandle);

    free(b);

    if (libHandle == NULL) { printf("Library (%s) don't searched\n", libPatch); return 0; }

    void (*hello_from_dynamic_runtime_lib)();
    hello_from_dynamic_runtime_lib = dlsym(libHandle, "printTEST"); //testName
    printf("hello_func = %p\n", hello_from_dynamic_runtime_lib);
    if (hello_from_dynamic_runtime_lib == NULL) { printf("Hello func don't searched\n"); return 0; }
    hello_from_dynamic_runtime_lib();

    dlclose(libHandle);

    return 0;
}