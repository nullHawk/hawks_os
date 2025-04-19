int _start(){
    char* cell = (char*) 0xb8000;
    int i = 1;
    while(i < (2 * 80 * 25)){
        *(cell + i) = 0xd5;
        i += 2;
    }
}