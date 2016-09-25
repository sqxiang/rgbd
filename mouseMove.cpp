#include <opencv2/opencv.hpp>  
#include <stdio.h>  
#include <iostream>
#include <fstream>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <vector>
using namespace std;  
using namespace cv;  

#define MODE (S_IRWXU | S_IRWXG | S_IRWXO) 
 
cv::Mat org,dst,img,tmp;
double origin_x,origin_y,fin_width,fin_height;
bool isSave = false;  
void on_mouse(int event,int x,int y,int flags,void *ustc)//event鼠标事件代号，x,y鼠标坐标，flags拖拽和键盘操作的代号  
{  
    static Point pre_pt(-1,-1);//初始坐标  
    static Point cur_pt(-1,-1);//实时坐标  
    char temp[16];  
    if (event == CV_EVENT_LBUTTONDOWN)//左键按下，读取初始坐标，并在图像上该点处划圆  
    {  
        org.copyTo(img);//将原始图片复制到img中  
        sprintf(temp,"(%d,%d)",x,y);  
        pre_pt = Point(x,y);  
        putText(img,temp,pre_pt,FONT_HERSHEY_SIMPLEX,0.5,Scalar(0,0,0,255),1,8);//在窗口上显示坐标  
        circle(img,pre_pt,2,Scalar(255,0,0,0),CV_FILLED,CV_AA,0);//划圆  
        imshow("img",img);  
    }  
    else if (event == CV_EVENT_MOUSEMOVE && !(flags & CV_EVENT_FLAG_LBUTTON))//左键没有按下的情况下鼠标移动的处理函数  
    {  
        img.copyTo(tmp);//将img复制到临时图像tmp上，用于显示实时坐标  
        sprintf(temp,"(%d,%d)",x,y);  
        cur_pt = Point(x,y);  
        putText(tmp,temp,cur_pt,FONT_HERSHEY_SIMPLEX,0.5,Scalar(0,0,0,255));//只是实时显示鼠标移动的坐标  
        imshow("img",tmp);  
    }  
    else if (event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON))//左键按下时，鼠标移动，则在图像上划矩形  
    {  
        img.copyTo(tmp);  
        sprintf(temp,"(%d,%d)",x,y);  
        cur_pt = Point(MIN(tmp.cols,x),MIN(tmp.rows,y));  
        putText(tmp,temp,cur_pt,FONT_HERSHEY_SIMPLEX,0.5,Scalar(0,0,0,255));  
        rectangle(tmp,pre_pt,cur_pt,Scalar(0,255,0,0),1,8,0);//在临时图像上实时显示鼠标拖动时形成的矩形  
        imshow("img",tmp);  
    }  
    else if (event == CV_EVENT_LBUTTONUP)//左键松开，将在图像上划矩形  
    {  
        org.copyTo(img);  
        sprintf(temp,"(%d,%d)",x,y);  
        cur_pt = Point(MIN(img.cols,x),MIN(img.rows,y));  
        putText(img,temp,cur_pt,FONT_HERSHEY_SIMPLEX,0.5,Scalar(0,0,0,255));  
        circle(img,pre_pt,2,Scalar(255,0,0,0),CV_FILLED,CV_AA,0);  
        rectangle(img,pre_pt,cur_pt,Scalar(0,255,0,0),1,8,0);//根据初始点和结束点，将矩形画到img上  
        //imshow("img",img);  
        img.copyTo(tmp);  
        //截取矩形包围的图像，并保存到dst中  
        int width = abs(pre_pt.x - cur_pt.x);  
        int height = abs(pre_pt.y - cur_pt.y);  
        if (width == 0 || height == 0)  
        {  
            printf("width == 0 || height == 0");  
            return;  
        }  
        dst = org(Rect(min(cur_pt.x,pre_pt.x),min(cur_pt.y,pre_pt.y),width,height));  
        origin_x = min(cur_pt.x,pre_pt.x);
        origin_y = min(cur_pt.y,pre_pt.y);
        fin_width = width;
        fin_height = height;
        isSave = true;
        namedWindow("dst");  
        imshow("dst",dst);  
        waitKey(0);  
    }  
}

