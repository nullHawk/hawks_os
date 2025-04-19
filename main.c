int _start(){
    char* TM_START = (char*) 0xb8000;
    int i;
    char obj = 0;
    while(1){
        i = 0;
        while(i < (2 * 80 * 25)){
            *(TM_START + i) = obj;
            i++;
            obj++;
        }
    }
}