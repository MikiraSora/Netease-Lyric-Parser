#include <regex>
#include <string>
#include <map>
#include <fstream>
#include <list>
#include <iostream>
#include "dirent.h"
#include <stdexcept>
#include "Lrc_Parser.h"
#include "/sdcard/ms_include/ms_debug.h"

using namespace std;
using namespace ms_std;

namespace ms_std {
#define dump(x) dpfunclv_t(1,DEBUG_TYPE_NOTICE,x);
#define p() scanf();
#define check_p(x) /*printf("\n%s",x);*/ fflush(stdout);
#define safe_string(x,d) ((x.size()!=0)?x:d)

bool Lrc_Parser::GetLrcFromNet(int id,int re=6){
  //printf("\nstart to get %d lrc from net",id);
       if(re<0){
  	    printf("\nGetLrcFromNet() : retry %d to get lrc",re);
  	    return 0;
  	  }
      char *buf,tmp_path[256],tmp_path2[256];
      buf=new char[801];
     sprintf(tmp_path,"%s_lrc_%d.lrc_parser_tmp",option->Tmp_Path.c_str(),id);
     
    
      sprintf(buf,R"(wget -q -O -  http://music.163.com/api/song/media?id=%d > %s)",id,tmp_path);
   
      system(buf);
     int file_size=0;
     fstream fs;
      
      FILE* f=fopen(tmp_path,"r");
      if(f==0){    	      
      return GetLrcFromNet(id,re-1);
      }
      fseek(f,0L,SEEK_END);
      file_size=ftell(f);
      if(!(bool)file_size){      	 
      	 return GetLrcFromNet(id,re-1);
      	}
      delete [] buf;	
      buf=new char[file_size+1];
      fclose(f);
      
      fs.open(tmp_path);
      
      fs.read(buf, file_size);
      fs.close();
      
      sprintf(tmp_path2,"rm -f %s",tmp_path);
     
       system(tmp_path2);
 
    string text = string(buf);
    if(text.size()==0){
        delete [] buf;
       throw logic_error("GetLrcFromNet() : download text is null.");
       }
      
    ParserLrc(text,true);
    if(raw_lrc.size()==0)
      printf("\nraw lrc text :\n%s",text.c_str());
      delete [] buf;
     return ((raw_lrc.size()>0)?true:false);
  }




    Lrc_Parser_Info *Lrc_Parser::GetInfoFormOnline(int t,string _url/*,Lrc_Parser_Info* info,*/,int re=6) {
  	  if(re<0){
  	    printf("\nretry %d to get tag",re);
  	    return 0;
  	  }
      char *buf,tmp_path[256],tmp_path2[256];
    buf=new char[801]; sprintf(tmp_path,"%s%d.lrc_parser_tmp",option->Tmp_Path.c_str(),t);
     if(t>0){
      sprintf(buf,R"(wget -q -O - http://music.163.com/m/song/%d/?userid=0 > %s)",t,tmp_path);
     }else{
         if(strcmp(_url.c_str(),"N/A")!=0)
         {
            //buf=_url.c_str();
            strcpy(buf,(_url.c_str()));
            
           }
           else{
                   string error_msg;
                   char _id[8];
                   sprintf(_id,"%d",t);
                   error_msg.append("Cant get url,id : ").append(_id).append(" , # : ").append(_url);
                    throw logic_error(error_msg.c_str());     
             }
       }
      system(buf);
     
     fstream fs;
     long _size=0;//=ftell(f);
      
      FILE* f=fopen(tmp_path,"r");
      if(f==0){    	      
      return GetInfoFormOnline(t,"",re-1);
      }
      fseek(f,0L,SEEK_END);
      _size=ftell(f);
      if(!(bool)_size){      	 
      	 return GetInfoFormOnline(t,"",re-1);
      	}
      	delete []buf;
      	buf=new char[_size];
      	
      fclose(f);
      
      fs.open(tmp_path);
      
      fs.read(buf, _size);
      fs.close();
      
      sprintf(tmp_path2,"rm -f %s",tmp_path);
     
       system(tmp_path2);
 Lrc_Parser_Info* info = new Lrc_Parser_Info();
    string text = string(buf);
    if(text.size()==0){
      delete[] buf;
       throw logic_error("GetTagFromNet() : download text is null.");
       }
    //int _ar=0,_ti=0;
    
    
    smatch sm;   
    
    regex reg(expr->expr_info_name);
    bool ismatch_ti(regex_search(text,sm,reg));    
    if(ismatch_ti)
      info->Title=sm[expr->info_name_id];
      
      reg=regex(expr->expr_info_artist);
bool ismatch_ar=regex_search(text,sm,reg);
     if(ismatch_ar)
      info->Artist=sm[expr->info_artist_id];
      
      
    if(ismatch_ti&&ismatch_ar){
      printf("\nuse normal expr. :\n");
      }else{    
    reg=regex(expr->expr_online_info);
    //bool ismatch = ;
    if(!(regex_search(text, sm, reg))){
      delete[] buf;
      delete info;
      printf("\n id : %d tag match failed ! raw text is :\n%s\n",t,text.c_str());
      return 0;
      }else{
        printf("\n use backup expr. ti : %s , ar : %s ",((ismatch_ti)?"true":"false"),((ismatch_ar)?"true":"false"));
          info->Title =string(sm[expr->online_title_id]); 
          info->Artist =string(sm[expr->online_artist_id]);
        }
    }
      delete [] buf;
     return info;
    }
    
