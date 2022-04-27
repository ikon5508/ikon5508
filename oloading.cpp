#include <cstdio>
#include <cstdarg>
#include <cstring>
//#include <cstdlib>
#include <iostream>


class myclass {
    public:
    
    void myfunc1 (int arg1, ...)
    {
        printf ("myfunc1\n");
    }
    
    
    void myfunc2 (int arg1)
    {
        printf ("myfunc2\n");
    }
    
 void operator << (const char *) {
     
 }   
    
};
typedef class myclass myclass_t;
   using namespace std;

class Test {
    private: 
    char data [500]; 
 
 
    public: 


    Test() { memset (data, 0, 500); }
 
 
    void print()
    { cout << data; }


    Test& operator<<(int y) 
    { 
        sprintf (data + strlen(data) , "%d",  y);
        return *this;
  }
    
        Test& operator<<(const char *y) 
    { 
        strcat (data, y);
        return *this;
  }
  
    
    };




void func (int arg1, ...)
{
    printf ("func\n");
}

int main() {

/*
myclass_t c1;

c1.myfunc1 (1, 3, 5);
c1.myfunc2 (2, 3, 5);
*/
class Test w;
w << 40 << 40 << "test";
w.print();

}
