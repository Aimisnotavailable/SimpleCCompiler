int t0 = 1;
float t1 = 2.4;
do{
    int t0 = 2;
    t0 = t0 + 1;
    printf("%d", t0); 
    printf("%f", t1);
    printf("%d", t1); // Type Mismatch
    printf("%f", t0); // Type Mismatch

    int t2 = t1 + t3; // Undeclared Variable
    t0 = 0;
}while(t0 < 1);