# P2PBitTorrent
# Assignment_3 - Peer-to-Peer Group Based File Sharing System

# _ Commands _


- Compiling the tracker (inside tracker folder)
-----------------------------------------------
    g++ tracker.cpp -o tracker -pthread


- Compiling the client   (inside client folder)
-----------------------------------------------
    g++ client.cpp -o client -pthread -lcrypto


- To run the tracker (inside tracker folder)
-----------------------------------------------
    ./tracker tracker_ip_port.txt


- To run the client (inside client folder)
-----------------------------------------------
    ./client IP:PORT tracker_ip_port.txt


# tracker_ip_port.txt
        - contains the IP and port of the tracker(s)

# Commands for operations

1. Create User Account: create_user <user_id> <passwd>

2. Login: login <user_id> <passwd>

3. Create Group: create_group <group_id>

4. Join Group: join_group <group_id>

5. Leave Group: leave_group <group_id>

6. List pending join request: list_requests <group_id>

7. Accept Group Joining Request: accept_request <group_id> <user_id>

8. List All Group In Network: list_groups

9. List All sharable Files In Group: list_files <group_id>

10. Upload File: upload_file <file_path> <group_id>

11. Download File: download_file <group_id> <file_name> <destination_path>

12. Logout: logout

13. Show_downloads: show_downloads

14. stop_share <group_id> <filename>