    void Lrc_Parser::ParserLrc(string& buffer,bool isRaw){
    	 list<string>& m=(isRaw?raw_lrc:trans_lrc);
    	 regex reg(expr->expr_lrc);
    	 smatch sm;
    	 sregex_iterator _s=sregex_iterator(buffer.begin(),buffer.end(),reg),_e=sregex_iterator();
    	 int rec_i=0/*,t_id=expr->sub_lrc_time_id,c_id=expr->sub_lrc_context_id*/,l_id=expr->lrc_id;
    	 for(auto it=_s;it!=_e;it++){
    	 	sm=*it;
    	 	m.push_back(sm[l_id]);
    	 	//cout<<"lrc : "<<sm[l_id]<<endl;
    	 	rec_i++;
    	 }
    	}
    
    void Lrc_Parser::ParserData(string buffer){
      //p  printf("\nparseData input data size = %d",buffer.size());
    	  int n_id=expr->sub_data_name_id;
    	  int v_id=expr->sub_data_value_id;
    	  int rec_i=0;
    	  //dump(printf("\n ok)"));
    	  regex reg(expr->expr_data);
    	  smatch sm;
    	  sregex_iterator _s=sregex_iterator(buffer.begin(),buffer.end(),reg),_e=sregex_iterator();
    	  for(auto it=_s;it!=_e;it++){
    	  	sm=*it;
    	  	data[sm[n_id]]=sm[v_id];
    	  	}
    	  	//printf("\nparse Data finish.");
    	  	fflush(stdout);
    	}
    	
      void Lrc_Parser::ParserNomalTag(string& buffer,Lrc_Parser_Option* opt){
    	  Lrc_Parser_Info* info=/*new Lrc_Parser_Info()*/0;
    	  regex reg(expr->expr_tag);
    	  smatch sm;
    	  sregex_iterator _s=sregex_iterator(buffer.begin(),buffer.end(),reg),_e=sregex_iterator();
    	  
    	  int n_id=expr->tag_name_id,v_id=expr->tag_value_id;
    	  for(auto it=_s;it!=_e;it++)
    	  {
    	    sm=*it;
    	    data[sm[n_id]]=sm[v_id];
    	    }
    	    int _id=-1;
    	    string weburl="";
    	    
    	    bool hasTag=((bool)data.count("ti"))&&((bool)data.count("ar"));
    	    if(((!(hasTag))||opt->ForceGetTagFormNet)){
    	    if((bool)data.count("musicId")){
    	       _id=atoi((data["musicId"]).c_str());
    	       }else{ if((bool)data.count(R"(#)")){
    	         weburl=data[R"(#)"];
    	        }else{
    	          weburl="N/A";
    	          }
    	        
    	        }
    	       if(!opt->NotToGetTagFromNet) info=GetInfoFormOnline(_id,weburl);    	       
    	        if(info==0)
    	          throw logic_error("-Cant get info from GetTagFromNet()");
    	          
    	        //check_p("save info value");
    	        data["ti"]=info->Title;
    	        data["ar"]=info->Artist;
    	         }
    	         //check_p("get text is finish");
    	    }
    	    
