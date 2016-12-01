/* 
 * File:   main.cpp
 * Author: farid Amiri
 *
 * Created on 6. listopad 2014, 13:37
 */




#include <cstdlib>
#include <iostream>     // std::cout
#include <algorithm>    // std::sort
#include <vector>       // std::vector
#include <fstream>      //file stream read/write
#include <stdint.h>
#include <iterator>
#include <string>
#include <sstream>


using namespace std;
typedef std::pair<uint64_t, uint64_t> datapair;
namespace std {

    inline ostream& operator<<(ostream& os, const datapair& p) {
        return os << p.first << ' ' << p.second;
    }

}

struct IntCmp {

    bool operator()(const datapair &leftOperand, const datapair &rightOperand) {
        if (leftOperand.first == rightOperand.first)
            return leftOperand.second < rightOperand.second;
        return leftOperand.first < rightOperand.first;
    }
} pairObj;


//split and sort data.txt to k chunks

int splitfile(uint64_t maxRecordInChunk) {
    

    unsigned long counter = 0;
    unsigned int chunk = 0;
    string tempFileName;
    std::vector<datapair> dataVector;



    std::fstream datafstm("data.txt");
    uint64_t fPair, sPair;



    bool negEofFlag = true;
    while (negEofFlag) {

        if (datafstm.eof())negEofFlag = false;


        if (counter < maxRecordInChunk && negEofFlag) {
            ++counter;

            datafstm >> fPair;
            datafstm >> sPair;
            dataVector.push_back(std::make_pair(fPair, sPair));

        } else {
            std::sort(dataVector.begin(), dataVector.end(), pairObj);
            ++chunk;

            tempFileName.clear();
            std::ostringstream oss;
            oss << "temp" << chunk << ".txt";
            tempFileName = oss.str();
            std::ofstream ofsm(tempFileName.c_str());

            counter = 0;

            copy(dataVector.begin(), dataVector.end(), ostream_iterator<datapair>(ofsm, "\n"));
            dataVector.clear();
            vector<datapair>().swap(dataVector);
            ofsm.close();
 
        }

    }

    datafstm.close();
   

    return chunk;


}

int main() {


    unsigned long counter = 0;
    clock_t startTime = clock();
    string tempFileName;
    unsigned int chunk = 0;
    uint64_t maxRecordInChunk = 15000000; //150000000 for big file and 15000000 for 10% file
    std::vector<datapair> dataVector;


    chunk = splitfile(maxRecordInChunk); //split data.txt to _chunk files 

    cout << "RUN Time for write sorted  " << chunk << " temp files is :" << double( clock() - startTime) / (double) CLOCKS_PER_SEC << " seconds." << endl;

    //create result file
    std::ofstream oresfsm("sortedfile.txt");
    //reading temp files
    std::ifstream itmpfsm[chunk];
    for (int i = 0; i < chunk; i++)
        {
        tempFileName.clear();                                                                                                           
        std::ostringstream oss;
        oss << "temp" << i + 1 << ".txt";							
        tempFileName = oss.str();                                                                                                    
      
        itmpfsm[i].open(tempFileName.c_str());
        }

    int remainUnreadFile = chunk;
    datapair * pairArray;
    pairArray = new datapair[chunk];

    int indexminPair = 0;

    datapair minPair;
    minPair.first = UINT64_MAX;
    minPair.second = UINT64_MAX;


    for (int i = 0; i < chunk; i++) {
        itmpfsm[i] >> pairArray[i].first;
        itmpfsm[i] >> pairArray[i].second;
       
        if (minPair.first > pairArray[i].first || ((minPair.first == pairArray[i].first) && (minPair.second > pairArray[i].second))) {
            minPair.first = pairArray[i].first;
            minPair.second = pairArray[i].second;
            indexminPair = i;
            
        }

    }
    
    bool pushed=true; 
   while (true) 
{
    

      if(pushed) 
                dataVector.push_back(std::make_pair(minPair.first, minPair.second));
     
   if(remainUnreadFile==0) counter= maxRecordInChunk;
     
   ++counter;
   if (counter> maxRecordInChunk) 
            {
      
                copy(dataVector.begin(), dataVector.end(), ostream_iterator<datapair>(oresfsm, "\n"));
                dataVector.clear();
                counter=0;
                
             }
  if(remainUnreadFile==0)break;
    
  if(itmpfsm[indexminPair].eof())
        {
        
	 remainUnreadFile--;
      
        pairArray[indexminPair].first=UINT64_MAX;
	pairArray[indexminPair].second=UINT64_MAX;
      //  indexminPair=-1;
        
        }

  else{
            
        itmpfsm[indexminPair] >> pairArray[indexminPair].first;
        itmpfsm[indexminPair] >> pairArray[indexminPair].second;
       
        }
  minPair.first=UINT64_MAX;
  minPair.second=  UINT64_MAX;
  pushed =false;
  for (int i = 0; i < chunk; i++) 
    {
   
    if(     
       (minPair.first>pairArray[i].first) ||
       (minPair.first==pairArray[i].first && minPair.second>pairArray[i].second )  )
      {
	minPair.first=pairArray[i].first;
	minPair.second=pairArray[i].second ;
        indexminPair=i;
        pushed=true;

      } 
            
    }
    	
 }





for (int i = 0; i < chunk; i++) {
    
   itmpfsm[i].close();
  }

oresfsm.close();
dataVector.clear();
vector<datapair>().swap(dataVector);
cout << "RUN Time for write total sorted  files counter=" <<  double(clock() - startTime) / (double) CLOCKS_PER_SEC << " seconds." << endl;

 return(0);
}
