#include <iostream>
#include <string>
using namespace std;
int main()
{
     int count,i;
     string s;
     system("color 0C");
     cout<<"-----------------------"<<endl;
     cout<<"Cout- an exe by cjz2005"<<endl;
     cout<<"-----------------------"<<endl;
     cout<<"Input the text:"<<endl;
     cin>>s;
     cout<<"Input the count:"<<endl;
     cin>>count;
     cout<<"Loading..."<<endl;
     for(i=0;i<count;i++)
     cout<<s;
     cout<<"\n====================\n";
     cout<<"|    [Completed!]  |"<<endl;
     cout<<"====================\n";
     system("pause");
     return 0;
}
