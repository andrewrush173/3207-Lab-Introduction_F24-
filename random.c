#include <stdio.h>
#include <stdlib.h>
#include <time.h>

char randchar() {
    return 'A' + (rand() % 26);
}

void rand_string(char *s, size_t size) {
    for (size_t i = 0; i < size; i++) {
        s[i] = randchar();  
    }
    s[size] = '\0';  
}

char* rand_string_alloc(size_t size) {
    char *s = malloc(size + 1);  
    if (s) {
        rand_string(s, size);  
    }
    return s;
}

int main() {
    
    srand(time(NULL));


    size_t string_size = 10;  
    char *random_str = rand_string_alloc(string_size);

    if (random_str) {
        printf("Random string: %s\n", random_str);
        free(random_str); 
    } else {
        printf("Memory allocation failed.\n");
    }

    return 0;
}

