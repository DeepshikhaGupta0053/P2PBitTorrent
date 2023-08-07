#include <sys/types.h>
#include <pthread.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <bits/stdc++.h>
#include <openssl/sha.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <signal.h>
// #include <string.h>

using namespace std;

#define block_capacity 524288
#define SIZE 32768
#define SA struct sockaddr
string FileName, t_ip, cl_ip;
int is_sign_login = 0, check_sha = 1;
uint16_t t_port, cl_port;
unordered_map<string, string> f_t_p;
int v_flag=1;
int gbl_flag=0;


vector<string> break_string_using_delimiter(string text, string delimiter)
{
    vector<string> res;
    size_t st, end = -1*delimiter.size();
    do 
    {
        st = end + delimiter.size();
        end = text.find(delimiter, st);
        res.push_back(text.substr(st, end - st));
        //cout << text.substr(st, end - st) << endl;

    } while (end != -1);
    return res;
}


unordered_map<string, string> downloads;
// map to maintain the downloads
vector<vector<string>> block_curr;
unordered_map<string, vector<int>> block_info;
vector<string> H_value_SHA_vicepiece;
// map to maintain the list of uploaded _files
unordered_map<string, unordered_map<string, bool>> upload_list;

long long calculate_size_of_file(char *rel_path_file) // OK
{
	string t = "";
	long size = -1;
	FILE *ptr_to_file = fopen(rel_path_file, "rb");

	if (ptr_to_file)
	{
		fseek(ptr_to_file, 0, SEEK_END);

		size = ftell(ptr_to_file) + 1;
		t = "gst";
		fclose(ptr_to_file);
	}
	else
	{
		cout<<"File not found."<<endl;
		return -1;
	}

	return size;
}



typedef struct detail_of_client_files
{
	string client_serv_socket_ip_port; 
	string name_of_the_file;
	long long capacity_of_file;
} detail_of_client_files;



void block_hash(string &H_value, string data_to_hash) //-OK
{
	// partial done
	unsigned char vec[20];
	// This function will calulate the single hash of the file and appemd delimiter at the end of it
	// vec[20] array will contai our hash value
	if (SHA1(reinterpret_cast<const unsigned char *>(&data_to_hash[0]), data_to_hash.length(), vec))
	{
		int p=0;
		while(p<20)
		{
			// collecting hash value from vec array from 
			char arr_curr[3];
			sprintf(arr_curr, "%02x", vec[p] & 0xff);
			H_value += string(arr_curr);	
			p++;
		}
	}
	else
	{
		// ifSHa did not return correct length of hash then error
		cout<<"Error in hashing"<<endl;
	}
	H_value += "*$*";
}


string calculate_full_hash(char *rel_path_file) //--OK
{
	// finding out the tital size of the file using this we will calculate the current segmet number
	long long total_size_erp = calculate_size_of_file(rel_path_file);
	vector<string> v;


	if (total_size_erp == -1)
		return "XYZ";
	

	string H_value_complete = "", data_to_hash = "", test = "";
	int motucs, curt;

	//FILE *fopen(const char *file_name, const char *mode_of_operation); The fopen() method in C is a library function that is used to open a file to perform various operations which include reading, writing etc. along with various modes. If the file exists then the particular file is opened else a new file is created.
	FILE *ptr_to_file = fopen(rel_path_file, "r");
	int segment_current = (total_size_erp / block_capacity) + 1;
	char poll[SIZE + 1];

	if (ptr_to_file)
	{
		int k=1;
		while(k<=segment_current)
		{
			motucs = 0;
			while (motucs < block_capacity && (curt = fread(poll, 1, min(SIZE - 1, block_capacity - motucs), ptr_to_file)))
			{

				poll[curt] = '\0';
				motucs += strlen(poll);

				data_to_hash += poll;
				memset(poll, 0, sizeof(poll));
			}
			test="gggsssttt";
			block_hash(H_value_complete, data_to_hash);

			k=k+1;
		}

		fclose(ptr_to_file);

	}
	else
	{
		printf("File not found.\n");
	}

	int p=0;
	while(p<3)
	{
		H_value_complete.pop_back();
		p=p+1;
	}

	return H_value_complete;
}

void array_of_bit(string name_of_the_file, long long int low, long long int high, int last) //--OK
{
	int q=0;

	if(last!=q)
	{
		vector<int> tmp(high - low + 1, 1);
		block_info[name_of_the_file] = tmp;
	}
	else
		block_info[name_of_the_file][low] = 1;

}


typedef struct get_block_information  // PPP -OK
{
	// this struct is used to store every detail about block/chunk
	string client_serv_socket_ip_port; // for IP of serverClient/Peer
	string name_of_the_file; // stores the name of file for chunk/block
	long long block_number; // block no to identify the block
	string end_point;      // destination of block/chunk
} get_block_information;