    	    long Lrc_Parser::CoverLrcTime(string str){
    	      smatch sm;
    	      bool ismatch=regex_search(str,sm,regex(expr->expr_lrc_time));
    	      if((!ismatch))
    	        return -1;
    	      #define l_atol(x) (atol(const_cast<char*>(((sm[expr->x])).str().c_str())))
    	      return (l_atol(lrc_time_min)*60000+l_atol(lrc_time_sec)*1000+l_atol(lrc_time_msec));
    	    }
    	    
    	    
    	    
    	    void Lrc_Parser::CombineLrc(Lrc_Parser_Option* opt){
    	      list<string> _f,_a;
    	      if((raw_lrc.size())&&(!trans_lrc.size())){
    	        lrc=raw_lrc;
    	       // printf("\nonly have raw lrc\n");
    	        return ;
    	        }
  	        if((trans_lrc.size())&&(!raw_lrc.size())){
      	      lrc=list<string>(trans_lrc);
      	      //printf("\nonly have tans lrc %d\n",trans_lrc.size());
      	      return;
  	        }
  	        if(!(trans_lrc.size()||raw_lrc.size()))
  	          throw logic_error("Combine_Lrc() : No any lrc in trans_lrc or raw_lrc.");
  	         // printf("\nboth two version lrc have.\n");
  	        switch(opt->Lrc_Combine_Type){
  	            case Option_Lrc_Combine_Type::New_Line_And_Raw_Lrc_First:
  	            _f=raw_lrc;
  	            _a=trans_lrc;
  	            goto new_line;
  	            
  	            case Option_Lrc_Combine_Type::New_Line_And_Trans_Lrc_First:
  	            _f=trans_lrc;
  	            _a=raw_lrc;
  	            goto new_line;  	          
  	              
  	            case Option_Lrc_Combine_Type::Side_By_Side_And_Raw_Lrc_First:
  	            _f=raw_lrc;
  	            _a=trans_lrc;
  	            goto side_by_side;	       
  	            
  	                 case Option_Lrc_Combine_Type::Side_By_Side_And_Trans_Lrc_First:
  	                 _f=trans_lrc;
  	                 _a=raw_lrc;
  	                 goto side_by_side;
  	                 
  	                 default:
  	                 throw logic_error("Combine_Lrc() : unknown combine type");
  	          }
  	          
  	          new_line:
  	          for(auto i=_f.begin();i!=_f.end();i++)
  	            lrc.push_back((*i));
  	            for(auto i=_a.begin();i!=_a.end();i++)
  	            lrc.push_back((*i));
  	          return;
  	          
  	          side_by_side:
  	          //time -> [time]_f_lrc + _a_lrc
  	          map<long,string> r_lrc;
  	          smatch sm;
  	          regex reg(expr->expr_split_lrc);
  	          long _t=-1;
  	          string _i,_u,_o;
  	          int _c=0;
  	          for(auto i=_f.begin();i!=_f.end();i++){
  	              _i=*i;
  	              _t=CoverLrcTime(_i);
  	              r_lrc[_t]=_i;
  	             // printf("\n%d",_t);
  	              _c++;
  	            }
  	            //printf("\n has %d lrc save into map,now last list has %d lrc",r_lrc.size(),_a.size());
  	         for(auto i=_a.begin();i!=_a.end();i++){
  	              _i=*i;
  	              _t=CoverLrcTime(_i);
  	              if(r_lrc.count(_t)==0){
  	                  r_lrc[_t]=_i;
  	                  //printf("\ntime %d lrc not found!",_t);
  	                  continue;
  	                }
  	             if(!regex_search(_i,sm,reg)){
  	              // printf("\ntime %d - %s cant match",_t,_i.c_str());
  	                continue;  	                
  	                }
  	             _u=sm[expr->lrc_split_id];
  	             //printf("\n%d :%s + %s",_t,_i.c_str(),_u.c_str());
  	             _o=r_lrc[_t];
  	             _o.append(R"(/)").append(_u);
  	             r_lrc[_t]=_o;
  	           }
  	           for(auto _it=r_lrc.begin();_it!=r_lrc.end();_it++)
  	           lrc.push_back(_it->second);
  	          return;    	           
    	      }
    	      
    	      
    	      
