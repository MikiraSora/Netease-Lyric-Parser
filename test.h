#include "Lrc_Parser.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include<cstdio>
#include <stdexcept>
#include <thread>
#include <map>
#include <chrono>
#include <list>
#include "ms_dir.h"
//#include <thread>
//#include <chrono>
#include "unistd.h"
#include "/sdcard/ms_include/ms_debug.h"
#define DEBUG_MODE
using namespace std;
using namespace ms_std;

 static volatile int max_count=0;
 static volatile  int now_count=0;
 static volatile  int failed_count=0;
 static volatile int thread_count=0;
static list<string> Music_List;
void func(int t_id,string save,Lrc_Parser_Option *o,string file_path){
  thread_count--;
  //string save_path=R"(/sdcard/LPLyric/)";
    Lrc_Parser lp;
   fstream fs;
    //printf("start load");
    //printf("\nload file : %s",file_path.c_str());
    fs.open(file_path);
    fs.seekg(0,fs.end);
    int len=fs.tellg();
    fs.seekg(0,fs.beg);
    //printf("\nlen : %d\n",len);
    char* bufc=new char[len];
    fs.read(bufc,len);
    fs.close();
    //printf("read finish");
    string buffer(bufc);
    if(!buffer.size()){
        throw logic_error("File no data");
    }
   
    Lrc_Parser_Result* res=0;
    Lrc_Parser_Option* opt=o;
    if(!opt)
      throw logic_error("option is null");
   
    try{
      printf("\n%d : 正在批量获取(gd)歌词信息中...( %d / %d)(failed  : %d)\n",t_id,now_count,max_count,failed_count); 
      
     res = lp.Decode(buffer,o);   
     
     if(!res)
       throw logic_error("cant get result");
      if(res->is_Finish_Parse){
      cout<<"标题 : "<<res->Title<<endl<<"艺术家 : "<<res->Artist<<endl;
      //
      string f_p;
      string file_name;
      file_name.append(res->Artist).append(R"( - )").append(res->Title); f_p.append(save);//append(file_name).append(R"(.lrc)");
      if(Music_List.size()!=0){
      for(auto &i:Music_List){
        if(strcmp(file_name.c_str(),i.c_str()))
          continue;
        f_p.append("Success/");
        Music_List.remove(i);
        }
        f_p.append("Failed/");
        }
    f_p.append(file_name).append(R"(.lrc)");
   printf("save to : %s\n",f_p.c_str());
   FILE* f=fopen(f_p.c_str(),"w+");
   if(!f)
     throw logic_error(string("can open file : ").append(f_p).c_str());
   int i_p=0;
   do{
   i_p=fwrite(res->Lyric.c_str(),1,res->Lyric.size(),f);
   //printf("\nwrite : %d",i_p);
   }while(!i_p); 
   fclose(f);
   
        /*fs.open(f_p);
        if(!fs.is_open())
          
          
        fs.write(res->Lyric.c_str(),res->Lyric.size());
        fs.close();
      //
      }
      */
      }else{
        printf("res is false\n");
        }
        
     }catch(logic_error e){
       cout<<"\n catch error!! cause : "<<e.what()<<endl;
       failed_count++;
       }
       now_count++;
    delete [] bufc;
    delete res;
    //sleep(0);
    //system("clear");
    thread_count++;
  }
                  
