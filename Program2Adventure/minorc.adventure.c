#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>

int main(void)
{	
	int i=1, cx,idx;
	srand ( time(NULL) );
//Array of room names which will later be randomly assigned to rooms
	 char * roomsArray[] = {
	"Dragon's Den",
    	"Maid's Quarters",
    	"Queen's Bedroom",
	"Pirate's Fortress",
	"Room of Enlightenment",
	"Skeleton Room",
	"Garden of Knowledge",
	"Rat City Tunnel",
	"Temple of Truth",
	"Map Room"
	};
//Make a copy of this array (to test if file exists later)
	char ** roomsCopy;
	int size = sizeof(roomsArray);
	roomsCopy = malloc(size);//Allocate memory block the size of the original array for roomsCopy.
	memcpy(roomsCopy, roomsArray, sizeof(roomsArray));

//get the system to call mkdir to make directory
	char directory[35];
	cx = snprintf ( directory, 35, "minorc.rooms.%d", getpid());
	char makeDir[35];	
	cx = snprintf ( makeDir, 35, "mkdir %s", directory);
	system(makeDir);
//get permission to access it
	char getPerm[35];
	cx = snprintf ( getPerm, 35, "chmod 700 %s",directory);
	system(getPerm);
//switch working directory
	chdir (directory);

//Randomly choose 7 of the 10 rooms. For 7 loops, the index of a room in
//roomsArray[10] is chosen by rand(), that room is added to chosenRoomsArray, 
//made into a file, and removed from roomsArray.
//During each iteration, the  "removal"  is done by copying
//all elements that are above roomsArray[randNum] into the element
//below them, so the chosen room is gone and
//there is another (NULL) at the top of roomsArray. 
	int roomsLeft = 10, randNum, loopCount;
	FILE *fp;
	char buffer[512];
	char * chosenNamesArray[7];
	for (idx = 0; idx < 7; idx++)
	{	
//Randomly choose room name
		randNum = rand()%roomsLeft;
		chosenNamesArray[idx] = strdup(roomsArray[randNum]);
		cx = snprintf(buffer, 35, "%s", roomsArray[randNum]);
		fp = fopen(buffer, "w");
		fprintf(fp, "ROOM NAME: %s", roomsArray[randNum]);
//loop to remove the room from roomsArray
		loopCount = (roomsLeft - randNum);
		for(i = 0; i < loopCount; i++)
		{
			roomsArray[randNum] = '\0';
			roomsArray[randNum] = roomsArray[randNum + 1];
			randNum++;
		}
		fclose(fp);
	roomsLeft--;
	}//End of for loop that names rooms.	

	int file_descriptor;
	ssize_t nread, nwritten;
	idx = 0;
	char someRoom[35];//Holds name of room that will have at least 3 connections 
	//by the time the while loop is done.
	char otherRoom[35];//Holds name of each other room that someRoom may connect to.
	int connectionNums[7] = { 0 }; //Array to keep track of number of connections
	int isConnected[8][7] = { {0} }; //Array to keep track of weather or not two
	//rooms are connected. The first row represents the positions of the 7 rooms 
	//in chosenNamesArray, and the 7 below represent the same thing vertically, 
	//with a 1 instead of a 0 indicating that the two rooms are connected. 
	int OFI, IFI, hasRemainder;//OFI = index were "someRoom" is named in 
	//chosenNamesArray[7]. IFI =  index where "otherRoom" is.
//Begin loop to add connections to each room.
for(OFI= 0; OFI < 7; OFI++){
	while(connectionNums[OFI] < 3){
		for(IFI = 0; IFI < 7; IFI++){
		//get name of file to be written to into string outerName
			cx = snprintf(someRoom, 35, "%s", chosenNamesArray[OFI]);
		//get name of file to be test that these are 2 different rooms
			cx = snprintf(otherRoom, 35, "%s", chosenNamesArray[IFI]);
			if (strcmp(someRoom,otherRoom) != 0){//If not same room
				if(isConnected[OFI][IFI + 1] == 0){//If rooms not connected
			//rolling the dice to decide whether to add connection to current IFI
					randNum = rand()%10;
					hasRemainder = (randNum % 2);
					if(hasRemainder != 0){
						//increment the array variable that holds the number of connections for "some room":
						connectionNums[OFI] = (connectionNums[OFI] + 1);
						//increment the array variable that holds the number of connections for "other room":
						connectionNums[IFI] = (connectionNums[IFI] + 1);
						//Note in OFI's column in isConnected that it is connected to IFI:
						isConnected[OFI][IFI + 1] = 1;
						//Note in IFI's column in isConnected that it is connected to OFI:
						isConnected[IFI][OFI + 1] = 1;
						file_descriptor = open(someRoom, O_RDWR);
						//add otherRoom to first file
						memset(buffer,0,strlen(buffer));
						cx = snprintf(buffer, strlen(otherRoom) + 17, "\nCONNECTION %d: %s", connectionNums[OFI], otherRoom);
						lseek(file_descriptor, 0, SEEK_END);
						nwritten = write(file_descriptor, buffer, strlen(buffer));
						close(file_descriptor);
						//add outerName to second file
						file_descriptor = open(otherRoom, O_RDWR);
						memset(buffer,0,strlen(buffer));
						cx = snprintf(buffer, strlen(someRoom) + 17, "\nCONNECTION %d: %s", connectionNums[IFI], someRoom);
						lseek(file_descriptor, 0, SEEK_END);
						nwritten = write(file_descriptor, buffer, strlen(buffer));
						close(file_descriptor);
						
					}//End of if(hasRemainder)
				}//End of if (is not connected)
			}//end of if (not looking at own room)	
		}//end of for loop to look at all other files
	}//End of while loop that ensures no. of connections is > 3.
}//End of for loop to roll the dice for what connections to add to each particular room.

//Now we randomly choose a start and end room

int uniqueStartEnd = 0, randNum2; //randNum hold index of start room,
//randNum2 holds index of end room.
while(uniqueStartEnd == 0){//is changed to 1 when start and end rooms are unique
	//choose start room
	randNum = rand()% 7;
	//choose end room
	randNum2 = rand()%7;
	if(randNum != randNum2){ 
		uniqueStartEnd++;
	}
}//End of chooose start and end 

//Set the room types
	cx = snprintf(buffer, 35, "%s", chosenNamesArray[randNum]);
	fp = fopen(buffer, "ab+"); //ab+ means read and write to end of file
	fprintf(fp, "\n""ROOM TYPE: START_ROOM");
	fclose(fp);
	cx = snprintf(buffer, 35, "%s", chosenNamesArray[randNum2]);
	fp = fopen(buffer, "ab+");
	fprintf(fp,"\n""ROOM TYPE: END_ROOM");
	fclose(fp);	
	for(i = 0; i < 7; i++) {
	if(i != randNum && i != randNum2){ //if not looking at stard and end room...

		cx = snprintf(buffer, 35, "%s", chosenNamesArray[i]);
		fp = fopen(buffer, "ab+"); 
		fprintf(fp, "\n""ROOM TYPE: MID_ROOM");
		fclose(fp);
		}
	}
	
//Now the game begins
	char * choicesArray[7];//Array to hold room connections
	char * pathArray[50];//Array to hold rooms user has visited
	int connectionIndex, pathIndex = 0, lastRoom = 0;
	//connectionIndex = idx for choicesArray
	//pathIndex = idx for pathArray
	//lastRoom = bool to determine when the last room has been reached
	char nextRoom[35];//Next room to enter
	char typedRoom[35];//Room typed in by user, but must be tested for validity
//Find the start room
for(i = 0; i < 7; i++){
	cx = snprintf(nextRoom, 35, chosenNamesArray[i]);
	file_descriptor = open(nextRoom, O_RDWR);
	nread = read (file_descriptor, buffer, 512);
	close(file_descriptor);
	if(strstr(buffer, "START") != NULL){
		break;
	}
	memset(buffer,0,strlen(buffer));
}
int otherIdx = 0;
//Play the game:
//While the last room has not been found
//-add the next room to  pathArray
//-read the contents of nextRoom into buffer
//-print name of nextRoom and search buffer for each name in chosenNamesArray
//-if name found, add name choicesArray, starting from beginning of array
//-keep track of how many connections are in choicesArray
//-print that number of connections from choicesArray 
//-prompt user to enter name of next destination, load into typedName
//-test if destination is valid, if not, try again
//-load valid typedName into nextName, repeat loop
//-When END_ROOM is found in buffer, print end messege and pathArray.
while(lastRoom == 0){
	connectionIndex = 0;
	pathArray[pathIndex++] = strdup(nextRoom);
	file_descriptor = open(nextRoom, O_RDWR);
	if (file_descriptor == -1){
		printf("\nFatal error\n");	
		break;

	}
	nread = read (file_descriptor, buffer, 512);
	close(file_descriptor);
	if(strstr(buffer, "END_ROOM") != NULL){
		int steps = (pathIndex - 1);
		printf("\nYOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
		printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:", steps);
		for(i = 1; i < pathIndex; i++){
			printf("\n%s", pathArray[i]);			

		}	
		char a[1];
		strcpy(a, "%");
		printf("\n%s\n", a);
		lastRoom++;
		break;
	}
	printf("\nCURRENT LOCATION: %s\n", nextRoom);
	printf("POSSIBLE CONNECTIONS: ");
	otherIdx = 0;
	
	for(idx = 0; idx < 7; idx++){
			//If name is found in buffer, and it is not the name of the room we're looking at:
			if(strstr(buffer, chosenNamesArray[idx]) != NULL && strcmp(chosenNamesArray[idx], nextRoom) != 0){ 
				choicesArray[otherIdx++] = strdup(chosenNamesArray[idx]); 
				connectionIndex++;
				}		
			}
	connectionIndex--; //Decrement connection index to print all but the last one with commas.
	for(i = 0; i < connectionIndex; i++){
	
		printf("%s, ", choicesArray[i]);
	}
	printf("%s.", choicesArray[connectionIndex]);//Print the last connection with a period.
	printf("\nWHERE TO? >");
	memset(buffer,0,strlen(buffer));
	memset(typedRoom,0,strlen(typedRoom));
	scanf(" %[^\n]", typedRoom); //The space before the % flushes stdin.		
	file_descriptor = open(typedRoom, O_RDWR);
	while(file_descriptor == -1){
		connectionIndex = 0;
		close(file_descriptor);
		file_descriptor = open(nextRoom, O_RDWR);
		nread = read(file_descriptor, buffer, 512);
		printf("\nHUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
		printf("CURRENT LOCATION: %s\n", nextRoom);
		printf("POSSIBLE CONNECTIONS: ");
		for(idx = 0; idx < 7; idx++){
			if(strstr(buffer, chosenNamesArray[idx]) != NULL && strcmp(chosenNamesArray[idx], nextRoom) != 0){ 
				choicesArray[otherIdx++] = strdup(chosenNamesArray[idx]); 
				connectionIndex++;
				}		
			}
		connectionIndex--;
		for(i = 0; i < connectionIndex; i++){
			printf("%s, ", choicesArray[i]);
		}
		printf("%s.", choicesArray[connectionIndex]);
		close(file_descriptor);
		printf("\nWHERE TO? >");
		memset(buffer,0,strlen(buffer));
		memset(typedRoom,0,strlen(typedRoom));
		scanf(" %[^\n]", typedRoom); //The space before the % flushes stdin so that user can input a string.		
		file_descriptor = open(typedRoom, O_RDWR);
	}
	close(file_descriptor); 
	cx = snprintf(nextRoom, 35, typedRoom);
}
	exit(0);
}
