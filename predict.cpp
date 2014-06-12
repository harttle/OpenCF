#include<iostream>
#include<fstream>
#include<string>
#include<queue>
#include"datainfo.h"
#include"util.h"

#include<boost/program_options.hpp>
namespace po = boost::program_options;


using namespace std;

typedef pair<int, double> pid;
typedef priority_queue<pid, vector<pid>, PairLess> pq;
typedef map<int, pq> mpq;
typedef map<int, pq>::iterator mpq_i;

// Declare the supported options.
po::options_description desc("Allowed options");
po::variables_map vm;

int nuser, nitem, nrating,  // user count, item count, rating item count
    
    base,                   // user based / item based
    nneibor,                // nearest neighbor count k
    sim_norm,               // similarity normalization
    sim_summing;            // similarity summing

string ifname,              // input / output / similarity file name
       ofname,
       sfname;  


shared_ptr<mpq> read_sim(){
    cout<<"read similarity..."<<endl;

    shared_ptr<mpq> pm_sim(new mpq());
    mpq& msim = *pm_sim;

    ifstream fsim(sfname.c_str(), ios::in);
    while(!fsim.eof()){
        int u1, u2;   double sim;
        fsim>>u1>>u2>>sim;    if(fsim.fail()) break;

        msim[u1].push(make_pair(u2, sim));
        msim[u2].push(make_pair(u1, sim));

        if(msim[u1].size() > nneibor)    msim[u1].pop();
        if(msim[u2].size() > nneibor)    msim[u2].pop();
    }
    fsim.close();

    switch(sim_norm){
    case 0:
        break;
    case 1:
        // norm sim by divide Count
        for(mpq_i i=msim.begin(); i!=msim.end(); ++i){
            pq& q = i->second;  int index = i->first;

            pq q1; int count=q.size();
            for(;!q.empty(); q.pop())   q1.push(q.top());

            for(;!q1.empty(); q1.pop()){

                pid id = q1.top();
                id.second /= count;

                q.push(id);
            } 
        }
        break;
    case 2:
        // norm sim by divide Average, only works when all sim > 0
        for(mpq_i i=msim.begin(); i!=msim.end(); ++i){
            pq& q = i->second;  int index = i->first;

            pq q1; double sum=0;
            for(;!q.empty(); q.pop()){

                pid id = q.top();

                q1.push(id); sum += id.second;
            }   
            double avg = sum/q1.size();

            for(;!q1.empty(); q1.pop()){

                pid id = q1.top();
                id.second /= avg;

                q.push(id);
            } 
        }
        break;
    default:
        break;
    }

    if(vm.count("verbose")) cout<<"similarity queue map:\n"<<msim<<endl;

    return pm_sim;
}

shared_ptr<mat> filtering(const mat& mrating, mpq& msim){
    cout<<"collaborative filtering..."<<endl;

    shared_ptr<mat> pm_predict(new mat(mrating));
    mat& m_predict = *pm_predict;


    int max_row_index = base ? nitem : nuser;

    for(mpq_i i=msim.begin(); i!=msim.end(); ++i){

        cout<<(i->first+1) * 100 / max_row_index<<"%\r"; cout.flush();

        // get current row
        row ri(m_predict, i->first);
        pq& q = i->second;

        // pop all sim rows
        for(;!q.empty(); q.pop()){

            int index = q.top().first;
            double sim   = q.top().second;

            crow rj(mrating, index);

            switch(sim_summing){
            case 0:
                ri += rj * sim;
                break;
            case 1:
                for(crow::const_iterator j=rj.begin(); j!=rj.end(); ++j){

                    int col = j.index();

                    double v = 1 - ( 1 - ri(col) ) * ( 1 - *j * sim );
                    if(v)   ri(col) = v;
                }
                break;
            default:
                break;
            }
        }
    }
    cout<<endl;

    if(vm.count("verbose")) cout<<"prediction matrix:\n"<<m_predict<<endl;
    
    return pm_predict;
}

int main(int argc, char* argv[]){
    
    desc.add_options()
        ("base,b", po::value<int>(&base)->default_value(1), "Specify user/item based similarity. Available values:\n0 :\tuser based\n1 :\titem based")
        ("user-count,u", po::value<int>(&nuser)->default_value(USER_COUNT), "user count")
        ("item-count,i", po::value<int>(&nitem)->default_value(ITEM_COUNT), "item count")
        ("rating-count,r", po::value<int>(&nrating)->default_value(RATING_COUNT), "rating count (required)")
        ("neighbor-count,n", po::value<int>(&nneibor)->default_value(NEIGHBOR_COUNT), "neighbor count k")
        ("sim-summing", po::value<int>(&sim_summing)->default_value(0), "Similarity summing:\n0 :\tdirect summing\n1 :\tprobability summing")
        ("sim-norm", po::value<int>(&sim_norm)->default_value(0), "Normalization for similarity:\n0 :\tno norm\n1 :\tnorm by divide k\n2 :\tnorm by divide average, only works for positive similarities")
        ("rating-norm", "Normalization for rating rows")
        ("input-file,f", po::value<string>(&ifname)->required(), "input file (required)")
        ("output-file,o", po::value<string>(&ofname)->required(), "output file (required)")
        ("sim-file,s", po::value<string>(&sfname)->required(), "similarity file (required)")
        ("verbose,v", "Enable verbose output")
        ("help,h", "Display this message")
    ;

    try{
        po::store(po::parse_command_line(argc, argv, desc), vm); po::notify(vm);    
    }
    catch(exception e){ cout << "Usage: predict [options]"<<endl; cout << desc <<endl; return 1; }

    if (vm.count("help")) { cout << "Usage: predict [options]"<<endl; cout << desc <<endl; return 1; }

    shared_ptr<mat> pm_rating = read_rating(ifname, base, nuser, nitem, nrating);
    mat& m_rating = *pm_rating;

    if(vm.count("rating-norm")){
        if(base)    normalize2_column(m_rating);
        else    normalize2_row(m_rating);
    } 

    shared_ptr<mpq> pm_sim = read_sim();

    shared_ptr<mat> pm_predict = filtering(m_rating, *pm_sim);
    mat& m_predict = *pm_predict;

    cout<<"saving to file..."<<endl;
    ofstream fout(ofname.c_str(), ios::out);
    fout.setf(ios::scientific, ios::floatfield); fout.precision(6);

    for(mat::iterator1 i=m_predict.begin1(); i!=m_predict.end1(); ++i){
        for(mat::iterator2 j=i.begin(); j!=i.end(); ++j){

            long unsigned int index[] = { j.index1(), j.index2() };

            double pred = m_rating(index[0], index[1]) ? m_rating(index[0], index[1]) : *j;

            if(pred<=0)   continue;   // do not recommend
            else if(pred>1)  pred=1;  // probobility <= 1
            fout<<index[base]<<'\t'<<index[1-base]<<'\t'<<pred<<endl;
        }
    }
    fout.close();

}
