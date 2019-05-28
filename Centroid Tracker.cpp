#include"iostream"
#include "map"
#include "set"
#include "vector"
#include "math.h"

using namespace std;

struct Rect{
    int x ;
    int y ;
    int width ;
    int height;
};




class Trackableobject{
   public:
  	int obj_ide;
  	vector<pair<int , int >> centroid ;
  	int count ;
  public :
  	Trackableobject( int objectID , pair<int , int > centroid );
 };


Trackableobject::Trackableobject( int objectID , pair<int , int > cent_point ){
           obj_ide = objectID;
           centroid.push_back(cent_point);
           count = 0 ;
  }



class My_Tracker{

private:
	int next_ide = 0;
	map<int , pair<int , int > > objects;
	std::map<int , Trackableobject* > mp;
	map<int , int >disappeared;
        vector<pair<int , pair<int , int >>>idx;
 	map<int , int  >dist;
        vector<pair<int , int >>inputCentroid ;
 	set<int >unused_current_centroid;
 	set<int >unused_centroid;
        set<int>used_idx;
        set<int>used_current_centroid;


	int mxDisappeared  = 50;
	int mxDistance = 40;
        int W = 500;
        int H = 150;
        pair<int , int >check_line_cross;
        int total_up =0;
  	int total_down =0;
public :
    My_Tracker(int maxDisappeared  ,  int maxDistance  );
    void registered(pair<int , int > centroid );
    void deregistered( int objectID);
    int D( int a , int b , int c , int  d);
    void update(std::vector<cv::Rect>v);
    void draw(cv::Mat &img );
    void check();
};

My_Tracker::My_Tracker(int maxDisappeared ,  int maxDistance  ){
	//My_Tracker::mxDisappeared = mxDisappeared;
	//My_Tracker::mxDistance = mxDistance ;
}

 void My_Tracker::registered(pair<int , int > centroid ){
 	objects[next_ide] = centroid;
 	disappeared[next_ide] = 0;
 	++ next_ide;
 }

void My_Tracker::deregistered( int objectID){
        auto it = objects.find(objectID);
        if(it != objects.end()) objects.erase(it);
        auto it_1 = disappeared.find(objectID);
	if(it_1 != disappeared.end()) disappeared.erase(it_1);
	if(mp.find(objectID) != mp.end()) mp.erase(objectID);
}


 int   My_Tracker:: D( int a , int b , int c , int  d){
 	return sqrt((a-c)*(a-c) + (b-d)*(b-d));
 }

 void  My_Tracker:: update(std::vector<Rect>v){

 	if(v.size()==0){
 		for(auto it : disappeared){
 			disappeared[it.first] += 1;
 			if(disappeared[it.first] >mxDisappeared)
 				 deregistered(it.first);
 		}
 		return ;
 	}

 	inputCentroid.clear() ;
 	unused_current_centroid.clear();
 	unused_centroid.clear();
        idx.clear();
 	dist.clear();

 	for(int i = 0 ; i<v.size() ; ++i ){
        Rect tmp = v[i];
        int cx = (tmp.x +  (tmp.x +  tmp.width ))/2;
        int cy = (tmp.y +  (tmp.y + tmp.height ))/2;
        inputCentroid.push_back({cx , cy });
        unused_current_centroid.insert(i);
 	}

 	if(objects.size()==0){
              for(auto it : inputCentroid)
                   registered(it);
 	}else{

 	   
 	   for(auto it : objects ){
               pair<int , int >ans = {-1 , INT_MAX};
               for( int j = 0 ; j<inputCentroid.size() ; ++j ){
               int  d = D(it.second.first , it.second.second , inputCentroid[j].first , inputCentroid[j].second );
               if( d < ans.second ){
                ans = { j , d };
             }
          }
           idx.push_back({ans.second ,{it.first , ans.first }});
           
           if(dist[it.first] > ans.second && dist[it.first] ){
            dist[it.first] = ans.second ;
           
           
           }
           if(dist[it.first] == 0 ){
                 dist[it.first] = ans.second ;
           }
           unused_centroid.insert(it.first);
 	}
        
        sort(idx.begin() , idx.end());
 	used_idx.clear();
 	used_current_centroid.clear();

       for(auto it : idx ){

           if(used_idx.find(it.second.second)!= used_idx.end()|| used_current_centroid.find(it.second.first)!= used_current_centroid.end())
                    continue;

           if(dist[it.second.first] > mxDistance ){
                   continue;
           }

         objects[it.second.first] = inputCentroid[it.second.second ];
         disappeared[it.second.first] = 0;

        used_idx.insert(it.second.second);
        used_current_centroid.insert(it.second.first);

        auto x = unused_current_centroid.find(it.second.second);
        if( x  != unused_current_centroid.end())unused_current_centroid.erase(x);

         x = unused_centroid.find(it.second.first);
        if(x != unused_centroid.end()) unused_centroid.erase(x);
       }

       if(objects.size() >= inputCentroid.size()){
        for(auto it : unused_centroid ){
            disappeared[it] += 1;
            if( disappeared[it] > mxDisappeared )
                deregistered(it);
        }
       }else{
         for(auto it : unused_current_centroid)registered(inputCentroid[it]);
       }
 	}
    return ;
 }





  void My_Tracker :: draw(cv::Mat &img ){
	    for(auto it : objects ){
	   	string s = "ID " + to_string(it.first);
	   	cv::putText( img , s , cv::Point(it.second.first -10 , it.second.second-10) , FONT_HERSHEY_DUPLEX, 0.5 ,cv::Scalar(0, 255, 0) , 2);
	   	cv::circle(img , cv::Point(it.second.first , it.second.second ) , 4 ,cv::Scalar(0 , 255 , 0 ) , -1 );
	  }

	  string s1 = "OUT : " + to_string(total_up) ;
          string s2 = " IN : " + to_string(total_down);
	  cv::putText( img , s1  , cv::Point(50 , H-20) , FONT_HERSHEY_DUPLEX, 1 ,cv::Scalar(0, 0, 255) , 2);
          cv::putText( img , s2  , cv::Point(50 , H+30) , FONT_HERSHEY_DUPLEX, 1 ,cv::Scalar(0, 0, 255) , 2);
	  if(check_line_cross.first==1 || check_line_cross.second ==1){
	          cv::line(img , cv::Point(0 , H) , cv::Point( W , H ) , Scalar(0, 0 , 255) , 2 );
          }
	  else{
	  	  cv::line(img , cv::Point(0 , H ) , cv::Point(W , H ) , Scalar(0, 255, 255) , 2 );
              }

  }

int main(){


}
