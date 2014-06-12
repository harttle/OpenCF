#include<iostream>
#include<fstream>
#include<string>
#include"linear.h"
#include <boost/foreach.hpp>

using namespace std;

double interpolation(curve line, int x){
    curve_ci after, before;

    for(after=line.begin(),before=line.end(); after!=line.end() && x > after->first; ++after) before=after;

    double res = 0;

    if(before == line.end()){
        if(after != line.end()) res = after->second; // <= first one
    }  
    else if(after == line.end())  res = before->second; // >  second one
    else{
        res = (after->second - before->second)
            *(x - before->first) / (after->first - before->first)
            + before->second;    //after != before
    }

    return res;
}

double delta_y(vector<pair<int, double> > line, int x1, int x2){
    return interpolation(line, x2) - interpolation(line, x1);
}

curve integral(const curve& line){
    curve integ;
    double v = 0;

    for(curve_ci i = line.begin(); i != line.end(); ++i){
        v += i->second;
        integ.push_back(make_pair(i->first, v));
    }
    return integ;
}

ostream& operator<<(ostream& o, const curve& line){
    o.setf(ios::scientific, ios::floatfield);
    o.precision(6);

    BOOST_FOREACH(pid i, line){
        o<<i.first<<'\t'<<i.second<<endl;
    }
    return o;
}
