#ifndef LRC_PARSER_H
#define LRC_PARSER_H

#include <string>
#include <map>
#include<list>
#include <regex>
#include "/sdcard/ms_include/ms_debug.h"

using namespace std;
//using namespace ms_std;

namespace ms_std {
  
  struct Lrc_Parser_Result{
       //map<string,string> data;
       string Title,Album,Artist,Lyric;
       int id;
       bool is_Finish_Parse;
    };
  
  struct Lrc_Parser_Info{
  	  string Artist,Title,Album;
  	};

  enum Option_Lrc_Type {
    Raw_Lrc,
    Trans_Lrc,
    Both_Raw_And_Trans_Lrc
  };

  enum Option_Lrc_Combine_Type {
    New_Line_And_Raw_Lrc_First,
    New_Line_And_Trans_Lrc_First,
    Side_By_Side_And_Raw_Lrc_First,
    Side_By_Side_And_Trans_Lrc_First
  };


  struct Lrc_Parser_Option {
    string Tmp_Path=R"(/sdcard/Lrc_Parser_Tmp/)";
    bool ExtraTag = true;
    bool NomalTag = true;
    bool ForceGetTagFormNet = false;
    bool ForceGetLrcFromNet=false;
    bool NotToGetTagFromNet=false;
    bool NotToGetLrcFromNet=false;
    Option_Lrc_Type Lrc_Type = Option_Lrc_Type::Raw_Lrc;
    Option_Lrc_Combine_Type Lrc_Combine_Type =
      Option_Lrc_Combine_Type::New_Line_And_Raw_Lrc_First;
  };

  struct Lrc_Parser_Expr {
    string expr_split_lrc=R"((\[\d{2}\d*\:\d{2}(\.\d*)?\])(.*))";
    int lrc_split_id=3;
    string expr_lrc=R"((\[\d{2}\d*\:\d{2}(\.\d*)?\](\s*.*?))(?=\s*\\n))";
    int lrc_id = 1; 
    string expr_tag=R"(\[\s*([^\d]+?)\s*\:\s*(.+?)\s*\])";
    int tag_name_id=1;
    int tag_value_id=2;
    string expr_lrc_time=R"(\[(\d{2}\d*)\:(\d{2})(\.(\d*))?\])";
    int lrc_time_min=1;
    int lrc_time_sec=2;
    int lrc_time_msec=4;
    string expr_online_info=R"((<title>)((.+?)\s-\s(.+?))((?=（)|(?=\s*-?\s*网易云音乐)))"/*R"((<title>)((.+?)\s-\s(.+?))（)"*/;
    int online_title_id=3;
    int online_artist_id=4;
    string expr_data=R"(\"\s*([\w\d\"-]+)\"\s*\:\s*(((\"\")|(\"(.+?)\")|((-?[\d\w]+)))(?=(\})|(\,\"\s*([\w\d\"-]+))\"\s*\:)))";
    int sub_data_name_id = 1;
    int sub_data_value_id = 2; // or 3
    
    string expr_info_name=R"(data\-res\-name\=\"(.+)\")";
    int info_name_id=1;
    string expr_info_artist=R"(data-res-author=\"(.+)\")";
    int info_artist_id=1;
    bool LoadExprFormFile(string absolute_path);
  } /* *Default_Lrc_Parser_Expr*/;

  class Lrc_Parser {
  	
    string last_result;
    map < string, string > data;
    list < string > raw_lrc, trans_lrc,lrc;
    Lrc_Parser_Option *option;
    Lrc_Parser_Expr*  expr;
    
    Lrc_Parser_Info* GetInfoFormOnline(int,string,int);//ok
    
    void CombineLrc(Lrc_Parser_Option*);//ok

    void ParserData(string);//ok
    
    long CoverLrcTime(string);//ok
    
    void ParserLrc(string&,bool);//ok
    
    void ParserNomalTag(string&,Lrc_Parser_Option*);//ok
    
    bool GetLrcFromNet(int,int);
    
  public:
      Lrc_Parser():Lrc_Parser(0,0){}
      Lrc_Parser(Lrc_Parser_Option * _option,Lrc_Parser_Expr* _e):option((_option?_option:(new Lrc_Parser_Option()))),expr(((_e)?(_e):(new Lrc_Parser_Expr()))){
          dpfunclv_t(1,DEBUG_TYPE_NOTICE,printf("\nnow opt is loaded : %d",adr_p(option)));
          dpfunclv_t(1,DEBUG_TYPE_NOTICE,printf("\nnow expr is loaded : %d",adr_p(expr)));
        } 
    
    Lrc_Parser_Result* Decode(string,Lrc_Parser_Option * v);
    
    //bool Check(string);

    const string GetLastDecodeResult() {return last_result;}
    };
}         // end spacename ms_std
#endif