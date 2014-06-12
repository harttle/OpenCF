#ifndef UTIL_H
#define UTIL_H

#include<iostream>
#include<fstream>
#include<queue>
#include<map>

#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/io.hpp>
namespace nu = boost::numeric::ublas;


using namespace std;

typedef nu::compressed_matrix<double> mat;
typedef nu::matrix_row<mat> row;
typedef const nu::matrix_row<const mat> crow;
typedef nu::matrix_column<mat> column;


// ostream << for map
template<typename T1,typename T2>
ostream& operator<<(ostream& o, map<T1, T2> m){
    for(typename map<T1,T2>::const_iterator i=m.begin(); i!=m.end(); ++i){

        o<<'('<<i->first<<','<<i->second<<")";
    }
    return o;
}

// ostream << for priority_queue
template<typename T1, typename T2, typename T3>
ostream& operator<<(ostream& o, priority_queue<T1, T2, T3> q){
    o<<"Priority Queue with top: "<<q.top()<<", size: "<<q.size();
    return o;
}

// ostream << for pair
template<typename T1, typename T2>
ostream& operator<<(ostream& o, pair<T1, T2> p){
    o<<'('<<p.first<<','<<p.second<<')';
    return o;
}

// less for pid
class PairLess{
public:
    bool operator()(const pair<int, double>& lhs, const pair<int, double>& rhs){
        
        // put large sim in the end
        return lhs.second > rhs.second;
    }
};

// read rating matrix
extern shared_ptr<mat> read_rating(string, int, int, int, int);

// 2-order normalization for rows of matrix
extern void normalize2_row(mat&);
extern void normalize2_column(mat&);

// whether row i is a zero_row
extern bool is_zero_row(mat::iterator1);

// readin rating matrix
// index: 
//      0   user-index
//      1   item-index
extern shared_ptr<mat> read_rating(int);

// substract row avg for non-zero elements
extern void remove_avg_row(mat&);

#endif