string client_conn(char *Client_C, char *PortI, string p_mour) //PPP --OK
{
	uint16_t client_PORT = stoi(string(PortI));
	int client_sock = 0;
	struct sockaddr_in server_address_of_current_client;

	client_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (client_sock < 0)
	{
		cout<<endl<<" Socket creation error"<<endl;
		return "error";
	}

	server_address_of_current_client.sin_family = AF_INET;
	server_address_of_current_client.sin_port = htons(client_PORT);
	string d="";
	int ghi=inet_pton(AF_INET, Client_C, &server_address_of_current_client.sin_addr);
	if (ghi< 0)
	{
		cout<<"ERROR:";
		perror("Peer Connection Error(INET)");
	}
	d=d+"gst";
	int mah=connect(client_sock, (struct sockaddr *)&server_address_of_current_client, sizeof(server_address_of_current_client));
	if (mah< 0)
	{
		cout<<"ERROR";
		perror("Peer Connection Error");
	}
	//cout << "Connected to peer" << endl;

	string cmd_to_peer = break_string_using_delimiter(p_mour, "*$*").front();
	//////////////////////////////////////////////////////////////////////////////////

	unordered_map<int,string> cmd_map;
	cmd_map[1]="current_path_file";
	cmd_map[2]="current_chunk_vector_details";
	cmd_map[3]="current_chunk";
	
	int cmd_switch=0;
	for(long long int i=1;i<=cmd_map.size();i++)
	{
		if(cmd_map[i]==cmd_to_peer )
		{
			cmd_switch=i;
			break;
		}

	}

	switch(cmd_switch)
	{

		case 1:
				{
					int a=send(client_sock, &p_mour[0], strlen(&p_mour[0]), MSG_NOSIGNAL);
					if (a== -1)
					{
						cout<<"Error in socket peer in rel_path_file"<<endl;
						return "error";
					}
					
					//cout << "Sent command to peer - " << cmd_to_peer << endl;
					
					//char cc_message_rply[10240] = {0};
					char cc_message_rply[10240];
					for(int i=0;i<10240;i++)
					{
						cc_message_rply[i]=0;
					}


					int b=read(client_sock, cc_message_rply, 10240);
					if (b<0)
					{
						cout<<"error in socket reading in current_path"<<endl;
						return "error";
					}
					//cout << "server replied - " << string(cc_message_rply) << endl;
					f_t_p[break_string_using_delimiter(p_mour, "*$*").back()] = string(cc_message_rply);

				}
				break;

		case 2:
				{
					int a=send(client_sock, &p_mour[0], strlen(&p_mour[0]), MSG_NOSIGNAL);
					if ( a== -1)
					{
						cout<<"error in socket reading in current_chunk_vector"<<endl;
						return "error";
					}
					//cout << "Sent command to peer - " << cmd_to_peer << endl;
					
					// char cc_message_rply[10240] = {0};
					char cc_message_rply[10240];
					for(int i=0;i<10240;i++)
					{
						cc_message_rply[i]=0;
					}
					int b=read(client_sock, cc_message_rply, 10240);
					if (b< 0)
					{
						cout<<"error in socket reading in current_chunk_vector"<<endl;
						return "error";
					}
					//cout << string(cc_message_rply) << endl;
					close(client_sock);

					return string(cc_message_rply);

				}	
				break;

		case 3:
				{ //PPP

					string strt;
					if(strt=="chunk")
					strt=" ";
					int a=send(client_sock, &p_mour[0], strlen(&p_mour[0]), MSG_NOSIGNAL);
					if (a== -1)
					{
						cout<<"error in socket reading in current_chunk"<<endl;
						return "error";
					}
					//cout << "Sent command to peer - " << cmd_to_peer << endl;
					vector<string> tik = break_string_using_delimiter(p_mour, "*$*");
					//The send() function shall initiate transmission of a message from the specified socket to its peer.
					string hash = "";
					// declared variable to store hashed_value
					string location_path_of_destination = tik[3];
					// assign destination location to location_lo_file
					char *location_of_file = &location_path_of_destination[0];
					//This function converts a string, provided as an argument in the function call, to long long int. It parses str interpreting its content as an integral number of the specified base, which is returned as a value of type long long int.
					long long int block_number = stoll(tik[2]);
					
					char current_buff[block_capacity];
					int size_curr = 0;
					int count;
					// stoll  Interprets a signed integer value in the string str .

					string fuc = "";
					while (size_curr < block_capacity)
					{
						//The read() function reads data previously written to a file.
						count = read(client_sock, current_buff, block_capacity - 1);
						if (count <= 0)
						{
							break;
							cout<<"peer error";
						}

						current_buff[count] = 0;
						fstream outline(location_of_file, std::fstream::in | std::fstream::out | std::fstream::binary);
						//A combination of ofstream and ifstream: creates, reads, and writes to files
						outline.seekp(block_number * block_capacity + size_curr, ios::beg);
						//techie+=1;
						outline.write(current_buff, count);
						outline.close();
			
						size_curr += count;
						fuc += current_buff;
						bzero(current_buff, block_capacity);//The bzero() function erases the data in the n bytes of the memory starting at the location pointed to by s, by writing zeros (bytes containing '\0') to that area.
					}
					// calculate the block wise hash
					block_hash(hash, fuc);   ///////////////

					int r=0;
					while(r<3)
					{
						hash.pop_back();
						r++;
					}

					// if the h_value is not validated then check_sha is failed
					if (hash != H_value_SHA_vicepiece[block_number])
						check_sha = 0;


					vector<string> name_of_the_file = break_string_using_delimiter(string(location_of_file), "/");
					// block_hash(hash, fuc);
					string name_of_file = name_of_the_file.back();
					//the close() function shall deallocate the file descriptor indicated by fildes.
					array_of_bit(name_of_file, block_number, block_number, 0);
					return "done";

				}
				break;				
	}

	close(client_sock);
	return "done";


}

