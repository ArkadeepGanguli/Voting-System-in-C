#include <stdio.h>
#include <string.h>

#define MAX_NAME_LEN 100

typedef struct {
    int id;  // Voter ID
    char name[MAX_NAME_LEN];  // Voter name
    char password[MAX_NAME_LEN];  // Voter password
    int vote;  // -1 = not voted
} Voter;

void createVoters() {
    FILE *file = fopen("voters.dat", "wb");
    if (file == NULL) {
        perror("Error creating voters.dat");
        return;
    }

    Voter voters[] = {
        {1, "Alice", "password123", -1},
        {2, "Bob", "mypassword", -1},
        {3, "Charlie", "securepass", -1}
    };
    int voterCount = sizeof(voters) / sizeof(voters[0]);

    fwrite(voters, sizeof(Voter), voterCount, file);
    fclose(file);
    printf("Voters.dat file created successfully.\n");
}

int main() {
    createVoters();
    return 0;
}
