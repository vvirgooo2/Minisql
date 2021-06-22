#include <bits/stdc++.h>
using namespace std;


int main(int argc, char* argv[])
{
	filebuf f;
    f.open("1", ios::in | ios::out);
    if(!f.is_open())
    {
        cout << "Fail!" << endl;
        ofstream new_f("1");
        new_f.close();
        f.open("1", ios::in | ios::out);
    }
    istream fin(&f);
    ostream fout(&f);
    
    return 0;
}