int function_to_file_upload(vector<string> data_inp_vec, int n_socket) //PPP --OK
{
	string name="file";
	char *location_of_file = &data_inp_vec[1][0];

	string name_of_the_file = break_string_using_delimiter(string(location_of_file), "/").back();
	name+="upload";
    int flg1=0,flg2=0;
	
    if (!(upload_list[data_inp_vec[2]].find(name_of_the_file) != upload_list[data_inp_vec[2]].end()))
	{
        // if it is found in upload_list
		upload_list[data_inp_vec[2]][name_of_the_file] = true;
		name="upload";
		f_t_p[name_of_the_file] = string(location_of_file);
	}
    // if (upload_list[data_inp_vec[2]].find(name_of_the_file) != upload_list[data_inp_vec[2]].end())
	else
    {   
		int a=0;
		cout << "File already uploaded" << endl;

		if (send(n_socket, "error", 5, MSG_NOSIGNAL) != -1)
		{
            a+=22;
            return 0;
		}
        a+=0;
        cout<<"Error in uploading"<<endl;
        return -1;

	}

	// calculate the hash value and if its XYZ retun ;
	string block_vice_H_value = calculate_full_hash(location_of_file);
	if (block_vice_H_value == "XYZ")
		return 0;

	string H_value_of_fil = "";
	// 	Constructs an ostringstream object with an empty sequence as content. Internally, its ostream base constructor is passed a pointer to a stringbuf object constructed with an argument
	ostringstream memory_buff_for_characters;
	//This data type represents the input file stream and is used to read information from files.
	ifstream in(location_of_file);
	// rdbuf Returns the associated stream buffer. If there is no associated stream buffer, returns a null pointer.
	memory_buff_for_characters << in.rdbuf();
	string contents = memory_buff_for_characters.str();
	string iriya = "";

	unsigned char current_file_buffer_hash[32];

    if (!(!SHA256(reinterpret_cast<const unsigned char *>(&contents[0]), contents.length(), current_file_buffer_hash)))
	{
        int h=0;
        while(h<32)
        {
			char cur_buff[3];
			sprintf(cur_buff, "%02x", current_file_buffer_hash[h] & 0xff);
			iriya += string(cur_buff); 
            h=h+1;
        }
        
	}
    else
    {
		name="SHA";
        cout<<"Error in hashing"<<endl;
	}

	string information_file = "";
	H_value_of_fil = iriya;
	string capacity_of_file = to_string(calculate_size_of_file(location_of_file));  //////////////
	// int f_download_file=0,f_list_groups=0,f_list_requests=0,f_list_files=0,f_stop_share=0,f_show_downloads=0;
	information_file += string(location_of_file) + "*$*";
	name="$";

	information_file += string(cl_ip) + ":" + to_string(cl_port) + "*$*";
	name="$++";

		// int lp=valid_size,x=1;;
		// for(int i=lp;i>=0;i--)	x++;
		
	information_file += capacity_of_file + "*$*";
	information_file += H_value_of_fil + "*$*";
	
	name="$--";
	information_file += block_vice_H_value;


    int d=send(n_socket, &information_file[0], strlen(&information_file[0]), MSG_NOSIGNAL);
	if (d==-1)
	{
        cout<<"Error in uploading"<<endl;
		return -1;
	}

	char cc_message_rply[10240];
	for(int i=0;i<10240;i++)
		cc_message_rply[i]=0;

	//The read() function shall attempt to read nbyte bytes from the file associated with the open file descriptor, fildes, into the buffer pointed to by buf. The behavior of multiple concurrent reads on the same pipe, FIFO, or terminal device is unspecified.
	read(n_socket, cc_message_rply, 10240);
	cout << cc_message_rply << endl;

	array_of_bit(name_of_the_file, 0, stoll(capacity_of_file) / block_capacity + 1, 1);

	return 0;
}


void handle_thread1_in_block_selection(detail_of_client_files *client_file_info)//PPP --OK
{
	// attach the name of the file for identification purpose at destinatio
	string p_mour = "current_chunk_vector_details*$*" + string(client_file_info->name_of_the_file);
	// extract serverPeerAddress ip _port 
	vector<string> serverPeerAddress = break_string_using_delimiter(string(client_file_info->client_serv_socket_ip_port), ":");
	// established the connection with client for communication
	string response = client_conn(&serverPeerAddress[0][0], &serverPeerAddress[1][0], p_mour);

	// run a loop less than size of current_block and push the ip_port info onto it
	size_t j=0;
	while(j<block_curr.size())
	{

		string strt="";
    
		if (response[j] == '1')
		{
			block_curr[j].push_back(string(client_file_info->client_serv_socket_ip_port));
		}
		else
		{
			strt+="error";
		}
		j=j+1;
	}

	delete client_file_info;
}

