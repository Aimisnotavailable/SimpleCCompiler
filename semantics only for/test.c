int x = 5;
float z = 2.4;

for(int i = 0; i < 5; i = i + 1){

    printf("%d", x); 
    printf("%f", z);
    printf("%d", z); // Type Mismatch
    printf("%f", x); // Type Mismatch

    int c = x + z; // Undeclared Variable
}
int i = 1; // Valid since out of scope
int c = 2; // Valid since out of scope

int x = 5; // Redeclared Variable