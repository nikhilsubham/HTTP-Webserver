#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <memory.h>
#include <errno.h>
#include "sll/sll1.h"

/*Server process is running on this port no. Client has to send data to this port no*/
#define SERVER_PORT     2000
#define MAX_CLIENT_SUPPORTED    32

char data_buffer[1024];
char data_buffer1[1024];

typedef struct student_{
    char name[32];
    unsigned int roll_no;
    char hobby[32];
    char dept[32];
} student_t; 

int monitored_fd_set[MAX_CLIENT_SUPPORTED];


static void intitiaze_monitor_fd_set(){

    int i = 0;
    for(; i < MAX_CLIENT_SUPPORTED; i++)
        {
          monitored_fd_set[i] = -1;
        }
}


static void add_to_monitored_fd_set(int skt_fd){

    int i = 0;
    for(; i < MAX_CLIENT_SUPPORTED; i++){

        if(monitored_fd_set[i] != -1)
            continue;
        monitored_fd_set[i] = skt_fd;
        break;
    }
}


static void
remove_from_monitored_fd_set(int skt_fd){

    int i = 0;
    for(; i < MAX_CLIENT_SUPPORTED; i++){

        if(monitored_fd_set[i] != skt_fd)
            continue;

        monitored_fd_set[i] = -1;
        break;
    }
}


static void
refresh_fd_set(fd_set *fd_set_ptr){

    FD_ZERO(fd_set_ptr);
    int i = 0;
    for(; i < MAX_CLIENT_SUPPORTED; i++){
        if(monitored_fd_set[i] != -1){
            FD_SET(monitored_fd_set[i], fd_set_ptr);
        }
    }
}


static int
get_max_fd(){

    int i = 0;
    int max = -1;

    for(; i < MAX_CLIENT_SUPPORTED; i++){
        if(monitored_fd_set[i] > max)
            max = monitored_fd_set[i];
    }
    return max;
}




student_t* find_person_db(dll_t* person_db, int roll_no) 
{
    if(!person_db || !person_db->head) return;
    
    struct Node *node = person_db->head;
    student_t *data = NULL;
   
     while (node != NULL) { 
        data = node->data;
        if(data ->roll_no == roll_no)
            return data;
        node = node->next; 
    } 
    return NULL;
}


/*string helping functions*/
/*Remove the space from both sides of the string*/
void
string_space_trim(char *string){

    if(!string)
        return;

    char* ptr = string;
    int len = strlen(ptr);

    if(!len){
        return;
    }

    if(!isspace(ptr[0]) && !isspace(ptr[len-1])){
        return;
    }

    while(len-1 > 0 && isspace(ptr[len-1])){
        ptr[--len] = 0;
    }

    while(*ptr && isspace(*ptr)){
        ++ptr, --len;
    }

    memmove(string, ptr, len + 1);
}