void handle_thread2_in_block_selection(get_block_information *clie_xy) //PPP --OK
{
	// get the anme of the file into a string variable
 	string name_of_the_file = clie_xy->name_of_the_file;
	// get the ip and port into string array by using delimiter 
	vector<string> client_C = break_string_using_delimiter(clie_xy->client_serv_socket_ip_port, ":");
	// store the dewtination into the end point variable
	string end_point = clie_xy->end_point;
	// identify the block number
	long long int block_number = clie_xy->block_number;
	string s= to_string(block_number);	
	// create a string mentioning current_chunk with destination to connect to
	string p_mour = "current_chunk*$*" + name_of_the_file + "*$*" + to_string(block_number) + "*$*" + end_point;
	s+=" ";
	// create a connection to end_point 
	client_conn(&client_C[0][0], &client_C[1][0], p_mour);
	if(s=="error")
	 return;

	delete clie_xy;
}


void selection_block_alogo(vector<string> data_inp_vec, vector<string> peers) // teha working download
{
	//prioritize the download of the piece that is the rarest in the network.
	string fname;
	vector<thread> threads;

	long long int capacity_of_file = stoll(peers.back());
	peers.pop_back();
	//  long long int stoll (const string&  str, size_t* idx = 0, int base = 10)
	// This function converts a string, provided as an argument in the function call, to long long int.
	long long int curret_seg = (capacity_of_file / block_capacity) + 1;
	block_curr.clear();
	block_curr.resize(curret_seg);
	// 		int valid_size=4;
	// 		int lp=valid_size,x=1;;
	vector<thread> threads2;
	fname=data_inp_vec[0];

	size_t cn=0;
	while(cn<peers.size())
	{
			detail_of_client_files *pf = new detail_of_client_files();
			// if (false && 2 == 2) {cout << "piecewise";}
			pf->capacity_of_file = curret_seg;
			pf->client_serv_socket_ip_port = peers[cn];
			
			int temp = 0;
			thread t;
			pf->name_of_the_file = data_inp_vec[2];
			threads.push_back(thread(handle_thread1_in_block_selection, pf));		
		
		cn=cn+1;
	}

	auto mp=threads.begin();
	while(mp != threads.end())
	{
		if (mp->joinable())
			{
				mp->join();
			}
		mp=mp+1;
	}


	size_t qw=0;
	while(qw < block_curr.size())
	{
		if (block_curr[qw].size() == 0)
		{
			cout << "All parts of the file are not available." << endl;
			return;
		}
		qw=qw+1;
	}

	threads.clear();
	srand((unsigned)time(0));
	long long int rec_segments = 0;

	string des_path = data_inp_vec[3] + "/" + data_inp_vec[2];
	
	FILE *file_pointer = fopen(&des_path[0], "w");
	if (file_pointer == 0)
	{
		cout<<"The file already exists."<<endl;
		fclose(file_pointer);
		return;
	}
	fclose(file_pointer);
	//The fclose() function closes a stream pointed to by stream . This function deletes all buffers that are associated with the stream before closing it.
	string ss(capacity_of_file, '\0');

	int t = 0;
	fstream in(&des_path[0], ios::out | ios::binary);
    fname+=to_string(t);
	//write() writes up to count bytes from the buffer starting at buf to the file referred to by the file descriptor fd.
	in.write(ss.c_str(), strlen(ss.c_str()));
	in.close();
	// 		int valid_size=4;
	// 		int lp=valid_size,x=1;
	block_info[data_inp_vec[2]].resize(curret_seg, 0);
	check_sha = 1;

	vector<int> tmp(curret_seg, 0);
	block_info[data_inp_vec[2]] = tmp;

	string peerToGetFilepath;

	while (rec_segments < curret_seg)
	{

		// for(int i=0;i<1024;i++){	data_inp_clin[i]=0;}
		long long int randompiece;
		while (true)
		{
			randompiece = rand() % curret_seg;
			if (block_info[data_inp_vec[2]][randompiece] == 0)
				break;
		}

		long long int peersWithThisPiece = block_curr[randompiece].size();
		string randompeer = block_curr[randompiece][rand() % peersWithThisPiece];
		string dest = data_inp_vec[3] + "/" + data_inp_vec[2];
		// 	for(int i=lp;i>=0;i--)
		// x++;
		get_block_information *req = new get_block_information();
		req->end_point = dest;
		req->client_serv_socket_ip_port = randompeer;
		// struct sockaddr_in serv_addr;
		req->block_number = randompiece;
		req->name_of_the_file = data_inp_vec[2];
		
		block_info[data_inp_vec[2]][randompiece] = 1;
         thread th;
        // pthread_t thread_server;  long long status=pthread_create(&thread_server, NULL, server_func, NULL);
		threads2.push_back(thread(handle_thread2_in_block_selection, req));
		rec_segments++;
        if(fname=="break")
		  break;
		// 		// 	    string statement="";
		// 		// vector<string> res_tracker;
		
		peerToGetFilepath = randompeer; 
	}

	auto kl=threads2.begin();
	while(kl<threads2.end())
	{
		if (kl->joinable()){
			kl->join();
		}
		kl=kl+1;
	}

	if (check_sha == 0)
	{
		cout << "Downloaded completed. The file may be corrupted.";
		printf("\n");
	}
	// 		cout<<"sha sha"<<endl;
	if(check_sha!=0)
	{
		cout << "Download completed. No corruption detected." << endl;
	}
	// when file is downloaded insert into downloads vector
	downloads.insert({data_inp_vec[2], data_inp_vec[1]});
	// get server address by dividing string usin :
	vector<string> serverAddress = break_string_using_delimiter(peerToGetFilepath, ":");

	fname;
	// establish client connection f---> download 
	client_conn(&serverAddress[0][0], &serverAddress[1][0], "current_path_file*$*" + data_inp_vec[2]);
	fname+="";
	return;
}


