#include <iostream>
#include <fstream>

#define MAXN 20000000
using namespace std;
bool a[MAXN];
bool b[MAXN];
int x;

int main() {
    freopen("out.txt", "w", stdout);

    freopen("1_1.txt", "r", stdin);

    while (!cin.eof()) {
        cin >> x;
        a[x] = true;
    }


//    cout << "----hi ifstream---" << endl;
    ifstream fin2("1_2.txt");
    if (fin2.is_open()) {
        while (!fin2.eof()) {
            fin2 >> x;
            b[x] = true;
        }
    }
    fin2.close();
//    cout << "-----bye ifstream----" << endl;


    int sum = 0;
    for (int i = 0; i < MAXN; i++)
        if (a[i] && b[i])
            sum++;


    cout << "The total number is: "<<sum << endl;
    for (int i = 0; i < MAXN; i++)
        if (a[i] && b[i])
            cout << i << endl;


}