#include<iostream>
#include"util.h"
using namespace std;


void normalize2_column(mat& m){
    cout<<"do 2-order normalization..."<<endl;
    int n = m.size2();

    for(int i=0; i<n; i++){
        column ci(m, i);

        double norm2 = norm_2(ci);  if(norm2==0)   continue;

        ci /= norm2;
    }
}


void normalize2_row(mat& m){
    cout<<"do 2-order normalization..."<<endl;
    int n = m.size1();

    for(int i=0; i<n; i++){
        row ri(m, i);

        double norm2 = norm_2(ri);  if(norm2==0)   continue;

        ri /= norm2;
    }
}

bool is_zero_row(mat::iterator1 i){

    for(mat::iterator2 j=i.begin(); j!=i.end(); ++j)
        if(*j)  return false;

    return true;
}

shared_ptr<mat> read_rating(string ifname, int index, int nuser, int nitem, int nrating){
    cout<<"read rating..."<<endl;

    if(index)   swap(nuser, nitem);

    shared_ptr<mat> pm_rating(new mat(nuser, nitem, nrating));
    ifstream fin(ifname.c_str(), ios::in);

    while(!fin.eof()){
        int i[2];   double rate;

        fin>>i[0]>>i[1]>>rate;    if(fin.fail()) break;

        (*pm_rating)(i[index], i[1-index]) = rate;
    }
    fin.close();

    return pm_rating;
}

void remove_avg_row(mat& m){
    cout<<"remove row average..."<<endl;

    for(mat::iterator1 i=m.begin1(); i!=m.end1(); ++i){

        int count=0; double sum=0;

        for(mat::iterator2 j=i.begin(); j!=i.end(); ++j){

            count++; sum += *j;
        }

        if(count==0 || sum==0)  continue;
        double avg = sum / count; 

        for(mat::iterator2 j=i.begin(); j!=i.end(); ++j){
            *j -= avg;
        }
    }
}