int function_to_download_file(vector<string> data_inp_vec, int n_socket) // -- OK f-name change
{
	// partially done
 
	int valid_size=4;
    if((data_inp_vec.size() == valid_size))
    {

		int lp=valid_size,x=1;;
		for(int i=lp;i>=0;i--)
		{
			x++;
		}

        int abc=1;
        string information_file = "";
        if(abc>0)
        {
            int t1;
            t1=t1+1;
            information_file += data_inp_vec[2] + "*$*";
            information_file += data_inp_vec[3] + "*$*";
            information_file += data_inp_vec[1];
        }
        
        int s=send(n_socket, &information_file[0], strlen(&information_file[0]), MSG_NOSIGNAL);
  
        if(s!=-1)
        {

            char cc_message_rply[524288] = {0};
            read(n_socket, cc_message_rply, 524288);
  
            if (!(string(cc_message_rply) == "File not found"))
            {
                vector<string> ran = break_string_using_delimiter(string(cc_message_rply), "&&&");
					// // printf("Error in downloading\n");
					// cout<<"Error in downloading"<<endl;
                vector<string> peersWithFile = break_string_using_delimiter(ran[0], "*$*");

				// parse string uding delimiter

                vector<string> tmp = break_string_using_delimiter(ran[1], "*$*");

                H_value_SHA_vicepiece = tmp;

                selection_block_alogo(data_inp_vec, peersWithFile);
                return 0;
            }
            else
            {
                cout << cc_message_rply << endl;
                return 0;
            }

        }
        else
        {
            cout<<"Error in downloading"<<endl;
            return -1;
        }

    }
    else
		return 0;


}


int establish_conn(vector<string> data_inp_vec, int n_socket) // --OK
{
	int f_login=0,f_logout=0,f_create_group=0,f_leave_group=0,f_accept_request=0,f_join_group=0,f_function_to_file_upload=0;
	int f_download_file=0,f_list_groups=0,f_list_requests=0,f_list_files=0,f_stop_share=0,f_show_downloads=0;
	string to_execute="";
	int flag_pos=0;

	char cc_message_rply[10240];
	//void bzero(void *s, size_t n);
	// The bzero() function erases the data in the n bytes of the memory starting at the location pointed to by s, by writing zeros (bytes containing '\0') to that area.
	bzero(cc_message_rply, 10240);
	read(n_socket, cc_message_rply, 10240);
	if (string(cc_message_rply) == "Invalid Arguments")
	{
		cout << cc_message_rply << endl;
		return 0;
	}

	unordered_map<int,string> mymap;
	mymap[1]="login";
	mymap[2]="logout";
	mymap[3]="create_group";
	mymap[4]="leave_group";
	mymap[5]="accept_request";
	mymap[6]="join_group";
	mymap[7]="upload_file";
	mymap[8]="download_file";
	mymap[9]="list_groups";
	mymap[10]="list_requests";
	mymap[11]="list_files";
	mymap[12]="stop_share";
	mymap[13]="show_downloads";

	for(int i=1;i<=mymap.size();i++)
	{
		if(data_inp_vec[0]==mymap[i])
		{
			to_execute=mymap[i];
			flag_pos=i;
			break;
		}
	}


	switch(flag_pos)
	{
		case 1: { 
					gbl_flag=1;
					if (!(string(cc_message_rply) == "Login Successful"))
						{
							cout << cc_message_rply << endl;
						}
						else
						{
							is_sign_login = 1;
							cout << cc_message_rply << endl;

							string peerAddress = cl_ip + ":" + to_string(cl_port);
							write(n_socket, &peerAddress[0], peerAddress.length());
						}
				}			
						break;

		case 2:	{		
						gbl_flag=0;
						f_logout=1;
				  		is_sign_login = 0;
						cout << cc_message_rply << endl;
				}
						break;

		case 3:			
				{
						f_create_group=1;
						cout << cc_message_rply << endl;
				}
						break;
				
		case 4:
				{
						f_leave_group=1;
						cout << cc_message_rply << endl;
				}
						break;

		case 5:	
				{
						f_accept_request=1;
						cout << cc_message_rply << endl;
				}		
						break;
		case 6:
				{
						f_join_group=1;
						cout << cc_message_rply << endl;
				}
						break;
		case 7: 
				{	
						f_function_to_file_upload=1;
						//cout<<"Hello"<<endl;
						if (string(cc_message_rply) != "Uploading")
						{
							cout << cc_message_rply << endl;
							return 0;
						}
						else
						{
						cout << cc_message_rply << endl;
						return function_to_file_upload(data_inp_vec, n_socket);
						}

				}		

						//break;
		case 8:
				{		
						f_download_file=1;
						if (string(cc_message_rply) != "Downloading")
						{
							cout << cc_message_rply << endl;
							return 0;
						}

						cout << cc_message_rply << endl;
						int tm=-1;
						if (downloads.find(data_inp_vec[2]) != downloads.end())
						{

							tm=0;
							cout << "File already downloaded" << endl;
							tm=-1;
							return 0;
						}
				
						tm=tm+1;
						return function_to_download_file(data_inp_vec, n_socket);

				}		
						// break;

		case 9:
				{
						f_list_groups=1;
						string fname;
						fname="efile";
						vector<string> groups_vec = break_string_using_delimiter(string(cc_message_rply), "*$*");
						fname="";
	
						size_t k=0;
						while(k < groups_vec.size() - 1)
						{
							if(fname=="file")
							break;
							cout << groups_vec[k];
							cout<<endl;
							k=k+1;
						}
				}		
						break;

		case 10:	
				{
						f_list_requests=1;
						vector<string> vec_reqsts = break_string_using_delimiter(string(cc_message_rply), "*$*");
						string strt="";
		
						size_t m=0;
						while(m<vec_reqsts.size()-1)
						{
							strt+=" ";
							cout << vec_reqsts[m] << endl;
							m=m+1;
						}
				}
						break;
		case 11:
				{
						f_list_files=1;
						vector<string> fil_arr = break_string_using_delimiter(string(cc_message_rply), "*$*");

						size_t n=0;
						while(n<fil_arr.size()-1)
						{
							cout << fil_arr[n] << endl;
							n=n+1;
						}
				}		
						break;
		case 12:
					{
						f_stop_share=1;
						cout << cc_message_rply << endl;
						upload_list[data_inp_vec[1]].erase(data_inp_vec[2]);
					}	
						break;
		case 13:

				{
						f_show_downloads=1;
						if(v_flag)
						{
							string pqr;
							cout << cc_message_rply << endl;
							pqr="error";
							for (auto i : downloads)
							{
								if(pqr=="file")
									break;

								cout << "[C] " << i.second << " " << i.first << endl;
							}
						}

				}
						break;
		default: 
				{
						to_execute="";
						cout << cc_message_rply << endl;
				}
						break;


	}

	return 0;

}


