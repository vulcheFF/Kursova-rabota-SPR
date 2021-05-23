#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>

#define PORT 8080

typedef enum vid {
    TOK,
    VODA,
    INTERNET,
    TELEVIZIQ,
    HRANA,
    KOLA,
    DANUCI,
    PARNO,
    DECA,
    NAEM
} vid;

typedef struct Costs {
	vid vid;
	float suma;
	int d,m,y;
	
    
    
}Costs;

int main(int argc, char* argv[])
{
	int sock_1, v_read;
	struct sockaddr_in address;
	
	if((sock_1 = socket(AF_INET, SOCK_STREAM, 0)) < 0){
	
		perror("Socket creation failure!!!");
		exit(0);
		
	}
		
	address.sin_family = AF_INET;	
	address.sin_port = htons(PORT);
	
	
	if(inet_pton(AF_INET, "127.0.0.1", &address.sin_addr) <= 0){
		printf("Invalid IP address");
		exit(1);
		
	}
	
	if(connect(sock_1, (struct sockaddr *)&address, sizeof(address)) < 0){
		printf("Connection failure");
		exit(2);
	
	}
	
	Costs costs;
	Costs c;
	Costs s;
	Costs snd;
	int month=0,year=0,count=0,countC=0;
	float sum=0,max=0;
	costs.vid=10;
	costs.suma=-1;
	costs.d=0;
	costs.m=0;
	costs.y=0;
	
	
	while(1){
	
	printf("\n1.Create expense.\n2.Monthly expense\n3.Biggest expense ever\n4.All expenses\n5.Exit\nChoose:");
	
	int choice;
	scanf("%d",&choice);
	
	
	if((send(sock_1,&choice, sizeof(int), 0))<0){
	perror("Error sending data\n");
	exit(6);
	
	}
	 
	
	
	switch(choice){
			
			case 1:
				//Get input from user
				printf("\nType of expenses:\n0.Electricity,\n1.Water,\n2.Internet,\n3.TV,\n4.Food,\n5.Car,\n6.Taxes,\n7.Heating,\n8.Children,\n9.Rent ");	
								
				while(1){
				printf("\nEnter type of expense: ");				    
				scanf("%u", &(costs.vid));
				if(costs.vid  >= 0 && costs.vid <=9 )
				{break;}
				 } 
				
				while(1){
				printf("\nEnter amount of expense:");
				scanf("%f",&(costs.suma));
				if(costs.suma >= 0 )
				{break;}
				} 
				
				while(1){
				printf("\nEnter day: ");				    
				scanf("%d", &(costs.d));
				if(costs.d  >= 1 && costs.d  <=31  )
				{break;}
				}
				
				while(1){
				printf("\nEnter month: ");				    
				scanf("%d", &(costs.m));
				if(costs.m <=12 && costs.m >= 1 )
				{break;}
				}
				
				while(1){
				printf("\nEnter year: ");				    
				scanf("%d", &(costs.y));
				if(costs.y >=2019 && costs.y <=2022)
				{break;}
				}
				   
				//Send data to the server   
				if((send(sock_1,&costs, sizeof(Costs), 0))<0){
					perror("Error sending data\n");
					exit(3);
				}else{
				
					printf("You've entered - type of expense:%u, amount of expense:%.2f, Data:%d/%d/%d\n",costs.vid,costs.suma,costs.d,costs.m,costs.y);
					
				}
				
				
				break;
			case 2:
				//Get month and year from user
				while(1){
				printf("Enter month:\n");				    
				scanf("%d", &month);	
				if(month <=12 && month>= 1){break;}			
				}
				
				while(1){
				printf("Enter year:\n");				    
				scanf("%d", &year);
				if(year >= 2019 && year <=2022){break;}				
				}
				
				//Sending data to server				
				if((send(sock_1,&month, sizeof(int), 0))<0){
					perror("Error sending data\n");
					exit(4);
				}
				if((send(sock_1,&year, sizeof(int), 0))<0){
					perror("Error sending data\n");
					exit(5);
				}
				
				
				//Read data from server
				if((v_read = read(sock_1,&sum,sizeof(float))) < 0){ 
				perror("Error reading");
				exit(6);
				}
				printf("Monthly expense for month: %d and year: %d is:%.2f lv\n",month,year,sum);
				
				break;
			case 3:
				
				//Read data from server
				
				
				if((v_read = read(sock_1,&s,sizeof(Costs))) <0)
					{
					perror("Error reading");
					exit(9);	
					}
				
				printf("Biggest expense ever made is type:%u amount:%.2flv on date:%d/%d/%d\n",s.vid,s.suma,s.d,s.m,s.y);
				
				
				break;
				
			case 4:
				//Read data from server
				if((v_read = read(sock_1,&count,sizeof(int))) < 0){
					perror("Error reading");
					exit(8);
					}
				
				printf("Count of expenses:%d\n",count);
				while(1){
					if((v_read = read(sock_1,&c,sizeof(Costs))) <0)
					{
					perror("Error reading");
					exit(9);	
					}
					
						
					printf("Montly expense type:%u Amount:%.2flv. Date:%d/%d/%d\n",c.vid,c.suma,c.d,c.m,c.y);
					countC++;
					if(countC == count){
					break;
					}
					
					
					
				}
				countC=0;
				
				
				break;
			case 5:
				printf("Close!\n");
				close(sock_1);
				break;
			
		
	
	
		}
		if(choice == 5) {break;}
		
	}
	
	
	
	
}

