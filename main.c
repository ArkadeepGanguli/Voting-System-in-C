#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME_LEN 100

typedef struct {
    int id;                  // Candidate ID
    char name[MAX_NAME_LEN]; // Candidate Name
    int totalVotes;          // Total Votes Received
} Candidate;

typedef struct {
    int id;                  // Voter ID
    char name[MAX_NAME_LEN]; // Voter Name
    char password[MAX_NAME_LEN]; // Password
    int vote;                // Candidate ID or -1 if not voted
} Voter;

// Candidate data stored in memory
Candidate candidates[] = {
    {1, "Candidate A", 0},
    {2, "Candidate B", 0},
    {3, "Candidate C", 0}
};
int totalCandidates = sizeof(candidates) / sizeof(candidates[0]);

// Function Prototypes
void initializeVoters();
void initializeVoteFile();
void adminMenu();
void voterMenu();
int authenticateVoter(int id, char password[MAX_NAME_LEN]);
void castVote(int voterId);
void displayResults();
void saveVotesToFile();
void loadVotesFromFile();

int main() {
    int choice;

    initializeVoters();    // Initializes voters if no `voters.dat` file exists
    initializeVoteFile();  // Initializes `votes.dat` file for storing votes

    while (1) {
        printf("\nVoting System\n");
        printf("1. Admin Login\n");
        printf("2. Voter Login\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
        case 1:
            adminMenu();
            break;
        case 2:
            voterMenu();
            break;
        case 3:
            exit(0);
        default:
            printf("Invalid choice! Please try again.\n");
        }
    }
    return 0;
}

// Initialize voters.dat with sample data if it doesn't exist
void initializeVoters() {
    FILE *file = fopen("voters.dat", "rb");
    if (file != NULL) {
        fclose(file);
        return; // File already exists, no need to initialize
    }

    file = fopen("voters.dat", "wb");
    if (file == NULL) {
        perror("Error creating voters.dat");
        exit(1);
    }

    Voter voters[] = {
        {1, "Alice", "password123", -1},
        {2, "Bob", "mypassword", -1},
        {3, "Charlie", "securepass", -1}
    };
    int voterCount = sizeof(voters) / sizeof(voters[0]);
    fwrite(voters, sizeof(Voter), voterCount, file);
    fclose(file);
    printf("Voters initialized successfully.\n");
}

// Initialize votes.dat for secure vote storage
void initializeVoteFile() {
    FILE *file = fopen("votes.dat", "rb");
    if (file != NULL) {
        fclose(file);
        loadVotesFromFile(); // Load existing votes into memory
        return;
    }

    file = fopen("votes.dat", "wb");
    if (file == NULL) {
        perror("Error creating votes.dat");
        exit(1);
    }

    for (int i = 0; i < totalCandidates; i++) {
        fwrite(&candidates[i].totalVotes, sizeof(int), 1, file);
    }
    fclose(file);
    printf("Vote file initialized successfully.\n");
}

// Save votes to votes.dat in binary format
void saveVotesToFile() {
    FILE *file = fopen("votes.dat", "wb");
    if (file == NULL) {
        perror("Error writing to votes.dat");
        return;
    }

    for (int i = 0; i < totalCandidates; i++) {
        fwrite(&candidates[i].totalVotes, sizeof(int), 1, file);
    }
    fclose(file);
}

// Load votes from votes.dat into memory
void loadVotesFromFile() {
    FILE *file = fopen("votes.dat", "rb");
    if (file == NULL) {
        perror("Error reading from votes.dat");
        return;
    }

    for (int i = 0; i < totalCandidates; i++) {
        fread(&candidates[i].totalVotes, sizeof(int), 1, file);
    }
    fclose(file);
}

// Authenticate voter by ID and password
int authenticateVoter(int id, char password[MAX_NAME_LEN]) {
    FILE *file = fopen("voters.dat", "rb");
    if (file == NULL) {
        perror("Error opening voters.dat");
        return -1;
    }

    Voter voter;
    while (fread(&voter, sizeof(Voter), 1, file)) {
        if (voter.id == id && strcmp(voter.password, password) == 0) {
            fclose(file);
            return voter.id; // Authentication successful
        }
    }

    fclose(file);
    return -1; // Authentication failed
}

// Cast a vote for a candidate
void castVote(int voterId) {
    FILE *file = fopen("voters.dat", "rb+");
    if (file == NULL) {
        perror("Error opening voters.dat");
        return;
    }

    Voter voter;
    int found = 0;

    while (fread(&voter, sizeof(Voter), 1, file)) {
        if (voter.id == voterId) {
            found = 1;

            if (voter.vote != -1) {
                printf("You have already voted!\n");
                fclose(file);
                return;
            }

            printf("Candidates:\n");
            for (int i = 0; i < totalCandidates; i++) {
                printf("%d. %s\n", candidates[i].id, candidates[i].name);
            }

            int choice;
            printf("Enter the candidate ID to vote for: ");
            scanf("%d", &choice);

            int valid = 0;
            for (int i = 0; i < totalCandidates; i++) {
                if (candidates[i].id == choice) {
                    valid = 1;
                    candidates[i].totalVotes++;
                    saveVotesToFile(); // Save updated votes to file
                    break;
                }
            }

            if (!valid) {
                printf("Invalid candidate ID.\n");
            } else {
                voter.vote = choice;

                fseek(file, -(long)sizeof(Voter), SEEK_CUR);
                fwrite(&voter, sizeof(Voter), 1, file);
                printf("Vote cast successfully!\n");
            }
            break;
        }
    }

    if (!found) {
        printf("Voter not found.\n");
    }

    fclose(file);
}

// Admin menu for viewing results
void adminMenu() {
    char password[MAX_NAME_LEN];
    printf("Enter admin password: ");
    scanf("%s", password);

    if (strcmp(password, "admin123") == 0) {
        displayResults();
    } else {
        printf("Invalid admin password!\n");
    }
}

// Display election results
void displayResults() {
    printf("Election Results:\n");
    for (int i = 0; i < totalCandidates; i++) {
        printf("%s: %d votes\n", candidates[i].name, candidates[i].totalVotes);
    }
}

// Voter menu for authentication and voting
void voterMenu() {
    int id;
    char password[MAX_NAME_LEN];
    printf("Enter your voter ID: ");
    scanf("%d", &id);
    printf("Enter your password: ");
    scanf("%s", password);

    int voterId = authenticateVoter(id, password);
    if (voterId != -1) {
        printf("Login successful!\n");
        castVote(voterId);
    } else {
        printf("Invalid voter credentials.\n");
    }
}