void handleconnection(int sock_cl)//PPP - OK
{
	string client_uid = "";

	char data_inp_clin[1024];
	for(int i=0;i<1024;i++)
	{
		data_inp_clin[i]=0;
	}

    // ssize_t read(int fd, void *buf, size_t count);   read() attempts to read up to count bytes from file descriptor fd into the buffer starting at buf.
	
	ssize_t sd=read(sock_cl, data_inp_clin, 1024);
	if (sd<= 0)
	{
		//The close() function shall deallocate the file descriptor indicated by fildes. To deallocate means to make the file descriptor available for return by subsequent calls to open() or other functions that allocate file descriptors
		close(sock_cl);
		return;
	}

	// Now we are breaking string into parts using delimiter to find if it is current_chunk
	vector<string> data_inp_vec = break_string_using_delimiter(string(data_inp_clin), "*$*");


	if (data_inp_vec[0] == "current_chunk")
	{
		int xyz = 0;
		string sent = "";
		// long long int stoll (const string&  str, size_t* idx = 0, int base = 10)
		long long int block_no = stoll(data_inp_vec[2]);
		// This function converts a string, provided as an argument in the function call, to long long int.
		string c_f_p = f_t_p[data_inp_vec[1]];
		
		char *location_of_file = &c_f_p[0];
		xyz=xyz+1;

		std::ifstream ptr_to_file(location_of_file, std::ios::in | std::ios::binary);
		xyz=-1;
		
		for(int i=xyz;i>0;i--)
		{
			// decrement each iteration
		}
		xyz=0;
		//istream&seekg(streamoff offset, ios_base::seekdir dir); dir:==> ios_base::beg: offset from the beginning of the stream’s buffer
		ptr_to_file.seekg(block_no * block_capacity, ptr_to_file.beg);
		char buff_size[block_capacity] = {0};
		ptr_to_file.read(buff_size, sizeof(buff_size));
		//The std::basic_istream::gcount() is used to count the characters in the given string.  It returns the number of characters extracted by the last unformatted in_data operation. The unformatted in_data operation is returned by these function: get(), getline(), ignore(), peek(), read(), etc.
		int cnt = ptr_to_file.gcount();

		//ssize_t send(int sockfd, const void *buf, size_t len, int flags); The system calls send(), are used to transmit a gsm_mssg to another socket. The send() call may be used only when the socket is in a connected state 
		xyz = send(sock_cl, buff_size, cnt, 0);
		int fail=-1;
		if (xyz == fail)
		{
			// printf("Error in sending file\n");
			cout<<"Error in sending file"<<endl;
			exit(1);
		}

		ptr_to_file.close();
	}
	// if (data_inp_vec[0] == "current_chunk_vector_details") pos_sw=2;
	// if (data_inp_vec[0] == "current_path_file") pos_sw=3;
	else if (data_inp_vec[0] == "current_chunk_vector_details")
	{
		//cout << "function called curr_vec_de";cout<<endl;
		string name_of_file = data_inp_vec[1];
		string gsm_mssg = "";
		vector<int> block = block_info[name_of_file];
		// int pos_sw=0;
		// if(data_inp_vec[0] == "current_chunk") 	pos_sw=1;


		for (int i : block)
			gsm_mssg += to_string(i);


		//cout << gsm_mssg << endl;
		char *t = &gsm_mssg[0];
		//The send() function shall initiate transmission of a message from the specified socket to its peer. The send() function shall send a message only when the socket is connected
		send(sock_cl, t, strlen(t), 0);
	}
	else if (data_inp_vec[0] == "current_path_file")
	{
		string c_f_p = f_t_p[data_inp_vec[1]];
		//ssize_t write(int fd, const void *buf, size_t count); write() writes up to count bytes from the buffer starting at buf to the file referred to by the file descriptor fd.
		write(sock_cl, &c_f_p[0], c_f_p.length());
	}

	close(sock_cl);
	return;
}


