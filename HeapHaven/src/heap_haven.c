#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_CHUNKS 16
#define CHUNK_SIZE 0x80

typedef struct {
    void *ptr;
    size_t size;
    int in_use;
} chunk_t;

chunk_t chunks[MAX_CHUNKS];

void setup() {
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
    
    // Print banner
    puts("=========================================");
    puts("         Heap Haven v1.0");
    puts("    Secure Memory Management System");
    puts("=========================================");
}

void win() {
    // Hidden function - prints the flag
    char flag[64];
    FILE *f = fopen("flag.txt", "r");
    if (f == NULL) {
        puts("[!] Flag file not found. Contact admin.");
        return;
    }
    fgets(flag, sizeof(flag), f);
    fclose(f);
    puts("\n[+] Congratulations! Here's your flag:");
    printf("%s\n", flag);
}

void print_menu() {
    puts("\n=== Main Menu ===");
    puts("1. Allocate chunk");
    puts("2. Edit chunk");
    puts("3. View chunk");
    puts("4. Free chunk");
    puts("5. Exit");
    printf("Choice: ");
}

void allocate_chunk() {
    int idx;
    size_t size;
    
    printf("Enter index (0-15): ");
    scanf("%d", &idx);
    
    if (idx < 0 || idx >= MAX_CHUNKS) {
        puts("[!] Invalid index!");
        return;
    }
    
    if (chunks[idx].in_use) {
        puts("[!] Chunk already allocated!");
        return;
    }
    
    printf("Enter size: ");
    scanf("%zu", &size);
    
    // Vulnerability: No size validation! Can allocate any size
    chunks[idx].ptr = malloc(size);
    chunks[idx].size = size;
    chunks[idx].in_use = 1;
    
    printf("[+] Allocated chunk %d of size %zu at %p\n", idx, size, chunks[idx].ptr);
    
    printf("Enter data: ");
    read(0, chunks[idx].ptr, size);
}

void edit_chunk() {
    int idx;
    size_t new_size;
    
    printf("Enter index (0-15): ");
    scanf("%d", &idx);
    
    if (idx < 0 || idx >= MAX_CHUNKS) {
        puts("[!] Invalid index!");
        return;
    }
    
    if (!chunks[idx].in_use) {
        puts("[!] Chunk not allocated!");
        return;
    }
    
    printf("Enter new size: ");
    scanf("%zu", &new_size);
    
    // Vulnerability: Realloc without checking
    // If new_size == 0, realloc behaves like free()
    // Also realloc can move chunks, causing UAF if we keep old pointer
    void *new_ptr = realloc(chunks[idx].ptr, new_size);
    
    if (new_ptr == NULL && new_size != 0) {
        puts("[!] Realloc failed!");
        return;
    }
    
    // Vulnerability: We update pointer even if realloc moved it
    chunks[idx].ptr = new_ptr;
    chunks[idx].size = new_size;
    
    if (new_size > 0) {
        printf("Enter new data: ");
        read(0, chunks[idx].ptr, new_size);
    }
    
    puts("[+] Chunk updated");
}

void view_chunk() {
    int idx;
    
    printf("Enter index (0-15): ");
    scanf("%d", &idx);
    
    if (idx < 0 || idx >= MAX_CHUNKS) {
        puts("[!] Invalid index!");
        return;
    }
    
    if (!chunks[idx].in_use) {
        puts("[!] Chunk not allocated!");
        return;
    }
    
    // Vulnerability: Can view freed chunks if in_use is incorrectly set
    if (chunks[idx].ptr == NULL) {
        puts("[!] Chunk pointer is NULL!");
        return;
    }
    
    printf("Data: ");
    write(1, chunks[idx].ptr, chunks[idx].size);
    puts("");
}

void free_chunk() {
    int idx;
    
    printf("Enter index (0-15): ");
    scanf("%d", &idx);
    
    if (idx < 0 || idx >= MAX_CHUNKS) {
        puts("[!] Invalid index!");
        return;
    }
    
    if (!chunks[idx].in_use) {
        puts("[!] Chunk not allocated!");
        return;
    }
    
    // Vulnerability: Double free possible if we don't check properly
    free(chunks[idx].ptr);
    
    // Vulnerability: Only mark as not in_use, but keep pointer!
    // Use-after-free vulnerability!
    chunks[idx].in_use = 0;
    
    puts("[+] Chunk freed");
}

int main() {
    int choice;
    
    setup();
    
    while (1) {
        print_menu();
        scanf("%d", &choice);
        
        switch (choice) {
            case 1:
                allocate_chunk();
                break;
            case 2:
                edit_chunk();
                break;
            case 3:
                view_chunk();
                break;
            case 4:
                free_chunk();
                break;
            case 5:
                puts("[+] Goodbye!");
                exit(0);
            default:
                puts("[!] Invalid choice!");
        }
    }
    
    return 0;
}