static char *
process_GET_request(char *URL, unsigned int *response_len, dll_t *person_db){

    printf("%s(%u) called with URL = %s\n", __FUNCTION__, __LINE__, URL);
    
    /*Let us extract the roll no of a students from URL using 
     * string handling 
     *URL : /College/IIT/?dept=CSE&rollno=10305042/
     * */
    char delimeter[2] = {'?', '\0'};

    string_space_trim(URL);
    char *token[5] = {0};

    token[0] = strtok(URL, delimeter);
    token[1] = strtok(0, delimeter);
    /*token[1] = dept=CSE&rollno=10305042*/
    delimeter[0] = '&';

    token[2] = strtok(token[1], delimeter);
    token[3] = strtok(0, delimeter);
    /*token[2] = dept=CSE, token[3] = rollno=10305042*/

    printf("token[0] = %s, token[1] = %s, token[2] = %s, token[3] = %s\n",
        token[0] , token[1], token[2], token[3]);

    delimeter[0] = '=';
    char *roll_no_str = strtok(token[3], delimeter);
    char *roll_no_value = strtok(0, delimeter);
    printf("roll_no_value = %s\n", roll_no_value);
    unsigned int roll_no = atoi(roll_no_value), i = 0;

   
    student_t* student = NULL;
    char roll[10];
    student= find_person_db(person_db, roll_no); 
    sprintf(roll, "%d", student->roll_no);
    
    if(i == 5)
        return NULL;
    
    /*We have got the students of interest here*/
    char *response = calloc(1, 1024);

    strcpy(response,
        "<html>"
        "<head>"
            "<title>HTML Response</title>"
            "<style>"
            "table, th, td {"
                "border: 1px solid black;}"
             "</style>"
        "</head>"
        "<body>"
        "<table>"
        "<tr>"
        "<td>");

        strcat(response , 
            student->name
        );

        strcat(response ,
            "</td></tr>");

        strcat(response ,
            "<tr><td>");

        strcat(response , 
            student->hobby
        );

        strcat(response ,
            "</td></tr>");
    
        strcat(response ,
            "<tr><td>");

        strcat(response , 
            student->dept
        );

        strcat(response ,
            "</td></tr>");

        strcat(response ,
            "<tr><td>");
        strcat(response , 
            roll
        );
           
        
        strcat(response ,
            "</td></tr>");
        strcat(response , 
                " <form method = \"post\">"
                   "name: <input type=\"text\" name=\"fname\"><br>"
                   "hobby: <input type=\"text\" name=\"fhobby\"><br>"
                   "department: <input type=\"text\" name=\"fdept\"><br>"
                   "roll_no: <input type=\"number\" name=\"froll_no\"><br>"
                   "<input type=\"submit\" value=\"Submit\"> "             
	         " </form> "
                "</table>"
                "</body>"
                "</html>");

    unsigned int content_len_str = strlen(response);

    /*create HTML hdr returned by server*/
    char *header  = calloc(1, 248 + content_len_str);
    strcpy(header, "HTTP/1.1 200 OK\n");      
    strcat(header, "Server: My Personal HTTP Server\n"    );
    strcat(header, "Content-Length: "  ); 
    strcat(header, "Connection: close\n"   );
    //strcat(header, itoa(content_len_str)); 
    strcat(header, "2048");
    strcat(header, "\n");
    strcat(header, "Content-Type: text/html; charset=UTF-8\n");
    strcat(header, "\n");

    strcat(header, response);
    content_len_str = strlen(header); 
    *response_len = content_len_str;
    free(response);
    return header;
}

static char *
process_POST_request(char *URL, unsigned int *response_len, char* data_buffer,dll_t *person_db){
    printf("******************************************************************\n");
    char *request_line = NULL;
    char *request_line1 = NULL;
    char del[2] = "\n";
    char* token[8] = {0};
    int i =0;

    request_line = strtok(data_buffer, del);

    while(request_line != NULL)
     {
       request_line1 = request_line;
       request_line = strtok(NULL, del);
     }
    printf("%s\n", request_line1);
    char bel[2] = {'=', '&'};
   
    request_line = strtok(request_line1, bel);
    

    while(request_line != NULL)
    {
      
      token[i] = request_line;
      request_line = strtok(NULL, bel);
      i++;
    }

    if(token[7] == NULL)
      return NULL;

    student_t *student3 = calloc(1, sizeof(student_t));
    strcpy(student3->name, token[1]);
    strcpy(student3->hobby, token[3]);
    strcpy(student3->dept, token[5]);
    int a = atoi(token[7]);
    student3->roll_no = a;
    At_front(&person_db->head, student3);
    
    return NULL;
}

