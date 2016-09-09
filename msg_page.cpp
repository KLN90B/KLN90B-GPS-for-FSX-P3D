#include "PCH.h"
#include "fslib.h"
#include "kln90b.h"
#include <vector>
#include <string>

using namespace std;
static symbol __video_buffer[7][23];
static int curr_screen_format;
static int mess_in_buf;
vector<string> m_buff;
extern INT SCREEN_FORMAT;

void print_msg_page(void)
{

	int mes_num=0;
	//for(vector<string>::iterator it=m_buff.begin();it != m_buff.end() && mes_num<6;++it)
	//{
	//   string str = *it;
	//   print_str(mes_num,0,ATTRIBUTE_NORMAL,"%-23s",str.c_str() ? str.c_str() : " ");
    //   mes_num++;
	//}
	if(!m_buff.empty())
	{
		//print_str(0,0,ATTRIBUTE_NORMAL,"%-23s",((string)*(--m_buff.end())).c_str() ? ((string)*(m_buff.end())).c_str() : " ");
		//mes_num=1;
		//m_buff.erase(--m_buff.end());
		print_str(0,0,ATTRIBUTE_NORMAL,"%-23s",((string)*m_buff.begin()).c_str());
		mes_num=1;
		m_buff.erase(m_buff.begin());
	}
	for(int i=mes_num;i<6;i++)
       print_str(i,0,ATTRIBUTE_NORMAL,"%-23s"," ");
	print_str(6,0,ATTRIBUTE_NORMAL,"%-6s"," ");
	print_str(6,17,ATTRIBUTE_NORMAL,"%-6s"," ");
	update_screen();
}

int do_msg_page(int ACTION)
{
  if(ACTION == ACTION_INIT || ACTION == ACTION_FREE_RESOURCES)
  {
     mess_in_buf=0;
	 m_buff.clear();
  }
  if(ACTION == ACTION_SHOW)
  {
	  sr_screen_to_buf(SCREEN_SAVE,__video_buffer);
	  curr_screen_format = SCREEN_FORMAT;
	  SCREEN_FORMAT = FORMAT_ONEPART;
      print_msg_page();
	  do_status_line(DISABLE_FMES,NULL);
  }
  if(ACTION == ACTION_HIDE)
  {
	  sr_screen_to_buf(SCREEN_RESTORE,__video_buffer);
	  SCREEN_FORMAT = curr_screen_format;
	  do_status_line(DISABLE_FMES,NULL);
  }
  if(ACTION == INPUT_MSG)
  {
	 if(!m_buff.empty())
	 {
        
		print_msg_page();
		if(m_buff.empty()) do_status_line(DISABLE_FMES,NULL);
		return(NO_ACTION);
	 }
	 return(NO_MORE_MESSAGES);
  }
  //if(ACTION == ACTION_TIMER)
  //   print_msg_page();
  return(NO_ACTION);
}

void msg_add_message(char *message)
{
   m_buff.insert(m_buff.begin(),message);
   do_status_line(ENABLE_FMES,NULL); 
}