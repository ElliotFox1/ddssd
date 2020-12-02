#include <iostream>
#include <vector>
#include <cmath>
#include <mpi.h>

using namespace std;

int main() {




	double xyDimension {100};
	double mainGrid[100][100] = {0};
	double updatedGrid[100][100] = {0};
	double changeGrid[100][100] = {0};


	bool keepGoing {true};
	double largestChange {0};
	double change {0};

	mainGrid[49][49] = 10.0;
	mainGrid[39][59] = 7.2;
	mainGrid[69][24] = -1.2;

	updatedGrid[49][49] = 10.0;
	updatedGrid[39][59] = 7.2;
	updatedGrid[69][24] = -1.2;

	double globalLargestChange{0};
	double start =  MPI_Wtime();

	MPI_Request sendRequest, recieveRequest;

	MPI::Init ( );
	int rank = MPI::COMM_WORLD.Get_rank ( );
	int size = MPI::COMM_WORLD.Get_size ( );

	cout << size << "  " << rank << endl;

	double pRow = xyDimension/size;
	double pRowLast = xyDimension;

	pRow = floor(pRow);
	pRowLast = pRow;

	int from, to, sFrom, sTo;

	if(rank < size-1){
		from = rank*pRow;
		to = from+pRow;
        }
	else{
		from = rank*pRow;
		to = from + pRowLast;
		}

	for (int h {0}; h < 50960; h++){

        if(size > 1){
			if( !(rank % 2)  && rank < size-1 ){
                MPI_Send(&mainGrid[to-1][0], xyDimension, MPI_DOUBLE, rank + 1, 40, MPI_COMM_WORLD); // 222 - unique tag
				}
			if( (rank % 2)  && size>1 ){
				MPI_Recv(&mainGrid[from-1][0], xyDimension, MPI_DOUBLE, rank - 1, 40, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				}
			if( (rank % 2)  && rank < (size-1) ){
                MPI_Send(&mainGrid[to-1][0], xyDimension, MPI_DOUBLE, rank + 1, 50, MPI_COMM_WORLD);
				}
			if( !(rank % 2)  && rank > 0){
                MPI_Recv(&mainGrid[from-1][0], xyDimension, MPI_DOUBLE, rank - 1, 50, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				}

			if( !(rank % 2)  && rank > 0 ){
                MPI_Send(&mainGrid[from][0], xyDimension, MPI_DOUBLE, rank - 1, 40, MPI_COMM_WORLD);
				}
			if( (rank % 2)  && rank < (size - 1)){
                MPI_Recv(&mainGrid[to][0], xyDimension, MPI_DOUBLE, rank + 1, 40, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				}
			if( (rank % 2) ){
                MPI_Send(&mainGrid[from][0], xyDimension, MPI_DOUBLE, rank - 1, 50, MPI_COMM_WORLD);
				}
			if( !(rank % 2)  && rank < (size -1) ){
                MPI_Recv(&mainGrid[to][0], xyDimension, MPI_DOUBLE, rank + 1, 50, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				}
			}

	for (int i {rank*(xyDimension/size)}; i < (rank+1)*(xyDimension/size); i++){
		for (int j {0}; j < xyDimension; j++){
			if (i == 0 && j == 0) {
				updatedGrid[i][j] = (mainGrid[i][j] + mainGrid[i+1][j] + mainGrid[i][j+1])/5;
				}
			else if (i == 0 && j != 99 && j != 0){
				updatedGrid[i][j] = (mainGrid[i][j] + mainGrid[i+1][j] + mainGrid[i][j-1] + mainGrid[i][j+1])/5;
				}
			else if (i == 0 && j == 99) {
				updatedGrid[i][j] = (mainGrid[i][j] + mainGrid[i+1][j] + mainGrid[i][j-1])/5;
				}
			else if (i == 99 && j == 0) {
				updatedGrid[i][j] = (mainGrid[i][j] + mainGrid[i-1][j] + mainGrid[i][j+1])/5;
				}
			else if (i == 99 && j != 99 && j != 0){
				updatedGrid[i][j] = (mainGrid[i][j] + mainGrid[i-1][j] + mainGrid[i][j-1] + mainGrid[i][j+1])/5;
				}
			else if (i == 99 && j == 99) {
				updatedGrid[i][j] = (mainGrid[i][j] + mainGrid[i-1][j] + mainGrid[i][j-1])/5;
				}
			else if (j == 0 && i != 99 && i != 0){
				updatedGrid[i][j] = (mainGrid[i][j] + mainGrid[i+1][j] + mainGrid[i-1][j] + mainGrid[i][j+1])/5;
				}
			else if (j == 99 && i != 99 && i != 0){
				updatedGrid[i][j] = (mainGrid[i][j] + mainGrid[i+1][j] + mainGrid[i-1][j] + mainGrid[i][j-1])/5;
				}
			else if ((i != 49 || j != 49) && (i != 39 || j != 59) && (i != 69 || j != 24)){
				updatedGrid[i][j] = (mainGrid[i][j]+mainGrid[i][j-1]+mainGrid[i][j+1]+mainGrid[i-1][j]+mainGrid[i+1][j])/5;
				}
			}
		}



	for (int b {rank*(xyDimension/size)}; b < (rank+1)*(xyDimension/size); b++ ){
		for (int n {0}; n < xyDimension; n++ ){
			mainGrid[b][n] = updatedGrid[b][n];
			}
		}

	for(int s = 1; p < size; s++){
		if(rank == s){
			for(int g = from; g<to;g++){
				MPI_Send(&mainGrid[g][0], xyDimension, MPI_DOUBLE, 0 , s*1000+g , MPI_COMM_WORLD);
				}
			}
		else if (rank == 0){
			if(s<size-1){
				sFrom = pRow*s;
				sTo = pRow*(s+1);
				}
			else{ 
				sFrom = pRow*s;
				sTo = xyDimension;
				}
                        
			for(int g = sFrom; g < sTo; g++){
				MPI_Recv(&updatedGrid[g][0], xyDimension, MPI_DOUBLE, p , s*1000+g , MPI_COMM_WORLD, MPI_STATUS_IGNORE);              
				}
			}
		}

        if(rank == 0){
        cout.precision(2);
        for (int i {0}; i < xyDimension; i++){
                cout << endl;
                for(int j {0}; j < xyDimension ; j++){
                        cout << updatedGrid[i][j];
                        }

                }

        cout << endl;
        cout << endl;
        cout << endl;
		cout << "point (55,55) is : " << updatedGrid[54][54] << endl;
			}

        MPI::Finalize();

        double end =  MPI_Wtime();

        cout << "Time to run this program is : " << end - start << endl;

    }