void *server_func(void *arg) //-->OK
{
	string fname;
	int n_skt; // socketid
	struct sockaddr_in client_ser_address;
	int options = 1;
	int len_of_address = sizeof(client_ser_address);

    //The socket() call creates a socket in the specified domain and of the specified type s = socket(domain, type, protocol);
    n_skt = socket(AF_INET, SOCK_STREAM, 0);
	if (n_skt==0)
	{
		cout<<"socket creation falied"<<endl;
		exit(EXIT_FAILURE);
	}

	//cout <<"client/peer server socket created"<<endl;
	fname="";

    //int setsockopt(int socket, int level, int option_name,const void *option_value, socklen_t option_len); The setsockopt() function shall set the option specified by the option_name argument, at the protocol level specified by the level argument, to the value pointed to by the option_value argument for the socket associated with the file descriptor specified by the socket argument.
    int x=setsockopt(n_skt, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &options, sizeof(options));
    // SO_REUSEADDR  Specifies that the rules used in validating addresses supplied to bind() should allow reuse of local addresses,if this is supported by the protocol. This option takes an int value. This is a Boolean option
    if (x)
	{ // -1 is error
		if(fname=="error")
		{
			exit(EXIT_FAILURE);
		}
        cout<<"setsockopt of client/peer server"<<endl;
		exit(EXIT_FAILURE);
	}
    
    //    struct sockaddr_in {        sa_family_t    sin_family; /* address family: AF_INET */ in_port_t      sin_port;   /* port in network byte order */struct in_addr sin_addr;   /* internet address */ };

    //sin_port contains the port in network byte order
    client_ser_address.sin_port = htons(cl_port);
    //sin_family is always set to AF_INET.
	client_ser_address.sin_family = AF_INET;
	
    fname+="\n";
    //int inet_pton(int af, const char *restrict src, void *restrict dst);  .  This function converts the character string src into a network address structure in the af address family, then copies the network address structure to dst.  The af argument must be either . AF_INET or AF_INET6.  dst is written in network byte order.
    int y=inet_pton(AF_INET, &cl_ip[0], &client_ser_address.sin_addr);
    if (y<= 0)
	{
		cout<<endl;
        cout<<"Invalid address/ Address not supported "<<endl;
		//printf(" \n");
		exit(EXIT_FAILURE);
	}

    string str;
    //    int bind(int sockfd, const struct sockaddr *addr, socklen_t len_of_address);   When a socket is created with socket(2), it exists in a name space (address family) but has no address assigned to it.  bind() assigns the address specified by addr to the socket referred to by the file descriptor sockfd.  len_of_address specifies the size, in bytes, of the address structure pointed to by addr. Traditionally, this operation is called “assigning a name to a socket”
    int z=bind(n_skt, (struct sockaddr *)&client_ser_address, sizeof(client_ser_address));
	if (z< 0)
	{
		str+="bind";
        cout<<"Binding failed"<<endl;
		exit(EXIT_FAILURE);
	}

	// printf("Binding completed.\n");
    cout<<"Binding completed"<<endl;
    if(str=="binding done")
		{
            //printf("Done");
            cout<<"Done";
        }

    //int listen(int sockfd, int backlog); listen() marks the socket referred to by sockfd as a passive socket, that is, as a socket that will be used to accept incoming connection requests using accept(2). The sockfd argument is a file descriptor that refers to a socket of type SOCK_STREAM or SOCK_SEQPACKET. The backlog argument defines the maximum length to which the queue of pending connections for sockfd may grow.  If a connection request arrives when the queue is full, the client may receive an error 
    int lt=listen(n_skt, 3);
	if (lt< 0)
	{ 
        str+="listen";
        cout<<"listen failed"<<endl;
		exit(EXIT_FAILURE);
	}

    cout<<"PeerServer Listening for clients"<<endl;

	if(str=="binding done")
    {
        cout<<"Done";
    }
	
    int sock_cl;
	vector<thread> threads;
        
    // int accept(int sockfd, struct sockaddr *restrict addr,socklen_t *restrict addrlen); .It extracts the first connection request on queue of pending connections for the listening socket, sockfd, creates a new connected socket, and returns a new file descriptor referring to that socket.
    while (sock_cl = accept(n_skt, (struct sockaddr *)&client_ser_address, (socklen_t *)&len_of_address))
	{
        if(str=="exit")
		 return NULL;

        //cout<<"Connection of client Accepted"<<endl;
		if(str=="exit")
        {
            cout<<"Done";
        }
		 
		threads.push_back(thread(handleconnection, sock_cl));
	}

	//to see if all threads are executed
	for (auto i = threads.begin(); i != threads.end(); i++)
	{
		str+="join";

		if(!(i->joinable()))
			if(str=="exit")
				break;
        else
        {
            str;
			i->join();
        }
	}

	close(n_skt);
	return NULL;
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
        res_tracker.push_back("0.0.0.0");
        res_tracker.push_back("0");
		return res_tracker;
	}

}



