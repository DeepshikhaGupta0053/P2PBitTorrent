
#include <sys/stat.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <sys/socket.h>
#include <bits/stdc++.h>
#include <stdarg.h>
#include <netdb.h>
#include <sys/types.h>
#include <pthread.h>
#include <openssl/sha.h>
#include <errno.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <fstream>
#define SA struct sockaddr
using namespace std;

string t_ip, uploadFile;
uint16_t t_port;

unordered_map<string, set < string>> grp_members;
unordered_map<string, set < string>> grp_requests;
unordered_map<string, string> cur_file_size;
unordered_map<string, string> admin_groups;
vector<string> group_list;
unordered_map<string, string> login;
unordered_map<string, bool> if_if_logged_in;
FILE *log_ptr;
void append_Tracker_log(string msg)
{
    fprintf(log_ptr, "%s", msg.c_str());
}

unordered_map<string, string> convert_port;
unordered_map<string, string> piece_wise;

int v_s=3;
int valid_size=4;

//PPP
bool is_valid_path(const string &stringS)
{
    //struct stat is a system struct that is defined to store information about files. It is used in several system calls, including fstat, lstat, and stat.
	struct stat ab;
	if (stat(stringS.c_str(), &ab) == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}
//PPP ---

vector<string> break_string_using_delimiter(string text, string delimiter)
{
    vector<string> res;
    size_t st, end = -1*delimiter.size();
    do 
    {
        st = end + delimiter.size();
        end = text.find(delimiter, st);
        res.push_back(text.substr(st, end - st));

    } while (end != -1);
    return res;
}


vector<string> fetch_ip_port(string st, string delimitor) //-->OK
{
    unsigned long long found_index=0;
    string ip="",port="";
    vector<string> res;
    found_index=st.find(delimitor);
    
    if(found_index!=string::npos)
    {
        //cout<<" delimiter(:) found at position:  "<<found_index+1<<endl;
        ip=st.substr(0,found_index);
        port=st.substr(found_index+1,st.length());
        //cout << "IP  = " <<ip<< endl;
        //cout << "PORT  = " <<port<<endl;
        res.push_back(ip);
        res.push_back(port);

    }
    else
    {
        cout<<" delimiter(:) NOT found"<<endl;
         append_Tracker_log("\ndelimiter(:) NOT found");
    }

    return res;

}


vector<string> fetch_ip_port_tracker(char* filepointer) //-->OK
{
	string f_name = string(filepointer);
    
    string statement="";
    vector<string> res_tracker;
    
    ifstream tracker_file(f_name);
    
    if (tracker_file.is_open()) 
    {
		getline(tracker_file, statement);
		vector<string> trac_ip_port = fetch_ip_port(statement,":");
	
		string ip=trac_ip_port[0];
        res_tracker.push_back(ip);

		string port=(trac_ip_port[1]);
        res_tracker.push_back(port);
        
		return res_tracker;
        
    }

	else
    {
		
		cout<<"Cannot open the file"<<endl;
        append_Tracker_log("\nCannot open the file");
        res_tracker.push_back("0.0.0.0");
        res_tracker.push_back("0");
		return res_tracker;
	}

}

//PPP --
void *exit_serv(void *arg)
{
	while (1)
	{
        // id the input is "quit"
		string inp;
		getline(cin, inp);
		if (inp == "quit")
		{
            // then exutthe program
            append_Tracker_log("\nquit");
			exit(0);
		}
	}
}

unordered_map<string, unordered_map<string, set< string>>> uploadList;

//PPP ---
int check_login_validate(string identify_usr, string usr_pass)
{
    // if the user _ id is not found in login map, so invalidate the user
	if (login.find(identify_usr) == login.end())
		return -1;

    // if the password of user dont  match then also invalidate
	if (login[identify_usr] != usr_pass)
		return -1;

    int sw_fl=0;

	if (if_if_logged_in.find(identify_usr) == if_if_logged_in.end())
        sw_fl=1;
	else
        sw_fl=2;

    switch(sw_fl)
    {
        case 1:
                {
                    if_if_logged_in.insert({ identify_usr, true });
                    return 0;
                }
                break;
        case 2:
                {
                    if (if_if_logged_in[identify_usr] == false)
                    {
                    	if_if_logged_in[identify_usr] = true;
                    	return 0;
                    }
                    else
                    {
                    	return 1;
                    }

                }   
                break;     
    }

	return 0;
}

//PPP ---
void function_to_file_upload(string &file_path, string &group_id, int peer_soc_port, string &client_id)
{
    int fl_sw=0;
	if (!is_valid_path(file_path))
        fl_sw=1;
	else if (grp_members.find(group_id) == grp_members.end())
        fl_sw=2;
	else if (grp_members[group_id].find(client_id) == grp_members[group_id].end())
        fl_sw=3;
	else
        fl_sw=4;

    switch(fl_sw)
    {
        case 1:
                {
		            write(peer_soc_port, "No file exists", 14);
                     append_Tracker_log("\nNo file exists");
                }
                break;
        case 2:
                {
		    	    write(peer_soc_port, "No Group Exists", 16);
                    append_Tracker_log("\nNo Group Exists");
                }
                break;
        case 3:
                {
                    write(peer_soc_port, "No client Present", 18);
                     append_Tracker_log("\nNo client Present");
                }
                break;
        case 4:
                {
                    // only 512kb per block  write to peer socket port  cout<<"Hello Tracker"<<endl;
                    write(peer_soc_port, "Uploading", 9);
                    append_Tracker_log("\nUploading....");
                    char file_buffer[524288];
                    for(int i=0;i<524288;i++)
                    {
                        file_buffer[i]=0;
                    }
                    string strt="file";
                    //The read() function reads data previously written to a file.
                    //append_Tracker_log("\nUploading....");
                    if (read(peer_soc_port, file_buffer, 524288))
                    {
                    	strt+="";
                    	if (string(file_buffer) == "error") 
                           { return; }
                        //append_Tracker_log("\nUploading...."); 
                         if(strt=="exit")
                    	    { return; }
                    	vector<string> current_file_details = break_string_using_delimiter(string(file_buffer), "*$*");
                        // get name of the file using delimiter /
                    	string filename = break_string_using_delimiter(string(current_file_details[0]), "/").back();
                        //   strt;
                    	string hashes = "";

                        size_t z=4;
                        while(z < current_file_details.size())
                        {
                            	strt+="filename";
                            	hashes += current_file_details[z];
                                // write(peer_soc_port, "Uploaded", 8);
                                // append_Tracker_log("\nUploaded");
                            	if(strt=="exit")
                            	return;
                            	if (z != current_file_details.size() - 1) hashes += "*$*";

                            z=z+1;
                        }
                   
                    	piece_wise[filename] = hashes;

                    	if (uploadList[group_id].find(filename) != uploadList[group_id].end())
                    	{
                            // if filename is not found in map
                    		uploadList[group_id][filename].insert(client_id);
                    	}
                    	else
                    	{
                    		uploadList[group_id].insert({ filename,{client_id} });
                    	}

                    	cur_file_size[filename] = current_file_details[2];
                        //write() writes up to count bytes from the buffer starting at buf to the file referred to by the file descriptor fd.
                    	write(peer_soc_port, "Uploaded", 8);
                        append_Tracker_log("\nUploaded");
                    }

                }

    }


}

//PPP ---
void function_to_download_file(string &group_id, string &file_name, string &file_path, int &peer_soc_port, string &client_id)
{
    int fl_sw=0;
	if (!is_valid_path(file_path)) // is the download file path valid
        fl_sw=1;
	else if (grp_members.find(group_id) == grp_members.end())  // if the group id is NOT found in group members
        fl_sw=2;
	else if (grp_members[group_id].find(client_id) == grp_members[group_id].end()) // if client id is NOT found
        fl_sw=3;
	else
        fl_sw=4;

    switch(fl_sw)
    {
        case 1:
                {
		            write(peer_soc_port, "No file exists", 14);
                    append_Tracker_log("\nNo file exists");
                }
                break;
        case 2:
                {
		            write(peer_soc_port, "No Group Exists", 16);
                    append_Tracker_log("\nNo Group Exists");
                }
                break;
        case 3:
                {
		            write(peer_soc_port, "No client Present", 18);
                    append_Tracker_log("\nNo client Present");
                }
                break;
        case 4:
                {
                    string strt;
                    char file_buffer[524288];
                    for(int i=0;i<524288;i++)
                    {
                        file_buffer[i]=0;
                    }
                    write(peer_soc_port, "Downloading", 13);
                    append_Tracker_log("\nDownloading....");
                    strt="download";
                    if (read(peer_soc_port, file_buffer, 524288))
                    {
                    	if(strt=="exit")
                    	 return;
                    	vector<string> current_file_details = break_string_using_delimiter(string(file_buffer), "*$*");
                        strt+=" ";
                    	string message_reply = "";
                    	// strt;
                    	if (uploadList[group_id].find(current_file_details[0]) != uploadList[group_id].end())
                    	{
        
                    		for (auto m: uploadList[group_id][current_file_details[0]])
                    		{
                    			if(strt=="exit")
                    				return;
                    			if (if_if_logged_in[m])
                    				message_reply += convert_port[m] + "*$*";
        
                    		}
                             strt+="";
                    		message_reply += cur_file_size[current_file_details[0]]+"&&&"+piece_wise[current_file_details[0]];
                            //write() writes up to count bytes from the buffer starting at buf to the file referred to by the file descriptor fd.
                    		write(peer_soc_port, &message_reply[0], message_reply.length());
                    		uploadList[group_id][file_name].insert(client_id);
                    	}
                    	else
                    	{
                            //ssize_t write(int fd, const void *buf, size_t count);
                    		write(peer_soc_port, "File not found", 14);
                             append_Tracker_log("\nFile not found");
                    	}
                    }

                }

    }
}


void FUNCTION_CREATE_USER(vector<string> &data_inp_vec,int &peer_soc_port) //1  -->
{

    if (data_inp_vec.size() == v_s)
    {
        string usr_pass=data_inp_vec[2];
        string identify_usr=data_inp_vec[1];
        
        int flag;

        if (!(login.find(identify_usr) == login.end()))
        {
            // User found , already exists
            flag = -1;
            write(peer_soc_port, "User already exists", 20);
            append_Tracker_log("\nUser already exists");
        }
        else
        {   
            //if the user is NOT Found insert the user name and password
            login.insert({ identify_usr, usr_pass });
            flag = 0;
            write(peer_soc_port, "Account Sucessfully Created", 28);
            append_Tracker_log("\nAccount Sucessfully Created");
            
        }

    }
    else
    {
        write(peer_soc_port, "Invalid Arguments", 18);
        append_Tracker_log("\nInvalid Arguments");
    }


}

void FUNCTION_LOGIN(vector<string> &data_inp_vec,int &peer_soc_port,string &client_id) //2 -->
{
    if (data_inp_vec.size() == 3)
    {
        // extract client id from second argument
        client_id = data_inp_vec[1];
        // flag to check login vlidation
        int flag = check_login_validate(data_inp_vec[1], data_inp_vec[2]);
        char login_buffer[1024];
        string cur_client_address;

        switch(flag)
        {
            case 1:
                    {
                        // if flag==1
                        write(peer_soc_port, "User has already logged in", 28);
                        append_Tracker_log("\nUser has already logged in");
                    }
                    break;
            case 0:
                    {// if flag==0
                        write(peer_soc_port, "Login Successful", 17);
                        append_Tracker_log("\nLogin Successful");
                        read(peer_soc_port, login_buffer, 1024);
                        cur_client_address = string(login_buffer);
                        convert_port[client_id] = cur_client_address;
                    }   
                    break;
            default:
                    {
                        // else
                        write(peer_soc_port, "User_id/Password is incorrect", 30);
                        append_Tracker_log("\nUser_id/Password is incorrect");
                    }

        }

    }
    else
    {
        write(peer_soc_port, "Invalid Arguments", 18);
        append_Tracker_log("\nInvalid Arguments");
    }

}

void FUNCTION_ACCEPT_REQUEST(vector<string> &data_inp_vec,int &peer_soc_port,string &client_id)//6  --->
{
    if (data_inp_vec.size() == v_s)
    {
        // valid no of aruments ie.. 2 arguments
        string group_id = data_inp_vec[1];
        string identify_usr = data_inp_vec[2];
        if (admin_groups.find(group_id) == admin_groups.end())
        {
            // the group is not found in the admin map
            write(peer_soc_port, "No group found", 13);
            append_Tracker_log("\nNo group found");
            
        }
        else if (admin_groups.find(group_id)->second != client_id)
        {
            // Admin priviliges not present so request declioned
            write(peer_soc_port, "You are not admin", 17);
            append_Tracker_log("\nYou are not admin");
        }
        else
        {
            //In case admin priviliges are present
            write(peer_soc_port, "Request accepted.", 18);
            append_Tracker_log("\nRequest accepted");
            grp_members[group_id].insert(identify_usr);
            //ssize_t write(int fd, const void *buf, size_t count); write() writes up to count bytes from the buffer starting at buf to the file referred to by the file descriptor fd.
            grp_requests[group_id].erase(identify_usr);
        }
    }
    else 
    {
        // if (data_inp_vec.size() != v_s)  if number of arguments are NOT equal to two then it is invalid command 
        write(peer_soc_port, "Invalid Arguments", 18);
        append_Tracker_log("\nInvalid Arguments");
    }
    
}

void FUNCTION_CREATE_GROUP(vector<string> &data_inp_vec,int &peer_soc_port,string &client_id,string &client_group_id)//7 --
{ 
     cout<<client_group_id<<" qwertt 2"<<endl;
    if (data_inp_vec.size() == 2)
    {
        // extracted the group id from input vector
        string group_id = data_inp_vec[1];
        int flag=0;
        for (string m: group_list)
        {
            // iterate through group list to see if the group exists or not
            if (m == group_id)
            {
                // do not create grouop if the group already exists
                write(peer_soc_port, "Group Already Exists", 21);
                append_Tracker_log("\nGroup Already Exists");
                flag=1;
            }
        }
        // only create group if the flag is false
        if(flag==0)
        {
            group_list.push_back(group_id);
            admin_groups.insert({ group_id, client_id });
            grp_members[group_id].insert(client_id);
            client_group_id = data_inp_vec[1];
            //ssize_t write(int fd, const void *buf, size_t count);
            write(peer_soc_port, "Group Successfully Created", 35);
            append_Tracker_log("\nGroup Successfully Created");
        }

    }
    else
    {
        // if (data_inp_vec.size() != 2)
        write(peer_soc_port, "Invalid Arguments", 18);
        append_Tracker_log("\nInvalid Arguments");
    }
    
}

void  FUNCTION_JOIN_GROUP(vector<string> &data_inp_vec,int &peer_soc_port,string &client_id)//8  PPP ---
{

    if (data_inp_vec.size() == 2)
    {
        int sw_fl=0;
        string group_id = data_inp_vec[1];
        // cout<<"okayee "<<sw_fl<<"  "<<group_id<<endl;

        if (admin_groups.find(group_id) == admin_groups.end())
            sw_fl=1;
        else if (grp_members[group_id].find(client_id) != grp_members[group_id].end())
            sw_fl=2;
        else
            sw_fl=3;

        switch(sw_fl)
        {
            case 1: {
                          write(peer_soc_port, "Wrong Group ID", 14);
                          append_Tracker_log("\nWrong Group ID");
                    }
                    break;
            case 2: {
                         write(peer_soc_port, "You are already present in group", 32);
                         append_Tracker_log("\nYou are already present in group");
                    }
                    break;
            case 3: {
                            grp_requests[group_id].insert(client_id);
                            write(peer_soc_port, "Group request successfully sent", 31);
                             append_Tracker_log("\nGroup request successfully sent");
                    }
    
        }
    }
    else
    {
            // if (data_inp_vec.size() != 2)
        write(peer_soc_port, "Invalid Arguments", 18);
        append_Tracker_log("\nInvalid Arguments");
    }
    
}

void FUNCTION_LIST_REQUESTS(vector<string> &data_inp_vec,int &peer_soc_port,string &client_id)//9  PPP ---
{

    if (data_inp_vec.size() == 2)
    {
        int sw_fl=0;
        string requests = "";
        string group_id = data_inp_vec[1];
        

        if (admin_groups.find(group_id) == admin_groups.end())
        {
            write(peer_soc_port, "No group found*$*", 18);
            append_Tracker_log("\nNo group found");
        }

        if (admin_groups[group_id] != client_id)
             sw_fl=1;
        
        else if (grp_requests[group_id].size() == 0)
            sw_fl=2;
        else
            sw_fl=3;

        switch(sw_fl)
        {
                case 1:
                        {
                            write(peer_soc_port, "You are not admin*$*", 20);
                            append_Tracker_log("\nNo group found");
                        }
                        break;
                case 2:
                        {
                            write(peer_soc_port, "No requests", 15);
                            append_Tracker_log("\nNo requests");
                        }   
                        break;
                case 3:
                        {
                            auto k=grp_requests[group_id].begin();
                            while(k != grp_requests[group_id].end())
                            {
                                requests += string(*k) + "*$*";
                                k++;
                            }
                            //ssize_t write(int fd, const void *buf, size_t count);
                            write(peer_soc_port, &requests[0], requests.length());
                        }     

        }
    }
    else
    {
        // if (data_inp_vec.size() != 2)
        write(peer_soc_port, "Invalid Arguments", 18);
        append_Tracker_log("\nInvalid Arguments");
    }

}


void FUNCTION_LEAVE_GROUP(vector<string> &data_inp_vec,int &peer_soc_port,string &client_id)//10  PPP ---
{
    int tw_val=0;
    if (data_inp_vec.size() == 2)
    {
    
            string group_id=data_inp_vec[1];
            if (admin_groups.find(group_id) == admin_groups.end())
                tw_val=1;
            else if (grp_members[group_id].find(client_id) != grp_members[group_id].end())
                tw_val=2;
            else
                tw_val=3;

            switch(tw_val)
            {
                case 1:
                        {
                            write(peer_soc_port, "No group found", 14);
                            append_Tracker_log("\nNo group found");
                        }
                        break;

                case 2:
                        {
                            if (admin_groups[group_id]!= client_id)
                            {
                                grp_members[group_id].erase(client_id);
                                cout<<"";
                                write(peer_soc_port, "Group left succesfully", 23);
                                append_Tracker_log("\nGroup left succesfully");
                                
                            }
                            else
                            {
                                //write() writes up to count bytes from the buffer starting at buf to the file referred to by the file descriptor fd.
                                write(peer_soc_port, "You are admin, you cannot leave", 35);
                                append_Tracker_log("\nYou are admin, you cannot leave");
                            }
                        }
                        break;
                case 3:
                        {
                            int identifier=-1;
                            if(identifier>0)
                                cout<<endl;
        
                            identifier=identifier+1;
                            //ssize_t write(int fd, const void *buf, size_t count);
                            write(peer_soc_port, "You are not part of group", 25);
                            append_Tracker_log("\nYou are not part of group");
                        }
                        
            }
    }
    else 
    {
        // if (data_inp_vec.size() != 2)
        write(peer_soc_port, "Invalid Arguments", 18);
    }
    
}


void FUNCTION_LIST_GROUPS(vector<string> &data_inp_vec,int &peer_soc_port) //11  PPP --
{

    int fl_sw=0;
    int identifier=-1;
    if (data_inp_vec.size() != 1)
        fl_sw=1;
    else if (group_list.size() == 0)
        fl_sw=2;
    else
        fl_sw=3;


    switch(fl_sw)
    {
        case 1:
                {
                    if(identifier>0)
                        {
                            cout<<"Invalid arguments"<<endl;
                             append_Tracker_log("\nInvalid arguments");
                        }
                    
                    //write() writes up to count bytes from the buffer starting at buf to the file referred to by the file descriptor fd.
                    write(peer_soc_port, "Invalid Arguments", 18);
                    append_Tracker_log("\nInvalid arguments");
                    identifier=identifier+1;                    
                }
                break;
        case 2:
                {
                    //ssize_t write(int fd, const void *buf, size_t count);
                    write(peer_soc_port, "No groups avail*$*", 18);
                    append_Tracker_log("\nNo groups avail");
                }    
                break;   
        case 3:
                {
                    string groups = "";
                    int i=0;
                    while(i < group_list.size())
                    {
                            while(i<0)
                            {
                                cout<<groups;
                                cout<<endl;

                            }
                            groups += group_list[i] + "*$*";                       
                        i=i+1;
                    }

                    if(identifier==-9999)
                      {
                        cout<<"Done";
                        append_Tracker_log("\nDone");
                        // printf("Done");
                      }  
                    //ssize_t write(int fd, const void *buf, size_t count);
                    write(peer_soc_port, &groups[0], groups.length() + 1);                   
                } 

    }



}


void FUNCTION_LIST_FILES(vector<string> &data_inp_vec,int &peer_soc_port) //12  PPP --
{

    if (data_inp_vec.size() != 2)
    {
        //ssize_t write(int fd, const void *buf, size_t count);
        write(peer_soc_port, "Invalid Arguments", 18);
        append_Tracker_log("\nInvalid Arguments");
    }

    else
    {
        int sw_fl=0;
        string group_id = data_inp_vec[1];
        if (admin_groups.find(group_id) == admin_groups.end())
            sw_fl=1;
        else if (uploadList[group_id].size() != 0)
            sw_fl=2;
        else
            sw_fl=3;


        switch(sw_fl)
        {
            case 1:
                    {write(peer_soc_port, "No group found*$*", 18);
                    append_Tracker_log("\nNo group found");}
                    break;
            case 2:
                    {
                            string list_files_reply = "";
                            for (auto i: uploadList[group_id])
                            {
                                list_files_reply += i.first + "*$*";
                            }

                            write(peer_soc_port, &list_files_reply[0], list_files_reply.length());
                    }    
                    break;   
            case 3:
                    {write(peer_soc_port, "No files found*$*", 18);
                    append_Tracker_log("\nNo files found");} 

        }
    }

}


void FUNCTION_STOP_SHARE(vector<string> &data_inp_vec,int &peer_soc_port,string &client_id) //13  PPP --
{

    if (data_inp_vec.size() != 3)
    {
        write(peer_soc_port, "Invalid Arguments", 17);
        append_Tracker_log("\nInvalid Arguments");
    }
    else
    {
        int fl1=0;
        string group_id = data_inp_vec[1];
        int fl2=0;
        string file_name = data_inp_vec[2];
        int fl3=0;
        if (admin_groups.find(group_id) == admin_groups.end())
        {
            fl1=1;fl2=0;fl3=0;
        }
        else if (uploadList[group_id].find(file_name) != uploadList[group_id].end())
        {
            fl2=1;fl1=0;fl3=0;
        }
        else
        {
            fl1=0;fl2=0;fl3=1;
        }
        if(fl1==1)
        {
            write(peer_soc_port, "No group found", 13);
            append_Tracker_log("\nNo group found");
        }
        if(fl2==1)
        {
            uploadList[group_id][file_name].erase(client_id);
            //ssize_t write(int fd, const void *buf, size_t count); write() writes up to count bytes from the buffer starting at buf to the file referred to by the file descriptor fd.
            write(peer_soc_port, "Stopped sharing the file from group", 35);
            append_Tracker_log("\nStopped sharing the file from group");
            if (uploadList[group_id][file_name].size() == 0)
            {
                uploadList[group_id].erase(file_name);
            }
        }
        if(fl3==1)
        {
             //ssize_t write(int fd, const void *buf, size_t count);
             write(peer_soc_port, "No file shared in the group", 28);
             append_Tracker_log("\nNo file shared in the group");
        }
    }


}



//this is to establish_conn of clients using threads and handle commands
void establish_conn(int peer_soc_port) //-- OK
{
	cout << "Thread created " << to_string(peer_soc_port) <<endl;
    append_Tracker_log("\nThread created");
    string client_id, client_group_id, current, in_data;
	while (1)
	{
        char buffer[2048];
        for(int i=0;i<2048;i++)
        {
            buffer[i]=0;
        }

		vector<string> data_inp_vec;
        //The read() function reads data previously written to a file.
        int a=read(peer_soc_port, buffer, 2048);
		if (a<= 0)
		{
			if_if_logged_in[client_id] = false;
            //The close() function shall deallocate the file descriptor indicated by fildes.
			close(peer_soc_port);
			break;
		}

		in_data = string(buffer);// explicit type case buffer stream data to string
		cout << "Request from client " << in_data <<endl;
        append_Tracker_log("\nRequest from client");
		stringstream stringarray(in_data);
        //A stringstream associates a string object with a stream allowing you to read from the string as if it were a stream (like cin). To use stringstream, we need to include sstream header file. The stringstream class is extremely useful in parsing input. 
		
        while (stringarray >> current)
			data_inp_vec.push_back(current);


        unordered_map<int,string> track_map;
        	track_map[1]="create_user";
            track_map[2]="login";
            track_map[3]="logout";
            track_map[4]="upload_file";
            track_map[5]="download_file";
            track_map[6]="accept_request";
            track_map[7]="create_group";
            track_map[8]="join_group";
            track_map[9]="list_requests";
            track_map[10]="leave_group";
            track_map[11]="list_groups";
            track_map[12]="list_files";
            track_map[13]="stop_share";
            track_map[14]="show_downloads";

            string to_execute="";
            int flag_pos=0;
            for(int i=1;i<=track_map.size();i++)
            {
                if(data_inp_vec[0]==track_map[i])
                {
                    to_execute=track_map[i];
                    flag_pos=i;
                    break;
                }
            }

        switch(flag_pos)
        {
            case 1:
                    {
                            FUNCTION_CREATE_USER(data_inp_vec,peer_soc_port);
   
                    }
                    break;
            case 2:
                    {
                            FUNCTION_LOGIN(data_inp_vec,peer_soc_port,client_id);
                            
                    }
                    break;
            case 3:
                    {
                                if_if_logged_in[client_id] = false;
                                write(peer_soc_port, "Logout Successful", 20);
                                append_Tracker_log("\nLogout Successful");
                    }
                    break;
            case 4:
                    {
                            if (data_inp_vec.size() != 3)
                            {
                                write(peer_soc_port, "Invalid Arguments", 18);
                                append_Tracker_log("\nInvalid Arguments");
                                //close(peer_soc_port);
                                
                            }
                            else
                            {
                                function_to_file_upload(data_inp_vec[1], data_inp_vec[2], peer_soc_port, client_id);
                            }

                    }
                    break;
            case 5:
                    {

                            if (data_inp_vec.size() != 4)
                            {
                                write(peer_soc_port, "Invalid Arguments", 18);
                                append_Tracker_log("\nInvalid Arguments");
                            }
                            else
                            {
                                function_to_download_file(data_inp_vec[1], data_inp_vec[2], data_inp_vec[3], peer_soc_port, client_id);
                            }

                    }
                    break;
            case 6:
                    {
                            FUNCTION_ACCEPT_REQUEST(data_inp_vec,peer_soc_port,client_id);

                    }
                    break;
            case 7:
                    {
                                FUNCTION_CREATE_GROUP(data_inp_vec,peer_soc_port,client_id,client_group_id);
                    }
                    break;
            case 8:
                    {
                                FUNCTION_JOIN_GROUP(data_inp_vec,peer_soc_port,client_id);
                    }
                    break;
            case 9:
                    {

                                FUNCTION_LIST_REQUESTS(data_inp_vec,peer_soc_port,client_id);

                    }
                    break;
            case 10:
                    {
                            FUNCTION_LEAVE_GROUP(data_inp_vec,peer_soc_port,client_id);

                    }
                    break;
            case 11:
                    {
                                FUNCTION_LIST_GROUPS(data_inp_vec,peer_soc_port);

                    }
                    break;
            case 12:
                    {
                            FUNCTION_LIST_FILES(data_inp_vec,peer_soc_port);

                    }
                    break;
            case 13:
                    {
                            FUNCTION_STOP_SHARE(data_inp_vec,peer_soc_port,client_id);

                    }
                    break;
            case 14:
                    {
                            write(peer_soc_port, "Downloads", 10);
                            append_Tracker_log("\nDownloads");
                    }
                    break;
            default:

                            write(peer_soc_port, "Invalid Command", 16);
                            append_Tracker_log("\nInvalid Command");
                    break;

        }

	}

    //The close() function shall deallocate the file descriptor indicated by fildes. To deallocate means to make the file descriptor available for return by subsequent calls to open() or other functions that allocate file descriptors.
	close(peer_soc_port);
}

int main(int argument_count, char *argument_list[])  //--OK
{
    log_ptr=fopen("\n\nTracker_log.txt", "a");
	int n_skt;// this is used to for maintainig socket id
	struct sockaddr_in server_addr;
    append_Tracker_log("Tracker Logs:   ");
    append_Tracker_log("\n----------------\n");
    // argument 2 contains the tracker file name
    vector<string> t_ip_port=fetch_ip_port_tracker(argument_list[1]);
    // assign tracket listening port and ip
    t_ip=t_ip_port[0];
    t_port=stoi(t_ip_port[1]);

	int len_of_address = sizeof(server_addr);
	int options = 1;

    //The socket() call creates a socket in the specified domain and of the specified type
    n_skt = socket(AF_INET, SOCK_STREAM, 0);
	if (n_skt== 0)
	{
        cout<<"socket creation failed"<<endl;
        append_Tracker_log("\nsocket creation failed");
		return -1;
	}
    //s = socket(domain, type, protocol);

	cout << "Tracker socket created.\n";
    append_Tracker_log("\nTracker socket created.");
    //int setsockopt(int socket, int level, int option_name,const void *option_value, socklen_t option_len); The setsockopt() function shall set the option specified by the option_name argument, at the protocol level specified by the level argument, to the value pointed to by the option_value argument for the socket associated with the file descriptor specified by the socket argument.
    int x=setsockopt(n_skt, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &options, sizeof(options));
	if (x)
	{// -1 is error 
        cout<<"setsockopt of tracker"<<endl;
        append_Tracker_log("\nsetsockopt of tracker.");
		return -1;
	}
    // SO_REUSEADDR  Specifies that the rules used in validating addresses supplied to bind() should allow reuse of local addresses,if this is supported by the protocol.  This option takes an int value. This is a Boolean option sin_port contains the port in network byte order
    
    //sin_family is always set to AF_INET.
    server_addr.sin_family = AF_INET;
    
	int identifier;
 	//The htons function takes a 16-bit number in host byte order  and returns a 16-bit number in network byte order used in TCP/IP networks (the AF_INET or AF_INET6 address family)	   
	server_addr.sin_port = htons(t_port);
    identifier=0;
    // int inet_pton(int af, const char *restrict src, void *restrict dst);
    int r=inet_pton(AF_INET, &t_ip[0], &server_addr.sin_addr);
    if (r<= 0)
	{
        cout<<endl<<"Invalid address/ Address not supported"<<endl;
        append_Tracker_log("\nInvalid address/ Address not supported.");
		return -1;
	}

    identifier+=1;
    //    int bind(int sockfd, const struct sockaddr *addr, socklen_t len_of_address);    When a socket is created with socket(2), it exists in a name space (address family) but has no address assigned to it.  bind() assigns the address specified by addr to the socket referred to by the file descriptor sockfd.  len_of_address specifies the size, in bytes, of the address structure pointed to by addr. Traditionally, this operation is called “assigning a name to a socket”
    int z=bind(n_skt, (SA*) &server_addr, sizeof(server_addr));
	
    if (z< 0)
	{   // -1 is error

		{identifier+=1;
			return -1;
		}

		cout<<"Binding failed"<<endl;
        append_Tracker_log("\nBinding failed.");
        return -1;
	}
    cout<<endl<<"Binding completed."<<endl;
    append_Tracker_log("\nBinding completed.");

	//here we are amking a queue size of 5 int listen(int sockfd, int backlog); listen() marks the socket referred to by sockfd as a passive socket, that is, as a socket that will be used to accept incoming establish_conn requests using accept(2). The sockfd argument is a file descriptor that refers to a socket of type SOCK_STREAM or SOCK_SEQPACKET. The backlog argument defines the maximum length to which the queue of pending establish_conns for sockfd may grow.  If a establish_conn request arrives when the queue is full, the client may receive an error 
    int lt=listen(n_skt, 5);
	if (lt< 0)
	{   // -1 is error
        cout<<"listen failed"<<endl;
        append_Tracker_log("\nlisten failed.");
		return -1;
	}
    cout<<"Tracker Listening for clients"<<endl;
    append_Tracker_log("\nTracker Listening for clients.");	

	//The pthread_create() function is used to create a new thread, with attributes specified by attr, within a process. If attr is NULL, the default attributes are used. If the attributes specified by attr are modified later, the thread's attributes are not affected.
    pthread_t terminate_thrd;
    long long status=pthread_create(&terminate_thrd, NULL, exit_serv, NULL);
	if ( status== -1)
	{
        cout<<"pthread creation error"<<endl;
        append_Tracker_log("\npthread creation error.");
		return -1;
	}

    vector<thread> threads;
	while (1)
	{
		int peer_soc_port;
        peer_soc_port = accept(n_skt, (SA*) &server_addr, (socklen_t*) &len_of_address);
        // int accept(int sockfd, struct sockaddr *restrict addr,socklen_t *restrict addrlen); It extracts the first establish_conn request on queue of pending establish_conns for the listening socket, sockfd, creates a new connected socket, and returns a new file descriptor referring to that socket.
		if (peer_soc_port < 0)
		{
            cout<<"Acceptance error during establish_conn of client"<<endl;
            append_Tracker_log("\nAcceptance error during establish_conn of client.");
		}
        cout<<endl;

	    string str;
        cout<<"Connection of client Accepted"<<endl;
        append_Tracker_log("\nConnection of client Accepted.");

		//this is used for adding all clients to tracker in while loop
		threads.push_back(thread(establish_conn, peer_soc_port));
	}

    auto ij=threads.begin();
    while(ij != threads.end())
    {
		string strt="";
		strt+=" ";

		if (!(ij->joinable()))
		{
			strt+="";
		}
		else
		{   
            // if it is joinable join it with main to make sure it is executed
			ij->join();
		}

        ij=ij+1;
    }

    cout<<"Exit"<<endl;
    append_Tracker_log("\nExit.");
    fclose(log_ptr);
	return 0;
}