int mk_dir(const char *dir)  
{  
    DIR *mydir = NULL;  
    if((mydir= opendir(dir))==NULL)//判断目录   
    {  
      int ret = mkdir(dir, MODE);//创建目录  
      if (ret != 0)  
      {  
          return -1;  
      }  
      printf("%s created sucess!\n", dir);  
    }  
    else  
    {  
        printf("%s exist!\n", dir);  
    }  
  
    return 0;  
}  

int main(int argc, char *argv[])  
{  
   char c;
    string dirname;
    dirname = argv[1];
    vector<string> types;
    vector<string> t_name;
    vector<string> new_tname;
    string depth = "depth_img1/";
    string rgb = "rgb_img1/";
    types.push_back(depth);
    types.push_back(rgb);
    for(int i=0;i<2;i++){
       t_name.push_back(types[i]+dirname);
       new_tname.push_back(types[i]+"new_"+dirname);
     }

    for(int i=0;i<2;i++){
    const char *s = new_tname[i].c_str();
     mk_dir(s);
    }
    DIR *dp;
    struct dirent *dirp;
    int index = 0;
    //int pos = dirname.find("/",0);
    //string f_dir = dirname.substr(0,pos);
    //string s_dir = dirname.substr(pos+1);
    //char new_dirc[128];
    //sprintf(new_dirc,"%s/new_%s",f_dir.c_str(),s_dir.c_str());
    //string new_dir(new_dirc);
    //cout<<f_dir<<" "<<s_dir<<endl;
    //cout<<"the dirctory is "<<argv[1]<<endl;
    if((dp=opendir(t_name[0].c_str()))==NULL)
       cout<<"Can't open "<<t_name[0]<<endl;
    while((dirp=readdir(dp))!=NULL){
      cout<<dirp->d_name<<endl;
      if(strcmp(".",dirp->d_name)==0 || strcmp("..",dirp->d_name)==0)
         {continue;}
      vector<string> img_name;
      vector<Mat> img_org;
      for(int i=0;i<2;i++){
        string imgname = t_name[i]+"/"+dirp->d_name;
        img_name.push_back(imgname);
        Mat src = imread(imgname,-1);
        img_org.push_back(src);
      }
      org = img_org[0];  
      org.copyTo(img);  
      org.copyTo(tmp);  
      namedWindow("img",WINDOW_NORMAL);//定义一个img窗口  
      setMouseCallback("img",on_mouse,0);//调用回调函数  
      imshow("img",img);
      char saved_img_name[128];
      char saved_txt_name[128];
      vector<string> saved_img;
      vector<string> saved_txt;
      while(1){
        c = (char)waitKey(0);
        if (27==c)
         break;
        if(c=='s' && isSave){
          org.copyTo(img);
          //string dname(dirp->d_name);
          //int pos = dname.find(".",0);
          //string f_dir = dname.substr(0,pos);
          sprintf(saved_img_name,"%s_%d.jpg",dirname.c_str(),++index);
          sprintf(saved_txt_name,"%s_%d.txt",dirname.c_str(),index);
          for(int i=0;i<2;i++){
            string tmp_img = new_tname[i]+"/"+saved_img_name;
            string tmp_txt = new_tname[i]+"/"+saved_txt_name;
            saved_img.push_back(tmp_img);
            saved_txt.push_back(tmp_txt);
           }
          for(int i=0;i<2;i++){
             ofstream outfile(saved_txt[i].c_str(),ios::out);
             outfile<<"("<<origin_x<<", "<<origin_y<<"), "<<fin_width<<", "<<fin_height<<endl;
             outfile.close();
         }
          Rect rect(origin_x,origin_y,fin_width,fin_height);
          for(int i=0;i<2;i++){
             img_org[i] = img_org[i](rect);
             imwrite(saved_img[i],img_org[i]);
           }
          //imwrite(saved_img,dst);
          cvDestroyWindow("dst");
          isSave = false;
          break;
          }
        if(c=='q' && isSave){
          org.copyTo(img);
          cvDestroyWindow("dst");
          isSave = false;
        }
  }
   }
   return 0;
} 
