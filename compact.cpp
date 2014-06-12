#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<map>
#include<vector>

#include<boost/program_options.hpp>
namespace po = boost::program_options;

using namespace std;

unsigned short iuser, iitem;
string ifname, ofname, umname, imname;

// Declare the supported options.
po::options_description desc("Allowed options");
po::variables_map vm;


int do_compact(){

    ifstream fin( ifname.c_str(), ios::in);
    ofstream fout( ofname.c_str(), ios::out);
    ofstream fumap( umname.c_str(), ios::out);
    ofstream fimap( imname.c_str(), ios::out);

    map<int, int> umap,imap;

    // mapped index
    int u=0, i=0;


    // do mapping
    while(!fin.eof()){

        string s_tmp; getline(fin, s_tmp); 
        if(fin.fail()) break;

        stringstream ss(s_tmp);

        for(int j=0; !ss.eof(); j++){
            
            if(iuser == j){
                int ntmp; ss>>ntmp; if(ss.fail()) break;

                if(umap.find(ntmp) == umap.end()) umap[ntmp] = u++;
                fout<<umap[ntmp];
            }
            else if(iitem == j){
                int ntmp; ss>>ntmp; if(ss.fail()) break;

                if(imap.find(ntmp) == imap.end()) imap[ntmp] = i++;
                fout<<imap[ntmp];
            }
            else {
                ss>>s_tmp; if(ss.fail()) break;
                fout<<s_tmp;
            }

            fout<<'\t';
        }
        fout<<endl;
    }

    // output map file
    for(map<int,int>::const_iterator k=umap.begin(); k!=umap.end(); ++k){
        fumap<<k->first<<' '<<k->second<<endl;
    }
    for(map<int,int>::const_iterator k=imap.begin(); k!=imap.end(); ++k){
        fimap<<k->first<<' '<<k->second<<endl;
    }

    fin.close(); fout.close(); fumap.close(); fimap.close();
    return 0;
}


int do_restore(){

    ifstream fin( ifname.c_str(), ios::in);
    ifstream fumap( umname.c_str(), ios::out);
    ifstream fimap( imname.c_str(), ios::out);
    ofstream fout( ofname.c_str(), ios::out);


    map<int, int> umap,imap;

    // read map file
    while(!fumap.eof()){
        int norigin,nmapped; fumap>>norigin>>nmapped; if(fumap.fail())   break;
        umap[nmapped] = norigin;
    }
    while(!fimap.eof()){
        int norigin,nmapped; fimap>>norigin>>nmapped; if(fimap.fail())   break;
        imap[nmapped] = norigin;
    }
   

    // do restore
    while(!fin.eof()){

        string s_tmp; getline(fin, s_tmp); 
        if(fin.fail()) break;

        stringstream ss(s_tmp);

        for(int j=0; !ss.eof(); j++){
            
            if(iuser == j){
                int ntmp; ss>>ntmp; if(ss.fail()) break;

                fout<<umap[ntmp];
            }
            else if(iitem == j){
                int ntmp; ss>>ntmp; if(ss.fail()) break;

                fout<<imap[ntmp];
            }
            else {
                ss>>s_tmp; if(ss.fail()) break;
                
                fout<<s_tmp;
            }

            fout<<'\t';
        }
        fout<<endl;
    }

    fin.close(); fout.close(); fumap.close(); fimap.close();
    return 0;
}


void help(po::options_description desc){
    cout << "Usage: compact [options]"<<endl;
    cout << "\tCompact user-column and item-column indices. The maps applied to input-file will be stored in output-file.umap and output-file.imap. '-r' is used when restoring indices, map files will be searched in the same directory as input-file."<<endl; 
    cout << desc <<endl;
}

// compact uid, bid:w
int main(int argc, char* argv[]){

    desc.add_options()
        ("user,u", po::value<unsigned short>(&iuser)->default_value(0), "set user column")
        ("item,i", po::value<unsigned short>(&iitem)->default_value(1), "set item column")
        ("restore,r", "restore compact file with map files in the same directory")
        ("input-file,f", po::value<string>(&ifname)->required(), "input file (required)")
        ("output-file,o", po::value<string>(&ofname)->required(), "output file (required)")
        ("usermap-file,U", po::value<string>(&umname)->required(), "user map file (required)")
        ("itemmap-file,I", po::value<string>(&imname)->required(), "item map file (required)")
        ("help,h", "print this help message")
    ;

    try{
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);    
    }
    catch(exception e){ help(desc); return 1; }

    if (vm.count("help")) { help(desc); return 1; }
    
    if(vm.count("restore")){ do_restore();  }
    else{ do_compact(); }

    return 0;
}

