#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>
#include <cmath>
#include <stdlib.h>
#include<sys/time.h>
#include <vector>
#include <algorithm>
#include <utility>
#include <map>
using namespace std;

//-----------------------DO NOT CHANGE NAMES, ONLY MODIFY VALUES--------------------------------------------

//Final Values that will be compared for correctness
//You may change the function prototypes and definitions, but you need to present final results in these arrays
//-----------------------------Structures for correctness check-------------------
int **SA_Final_student;
int **L_counts_student;
char *L_student;
int C[]={0,0,0,0,0};//C[c] = total # of text characters which are alphabetically smaller than c
vector<pair<string, int*> >pairs;
map<char, int> charIndex = {
    {'$',0},
    {'A',1},
    {'C',2},
    {'G',3},
    {'T',4}
};
//--------------------------------------------------------------------------------


//-----------------------DO NOT CHANGE--------------------------------------------

int read_count = 0;
int read_length = 0;

//Read file to get reads
inline void inputReads(vector<string> &reads, string &file_path){
    fstream read_file(file_path);
    int ch, lines=0;
    string line;
    while (read_file>>line)
    {
        line += '$';
        reads.push_back(line);
    }
    read_count = reads.size();
    read_length = reads[0].length();
}


//Generate Sufixes and their SA's for a read
inline void generateSuffixes(vector<vector<string>> &suffixes, vector<string> &reads){
    // vector<string> suffixes;
    for (int i=0; i<read_count; i++)
    {
        vector<string> suffixesOfOneString;
        for (int j=0; j<read_length; j++)
        {
            string s = reads[i];
            //Rotate read by 1 character using std::rotate()
            rotate(s.begin(), s.begin() + j, s.end());
            suffixesOfOneString.push_back(s);
        }
        suffixes.push_back(suffixesOfOneString);
    }
}


//Calculates the final FM-Index -- student version
inline void makeFMIndex_student(vector<vector<string>>& suffixes, char * (&L)){

    int i, j;

    SA_Final_student = new int*[read_count*read_length];
    for (int i=0; i<read_count*read_length; i++)
        SA_Final_student[i] = new int[2];

    //Temporary storage for collecting together all suffixes
    vector<string> temp_suffixes(read_count*read_length);

    //Initalization of temporary storage
    for(i=0;i<read_count;i++){
        for(j=0;j<read_length;j++){
            temp_suffixes[i*read_length+j] = suffixes[i][j];
            SA_Final_student[i*read_length+j][0]=j;
            SA_Final_student[i*read_length+j][1]=i;
        }
    }
    
    string temp;
    L_counts_student = new int*[read_length*read_count];
    for (int i=0; i<read_length*read_count; i++)
    {
        L_counts_student[i] = new int[5];
        for(j=0;j<5;j++){
            L_counts_student[i][j]=0;
        }
    }
    for (int i=0; i<read_count*read_length; i++)
        pairs.push_back(make_pair(temp_suffixes[i], SA_Final_student[i]));
    
    //Focus on improving this for evaluation purpose
    //Sorting of suffixes
    sort(pairs.begin(), pairs.end(), 
    [](pair<string, int*> &p1, pair<string, int*> &p2) -> bool{ 
        return p1.first<p2.first; 
    });
//----------------For debug purpose only - temp_suffixes-----------------
/*
    cout << endl;
    for (int i=0; i<temp_suffixes.size(); i++)
        cout << temp_suffixes[i] << endl;
    cout << endl;
*/
    char this_F = '$';
    j=0;
    
    //Calculation of L's and L_counts_student's
    for(i=0;i<read_count*read_length;i++){
        // char ch = temp_suffixes[i][read_length-1];
        char ch = pairs[i].first[read_length-1];
        L_student[i]=ch;
        if(i>0){
            for(int k=0;k<5;k++)
                L_counts_student[i][k]=L_counts_student[i-1][k];
        }
        L_counts_student[i][charIndex.find(ch)->second]++;
    }
}

//-----------------------DO NOT CHANGE--------------------------------------------

int main(int argc, char *argv[])
{
    string read_data_file= "P10000.txt";   // input DATA
    string queryString=argv[1];
    vector<string> reads;
    inputReads(reads, read_data_file);//Input reads from file

    // vector<vector<string>> suffixes;
    vector<vector<string>> suffixes_student;

    //-----------------------------Structures for correctness check----------------------------------------------
    L_student=(char*)malloc(read_count*read_length*sizeof(char*));//Final storage for last column of sorted suffixes
    //-----------------------------Structures for correctness check----------------------------------------------
    

    //-----------Default implementation----------------
    //-----------Time capture start--------------------
    struct timeval  TimeValue_Start;
    struct timeval  TimeValue_Final;
    struct timezone TimeZone_Start;
    struct timezone TimeZone_Final;
    long time_start, time_end;
    double time_overhead_default, time_overhead_student;

   


    // -----------Your implementations------------------


    
    // Generate read-wise suffixes
    generateSuffixes(suffixes_student, reads);
    
    //Calculate finl FM-Index
    makeFMIndex_student(suffixes_student, L_student);

    //construct C table
    C[1] = L_counts_student[read_count*read_length-1][0];//A=moneysign
    C[2] = C[1]+L_counts_student[read_count*read_length-1][1];//C=moneysign+A
    C[3] = C[2]+L_counts_student[read_count*read_length-1][2];//G=moneysign+A+C
    C[4] = C[3]+L_counts_student[read_count*read_length-1][3];//G=moneysign+A+C+G

    int i = 0;
    int j = read_count*read_length-1;

    gettimeofday(&TimeValue_Start, &TimeZone_Start); 

    char c = queryString[queryString.length()-1];
    i = C[charIndex.find(c)->second] + 0;
    j = C[charIndex.find(c)->second] + L_counts_student[j][charIndex.find(c)->second]-1;

    if (i < j)
    {
        for (int k=queryString.length()-2; k>=0; k--)
        {
            c = queryString[k];
            i = C[charIndex.find(c)->second] + L_counts_student[i-1][charIndex.find(c)->second];
            j = C[charIndex.find(c)->second] + L_counts_student[j][charIndex.find(c)->second]-1;
        
            if (i >= j)
                break;
        }
    }

    sort(pairs.begin()+i,pairs.begin()+j, [](const pair<string, int*> & a, const pair<string, int*> & b) -> bool
    { 
        return a.second[1] > b.second[1]; 
    }); //小到大sort好 

    
    int counter = 1;
    for (int k=i+1; k<=j; k++)
    {
        if(pairs[k].second[1]==pairs[k-1].second[1])//只要前後兩個比對 
        	continue;
        else
        	counter++;
    }

    //-----------Call your functions here--------------------
    gettimeofday(&TimeValue_Final, &TimeZone_Final);


    time_start = TimeValue_Start.tv_sec * 1000000 + TimeValue_Start.tv_usec;
    time_end = TimeValue_Final.tv_sec * 1000000 + TimeValue_Final.tv_usec;
    time_overhead_student = (time_end - time_start)/1000000.0;
    cout << "number of occurences: " << counter << endl;
    cout << "time_overhead_student" << time_overhead_student << "seconds" << endl;
    //--------------------------------------------------

    
    //dynamicallly free space
    for (int i=0; i<read_count*read_length; i++)
        delete []SA_Final_student[i];
    delete []SA_Final_student;
    reads.clear();
    pairs.clear();
    charIndex.clear();
    return 0;
}
