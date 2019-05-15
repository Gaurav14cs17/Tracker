#include"iostream"
#include "map"
#include "set"
#include "vector"
#include "math.h"

using namespace std;

struct Box{
    int x ;
    int y ;
    int x1 ;
    int y1;
};


class My_Tracker{

private:
	int next_ide = 0;
	map<int , pair<int , int > > objects;
	map<int , int >disappeared;
	int mxDisappeared = 10;
	int mxDistance = 80;

public :
    My_Tracker(int maxDisappeared  ,  int maxDistance );
    void registered(pair<int , int > centroid );
    void deregistered( int objectID);
    int  D( int a , int b , int c , int  d);
    map<int ,pair<int , int >> update(std::vector<Box>v);
};

My_Tracker::My_Tracker(int maxDisappeared ,  int maxDistance  ){
//	My_Tracker::mxDisappeared = mxDisappeared;
//    My_Tracker::mxDistance = mxDistance ;
}

 void My_Tracker::registered(pair<int , int > centroid ){
 	objects[My_Tracker::next_ide] = centroid;
 	disappeared[My_Tracker::next_ide] = 0;
 	++ My_Tracker::next_ide;
 }

void My_Tracker::deregistered( int objectID){
	objects.erase(objectID);
	disappeared.erase(objectID);
}


 int  My_Tracker:: D( int a , int b , int c , int  d){
 	return sqrt((a-c)*(a-c) + (b-d)*(b-d));
 }

 map<int , pair<int , int >> My_Tracker:: update(std::vector<Box>v){

 	if(v.size()==0){
 		for(auto it : disappeared){
 			disappeared[it.first] += 1;
 			if(disappeared[it.first] >mxDisappeared)
 				 deregistered(it.first);
 		}
 		return objects;
 	}

 	vector<pair<int , int >>inputCentroid ;
 	set<int >unused_current_centroid;
 	set<int >unused_centroid;

 	for(int i = 0 ; i<v.size() ; ++i ){
        Box tmp = v[i];
        int cx = (tmp.x + tmp.x1 )/2;
        int cy = (tmp.y + tmp.y1) /2;
        inputCentroid.push_back({cx , cy });
        unused_current_centroid.insert(i);
 	}

 	if(objects.size()==0){
        for(auto it : inputCentroid)
            registered(it);
 	}
 	else{
 	   vector<pair<int , int >>idx;
 	   map<int , int >dist;

 	   for(auto it : objects ){
		pair<int , int >ans = {-1 , INT_MAX};
		for( int j = 0 ; j<inputCentroid.size() ; ++j ){
		    int  d = D(it.second.first , it.second.second , inputCentroid[j].first , inputCentroid[j].second );
		    if( d <= ans.second ){
			ans = { j , d };
		    }
		}
		idx.push_back({it.first , ans.first });
		dist[it.first] = ans.second ;
		unused_centroid.insert(it.first);
 	   }

 	   set<int>used_idx;
 	   set<int>used_current_centroid;

           for(auto it : idx ){
		if(used_idx.find(it.second)!= used_idx.end()|| used_current_centroid.find(it.first)!= used_current_centroid.end())
		    continue;
		if(dist[it.first] > mxDistance ){
		    continue;
		}
		objects[it.first] = inputCentroid[it.second ];
		disappeared[it.first] = 0;
		used_idx.insert(it.second);
		used_current_centroid.insert(it.first);
		auto x = unused_current_centroid.find(it.second);
		if( x  != unused_current_centroid.end())unused_current_centroid.erase(x);
		 x = unused_centroid.find(it.first);
		if(x != unused_centroid.end()) unused_centroid.erase(x);
	    }

	    if(objects.size() >= inputCentroid.size()){
		for(auto it : unused_centroid ){
		    disappeared[it]+= 1;
		    if( disappeared[it] > mxDisappeared  )
			deregistered(it);
		}
	     }else{
		 for(auto it : unused_current_centroid)registered(inputCentroid[it]);
	     }
	}
	return objects;
 }





int main(){


}