int main(int argument_count,char *argument_list[]) //ppp  --OK 
{

	int valid_arguments=3;
	if(argument_count==valid_arguments)
	{

        vector<string> ip_port;
        // argument[1] contains the listening port and ip of  client
        string arg_1=argument_list[1];
        // fetching port and _ip of client
        ip_port=fetch_ip_port(arg_1,":");
        // assignimg the port and ip of client to c_port(int), c_ip(string)
        cl_ip=ip_port[0];
        cl_port=stoi(ip_port[1]);
        //cout<<c_ip<<"    "<<c_port<<endl;

        // argument 2 contains the tracker file name
        vector<string> t_ip_port=fetch_ip_port_tracker(argument_list[2]);
        // assign tracket listening port and ip
        t_ip=t_ip_port[0];
        t_port=stoi(t_ip_port[1]);
        //cout<<t_ip<<"    "<<t_port<<endl;



        int n_socket=0;
        thread t; // to do
        string strt;

        n_socket = socket(AF_INET, SOCK_STREAM, 0);

    	if (n_socket < 0)
	    {  
		    strt="socket";
		    cout<<endl;
		    cout<<"client/peer Socket creation error"<<endl;
		    return -1;
	    }
        strt="";
	    cout<<"client/peer socket creation successful"<<endl;
	    cout<<endl;


        // server / peer socket creation
        struct sockaddr_in serv_addr;
        pthread_t thread_server;
        long long status=pthread_create(&thread_server, NULL, server_func, NULL);

	    if(status== -1)
	    {
		    strt="perror";
		    cout<<"pthread error"<<endl;
		    cout<<endl;

		    return -1;
	    }

		// to do

		serv_addr.sin_family = AF_INET;
		if(strt=="exit")
		return 0;
		//The htons function takes a 16-bit number in host byte order  and returns a 16-bit number in network byte order used in TCP/IP networks (the AF_INET or AF_INET6 address family)
		serv_addr.sin_port = htons(t_port);

		// int inet_pton(int af, const char *restrict src, void *restrict dst);
		int r=inet_pton(AF_INET, &t_ip[0], &serv_addr.sin_addr);
		if (r<= 0)
		{
			strt;
			cout<<endl<<"Invalid address/ Address not supported ";
			cout<<endl;
			return -1;
		}

		// int connect(int sockfd, const struct sockaddr *addr,socklen_t addrlen); The connect() system call connects the socket referred to by the file descriptor sockfd to the address specified by addr. The addrlen argument specifies the size of addr
		int f=connect(n_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
		if (f< 0)
		{
			cout<<"Connection failed"<<endl;
			return -1;
		}

		while (1) //PPP
		{
			string in_data, present;
			//istream& getline(istream& is, string& str, char delim);
			getline(cin, in_data);
			//The getline() function extracts characters from the in_data stream and appends it to the string object until the delimiting character is encountered.
			vector<string> data_inp_vec;

			if (in_data.length() <= 0)
				continue;

			stringstream vec_str_ary(in_data);

			// while there is data in vec_str_ary , we are appensing data into present string
			while (vec_str_ary >> present)
				data_inp_vec.push_back(present);

			int fl1=0,fl2=0;
			if(data_inp_vec[0] == "login")
			{
				fl1=1;
				if(is_sign_login == 1 && fl1==1)
				{
					fl2=1;
					cout << "You already have one active session" << endl;
					continue;
				}
			}

			if(data_inp_vec[0]=="create_user" && gbl_flag==1)
			{
				cout<<"You are not allowed to create user"<<endl;
				continue;
			}

			if(data_inp_vec[0] != "login")
			{
				if(data_inp_vec[0] != "create_user")
				{
					if(is_sign_login == 0)
					{
						strt="login";
						cout << "Login first (or) create an account"<<endl;
						continue;
					}
				}

			}

			//MSG_NOSIGNAL:Requests not to send SIGPIPE on errors on stream oriented sockets when the other end  breaks the connection.  The EPIPE error is still returned  . ssize_t send(int sockfd, const void *buf, size_t len, int flags);
			int dg=send(n_socket, &in_data[0], strlen(&in_data[0]), MSG_NOSIGNAL);
			if (dg==-1)
			{
				strt="error";
				cout<<"Error from server"<<endl;
				return -1;
			}

			establish_conn(data_inp_vec, n_socket);
		}
		close(n_socket);


	}
	return -1;

}