void
setup_tcp_server_communication(dll_t *person_db){

    
    int master_sock_tcp_fd = 0, 
        sent_recv_bytes = 0, 
        addr_len = 0, 
        opt = 1;

    int comm_socket_fd = 0;   
    int data_socket;  
    fd_set readfds; 

    intitiaze_monitor_fd_set();            
    
    struct sockaddr_in server_addr, 
                       client_addr;

   
    if ((master_sock_tcp_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP )) == -1)
    {
        printf("socket creation failed\n");
        exit(1);
    }
    
    if (setsockopt(master_sock_tcp_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt))<0){
        printf("TCP socket creation failed for multiple connections\n");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    
    server_addr.sin_addr.s_addr = INADDR_ANY; 
    addr_len = sizeof(struct sockaddr);

    if (bind(master_sock_tcp_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
    {
        printf("socket bind failed\n");
        return;
    }

    
    if (listen(master_sock_tcp_fd, 5)<0)  
    {
        printf("listen failed\n");
        return;
    }

    add_to_monitored_fd_set(master_sock_tcp_fd);
    
    while(1){

        //FD_ZERO(&readfds);                     
        //FD_SET(master_sock_tcp_fd, &readfds); 
        refresh_fd_set(&readfds);  

        printf("blocked on select System call...\n");

        
        select(get_max_fd() + 1, &readfds, NULL, NULL, NULL);
       

        if (FD_ISSET(master_sock_tcp_fd, &readfds))
        { 
            printf("New connection recieved recvd,Client and Server completes TCP-3 way handshake at this point\n");

            comm_socket_fd = accept(master_sock_tcp_fd, (struct sockaddr *)&client_addr, &addr_len);
            if(comm_socket_fd < 0){
                printf("accept error : errno = %d\n", errno);
                exit(0);
            }

            printf("Connection accepted from client : %s:%u\n", 
                inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

            add_to_monitored_fd_set(comm_socket_fd);
         }

         
         else
         {
               int i = 0, comm_socket_fd = -1; 
               for(; i < MAX_CLIENT_SUPPORTED; i++){

                   if(FD_ISSET(monitored_fd_set[i], &readfds)){
                      comm_socket_fd = monitored_fd_set[i];
                   
                   printf("Server ready to service client msgs.\n");
                   memset(data_buffer, 0, sizeof(data_buffer));

                   sent_recv_bytes = recvfrom(comm_socket_fd, (char *)data_buffer, sizeof(data_buffer), 0,
                        (struct sockaddr *)&client_addr, &addr_len);

                   printf("Server recvd %d bytes from client %s:%u\n", sent_recv_bytes, 
                        inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

                   if(sent_recv_bytes == 0){
                     close(comm_socket_fd);
                     remove_from_monitored_fd_set(comm_socket_fd);
                     break; 
                    }

                   
                   /*BEGIN : Implement the HTTP request processing functionality */
                   memmove(data_buffer1, data_buffer, 1024);
                   printf("%s\n",data_buffer1);
                   char *request_line = NULL;
                   char del[2] = "\n", 
                        *method = NULL,
                        *URL = NULL;
                   request_line = strtok(data_buffer, del); /*Extract out the request line*/
                   del[0] = ' ';
                   method = strtok(request_line, del); 

                   URL = strtok(NULL, del);                /*Extract the URL*/
                   printf("Method = %s\n", method);
                   printf("URL = %s\n", URL);
                   char *response = NULL;
                   unsigned int response_length = 0 ; 
 

                   if(strncmp(method, "GET", strlen("GET")) == 0){
                    response = process_GET_request(URL, &response_length,person_db);
                   }

                   else if(strncmp(method, "POST", strlen("POST")) == 0){
                    response = process_POST_request(URL, &response_length, data_buffer1,person_db);
                   }
    
                   else{
                    printf("Unsupported URL method request\n");
                    close(comm_socket_fd);
                    remove_from_monitored_fd_set(comm_socket_fd);
                    break;
                   }

                  
                  /* Server replying back to client now*/
                  if(response){
                     printf("response to be sent to client = \n%s", response);
                     sent_recv_bytes = sendto(comm_socket_fd, response, response_length, 0,
                            (struct sockaddr *)&client_addr, sizeof(struct sockaddr));
                    free(response);
                    printf("Server sent %d bytes in reply to client\n", sent_recv_bytes);
                    //close(comm_socket_fd);
                    //break;
                    }

              }
            }
          }
       }
  }
       

int
main(int argc, char **argv){

    student_t *student1 = calloc(1, sizeof(student_t));
    strncpy(student1->name, "Nikhil", strlen("Nikhil"));
    strncpy(student1->hobby, "Programming", strlen("Programming"));
    strncpy(student1->dept, "CSE", strlen("CSE"));
    student1->roll_no = 10305042;

student_t *student2 = calloc(1, sizeof(student_t));
    strncpy(student2->name, "Nitin", strlen("Nitin"));
    strncpy(student2->hobby, "Programming", strlen("Programming"));
    strncpy(student2->dept, "CSE", strlen("CSE"));
    student2->roll_no = 10305048;

student_t *student3 = calloc(1, sizeof(student_t));
    strncpy(student3->name, "Avinash", strlen("Avinash"));
    strncpy(student3->hobby, "Cricket", strlen("Cricket"));
    strncpy(student3->dept, "ECE", strlen("ECE"));
    student3->roll_no = 10305041;

student_t *student4 = calloc(1, sizeof(student_t));
    strncpy(student4->name, "Jack", strlen("Jack"));
    strncpy(student4->hobby, "Cricket", strlen("Cricket"));
    strncpy(student4->dept, "ECE", strlen("ECE"));
    student4->roll_no = 10305032;

dll_t *person_db = get_new_dll();
    At_front(&person_db->head, student1);
    At_front(&person_db->head, student2);
    At_front(&person_db->head, student3);
    At_front(&person_db->head, student4);

    setup_tcp_server_communication(person_db);
    return 0;
}