int main(){
 /* DEBUG_ENABLE_DUMPTYPE(DEBUG_TYPE_ALL);
  DEBUG_SETDUMPLEVEL(2);*/
  printf("请输入设置,格式如下:\n<歌词文件夹路径> <存储临时文件路径> [可选设置1] [可选设置2] ... [可选设置n] \n可选设置:\n-ExtraTag/-e : 添加额外标签\n-Normal/-n : 添加通用标签\n-ParseInv_<数字> : 文件读取间隔(毫秒)，短了容易boom,默认+1s\n-Check_<路径> : 检查所得歌词是否和文件夹内存在符合歌曲，没有就丢弃删除歌词文件，不写此语句则不检查\n-MThread_<数字> : 使用<数字>条线程同时进行(过多会炸，不选默认单线程)\n-ForceGetTagFromNet/-ft:\n强行直接从网络获取歌曲信息(失败才尝试在文件内获取,不选这个选项反之执行)\n-NotToGetTagFromNet/-nt : 无论如何都不会使用网络更新歌曲信息\n-ForceGetLrcFromNet/-fl : 强行从网上获取歌词文本(纯音乐或者非正式格式歌词文本将会跳过)\n-Lrc_Raw/-lr: 获取原歌曲歌词\nNotToGetLrcFromNet/-nl : 无论如何都不会使用网络下载歌词\n-Lrc_Trans/-lt : 获取翻译后歌词(如果有)\n-Lrc_Combine<Type>/-lc<Type>设置歌词合并方向 : \n  NR : 另起一行且原歌词在先显示\n  NT :另起一行且翻译歌词在先显示\n  SR或ST 并列一行，先后如前者所示.\n设置例子 :\n\n\n/sdcard/netease/Lyric/ /sdcard/LRC_Tmp/ -NormalTag -ExtraTag -Lrc_Raw -Lrc_Trans -Lrc_Combine_NR\n\n\nplease input your option codes : \n\n> ");
  char save_path_b[256];
  char tmp_path_b[256];
  char op_b[256];
  scanf("%s",save_path_b);
  scanf("%s",tmp_path_b);
  gets(op_b); 
  fflush(stdin);

  Dictionary dir("/sdcard/netease/cloudmusic/Download/Lyric/");
  
  string save_path=string(save_path_b);
  auto collection=dir.Get_Files();
  //map<string>
  max_count=collection.size();
  
  Lrc_Parser_Option* opt=new Lrc_Parser_Option();
  string opt_s(op_b);
  opt->Tmp_Path=tmp_path_b;
  #define fv(name,s_name,r_name,tvalue,fvalue) if((opt_s.find(name)!=string::npos)||(opt_s.find(s_name)!=string::npos)){ opt->r_name=tvalue;}else{opt->r_name=fvalue;}
  fv("-ForceGetLrcFromNet","-fl",ForceGetLrcFromNet,true,false);
  fv("-NotToGetLrcFromNet","-nl ",NotToGetLrcFromNet,true,false);
fv("-NotToGetTagFromNet","-nt ",NotToGetTagFromNet,true,false);
  fv("-ExtraTag","-e ",ExtraTag,true,false);
  fv("-NormalTag","-n ",NomalTag,true,false);
  fv("-ForceGetTagFromNet","-ft ",ForceGetTagFormNet,true,false);
  if((opt_s.find("-Lrc_Raw")!=string::npos)&&(opt_s.find("-Lrc_Trans"))){
       opt->Lrc_Type=Option_Lrc_Type::Both_Raw_And_Trans_Lrc;
    }else{
      fv("-Lrc_Raw","-lr ",Lrc_Type,Option_Lrc_Type::Raw_Lrc,Option_Lrc_Type::Trans_Lrc);
            fv("-Lrc_Trans","-lt ",Lrc_Type,Option_Lrc_Type::Trans_Lrc,Option_Lrc_Type::Raw_Lrc);
      
      }
         smatch _sm;
         regex _reg(R"(-MThread_(\d+))");
         bool ismatch=regex_search(opt_s,_sm,_reg);
         if(ismatch)
         {
           thread_count=(atoi(const_cast<char*>(_sm[1].str().c_str())));
           }else{
             thread_count=1;
             }           
            
         int _inv=1;
         _reg=regex(R"(-ParseInv_(\d+))"); 
           ismatch=regex_search(opt_s,_sm,_reg);
         if(ismatch)
         {
           _inv=(atoi(const_cast<char*>(_sm[1].str().c_str())));
           }else{
            _inv=1000;
             }
             
             
          string Check_Path;
          _reg=regex(R"(-Check_(/(.+/)+?))");
     ismatch=regex_search(opt_s,_sm,_reg);
         if(ismatch)
         {           
         Check_Path=_sm[1].str();
           }else{
             Check_Path="N/A";
             }
             
         
             fv("-Lrc_Combine_NR","-lcnr ",Lrc_Combine_Type,Option_Lrc_Combine_Type::New_Line_And_Raw_Lrc_First,Option_Lrc_Combine_Type::New_Line_And_Trans_Lrc_First);
             fv("-Lrc_Combine_NT","-lcnt ",Lrc_Combine_Type,Option_Lrc_Combine_Type::New_Line_And_Trans_Lrc_First,Option_Lrc_Combine_Type::New_Line_And_Raw_Lrc_First);
             fv("-Lrc_Combine_SR","-lcsr ",Lrc_Combine_Type,Option_Lrc_Combine_Type::Side_By_Side_And_Raw_Lrc_First,Option_Lrc_Combine_Type::Side_By_Side_And_Trans_Lrc_First);
             fv("-Lrc_Combine_ST","-lcst ",Lrc_Combine_Type,Option_Lrc_Combine_Type::Side_By_Side_And_Trans_Lrc_First,Option_Lrc_Combine_Type::Side_By_Side_And_Raw_Lrc_First);
      //
  //
  //#eopt->Tmp_Path=string(tmp_path_b);
  #undef fv
  #define ifb(x) ((opt->x)?"true":"false")
  
  printf("\nSo,your option is :\nExtraTag : %s\nNormalTag : %s\nForceUpdateTagFromNetwork : %s\nNotToGetTagFromNetwork : %s\nForceUpdateLrcFromNet : %s\nNotToGetLrcFromNet : %s\nSave Path : %s\nParseInv : %dms\nMulti-Thread : %d \nTmp Path : %s\nCheck Path : %s\n",ifb(ExtraTag),ifb(NomalTag),ifb(ForceGetTagFormNet),ifb(NotToGetTagFromNet),ifb(ForceGetLrcFromNet),ifb(NotToGetLrcFromNet),save_path_b,_inv,thread_count,tmp_path_b,((Check_Path.size()!=0)?Check_Path.c_str():"N/A"));
  #undef ifb
  printf("\nok\n");
  fflush(stdout);
  
  if(strcmp(Check_Path.c_str(),"N/A")){
  Dictionary music_dir(Check_Path.c_str());
  
  _reg=regex(R"(/?((.+?/)*)(.+/?))");
  auto _e=sregex_iterator();
  sregex_iterator _s;
  for(auto &i:music_dir.Get_Files()){
 _s=sregex_iterator(i.begin(),i.end(),_reg);
 for(auto itor=_s;itor!=_e;itor++){
 Music_List.push_back((*itor)[3].str());
 }
  }
  }
  
  
 printf("\nplease input anything to start.\n");
  getc(stdin);
    string file_path;
    thread t;
    while((collection.size())){        
         if(thread_count<=0){
           continue;
           }
         file_path=collection.front();
         collection.pop_front();
         t=thread(func,thread_count,save_path,opt,file_path);
         t.detach();
         //sleep(_inv);
         this_thread::sleep_for(chrono::milliseconds(_inv));
      }   
}