    	  Lrc_Parser_Result* Lrc_Parser::Decode(string text,Lrc_Parser_Option* opt){
    	 opt=((opt!=0)?opt:option);
    	 ParserData(text);
    	
    	    if(0==data.size())
    	      throw logic_error("Decode() : No data in map");
        	    ParserNomalTag(text,opt);
      	   bool lrc_result=false;
    	   int _id=atoi(const_cast<char*>((data["musicId"]).c_str() ));
    	   
        if(!opt->NotToGetLrcFromNet){
            //printf("\nNotToGetLrcFromNet=false");
            if(opt->ForceGetLrcFromNet){
                //printf("\nFroceGetLrcFromNet=true.");
                GetLrcFromNet(_id);
                                                         }else{
                printf("\nFroceGetLrcFromNet=false"); if((data.count("lyric")>0)&&((opt->Lrc_Type==Option_Lrc_Type::Raw_Lrc)||(opt->Lrc_Type==Option_Lrc_Type::Both_Raw_And_Trans_Lrc))){
                 //printf("\nParseLrc(data.lyric,true");
                 ParserLrc(data["lyric"],true);   
            }
            }                                               
              
          }
        else{
          //printf("\nNotToGetLrcFromNet=true");
           if((data.count("lyric")>0)&&((opt->Lrc_Type==Option_Lrc_Type::Raw_Lrc)||(opt->Lrc_Type==Option_Lrc_Type::Both_Raw_And_Trans_Lrc))){
    	      ParserLrc(data["lyric"],true);
    	      if(raw_lrc.size()==0){
    	          // printf("\nNo lrc in list , Will call GetLrcFromNet");
    	           if(!opt->NotToGetLrcFromNet){ 
    	                 //printf("\nNotToGetLrcFromNet=false" );
  	                     GetLrcFromNet( _id);    	                  }
    	                
    	             }
    	        }
    	   
    	   }
ParserLrc(data["translateLyric"],false);

if(!(raw_lrc.size()||trans_lrc.size()))
    	      throw logic_error("Decode() : Cant got any lrc from file");
    	      
    	      
    	      //check_p("Lrc load : ok");
    	      CombineLrc(opt);
    	      //check_p("CombineLrc : OK");
    	      
    	      //printf("\nnow lrc list has %d lrc\n",lrc.size());
    	      
    	      if(lrc.size()==0)  	      
    	        throw logic_error("Decode() : output lrc_list hasnt any lrc");
    	        
    	        
    	      #define add_info(x) if(data.count(x)>0){string _s;_s.append("[").append(x).append(":").append(data[x]).append("]");lrc.push_front(_s);}
    	     
    	      #define add_nm_info(x)if(opt->NomalTag){add_info(x)}
    	      
    	      #define add_ex_info(x) if(opt->ExtraTag){add_info(x)}
    	      
    	      add_ex_info(R"(#)");
    	      add_ex_info("musicId");
    	      add_nm_info("by");
    	      add_nm_info("al");
    	      add_nm_info("co");
    	      add_nm_info("ar");
    	      add_nm_info("ti");
    	      add_nm_info("lr");
    	      #undef add_nm_info
    	       #undef add_ex_info
    	      #undef add_info
    	     string _lrc;
    	     for(auto it=lrc.begin();it!=lrc.end();it++){
    	       //dump(printf("\n%s",(*it).c_str()));
    	       _lrc.append(*it).append("\n");
    	       }    	        	    
    	       if(_lrc.size()==0)
    	          throw logic_error("Decode() : no lrc add in.");
    	   
    	    Lrc_Parser_Result* r=new Lrc_Parser_Result();
    	    #define add_info(y,x) if(data.count(x)){r->y=data[x];}
    	    //r->Title=data["ti"];
    	    add_info(Title,"ti")
    	    add_info(Artist,"ar")
    	    add_info(Album,"al")
    	    
    	    r->id=(atoi(const_cast<char*>(((data[R"(#)"])).c_str())));
    	    //add_info(
    	    r->Lyric=_lrc;
    	    #undef add_info
    	                                               
    	                                              r->is_Finish_Parse=((r->Lyric.size())&&(r->Title.size())&&(r->Artist.size()));
 
    	    return r;
    	    }
    	  
#undef dump
}