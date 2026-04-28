int t0 = 1;
float t1 = 2.4;
do{
    t0 = t0 + 1;
    printf("The value of t0 is %d", t0);
    printf("The value of t1 is %f", t1);
    printf("This is a type mismatch %d", t1);
    printf("So is this %f", t0);

    int t2 = t1 + t3;
    t0 = 0;
}while(t0 < 5);