
#include <iostream>
#include <vector>
#include <queue>
#include <sstream>
using namespace std;

struct Puzzle8 {
    Puzzle8(){}
    Puzzle8( vector<vector<int>> state ){
        this->state = state;
    }
    Puzzle8( vector<vector<int>> state, int depth ){
        this->state = state;
        this->depth = depth;
    }
    
    //Index of the blank space
    struct BlankSpace{
        int row;
        int col;
    } blankSpace;
    
    //How many expansion to reach the current state
    int depth = 0;
    
    //Heuristic cost of the current state
    int heuristicCost = 0;
    
    //Initial state
    vector<vector<int>> state = {
        {0,7,2},
        {4,6,1},
        {3,5,8}
    };
    
    //Goal state
    vector<vector<int>> goal = {
        {1,2,3},
        {4,5,6},
        {7,8,0}
    };
    
    //Container to hold all children
    vector<Puzzle8> children = {};
    
    //Function to display state
    void displayState(){
        for( auto& row: state ){
            printf("| %d %d %d |\n", row[0], row[1], row[2]);
        }
    }
    
    //Function to update the index of the blank space
    void updateBlankSpace() {
        for( int row=0; row<3; row++){
            for( int col=0; col<3; col++){
                if( state[row][col] == 0){
                    blankSpace.row = row;
                    blankSpace.col = col;
                }
            }
        }
    }
    
    //Function to expand current state into two or more possible states
    void expandState () {
        updateBlankSpace();
        
        //Swap up
        if( blankSpace.row == 1 || blankSpace.row == 2) {
            Puzzle8 copy( state, depth+1 );
            swap( copy.state[blankSpace.row][blankSpace.col],
                 copy.state[blankSpace.row-1][blankSpace.col]);
            children.push_back(copy);
        }
        
        //Swap down
        if( blankSpace.row == 0 || blankSpace.row == 1) {
            Puzzle8 copy( state, depth+1 );
            swap( copy.state[blankSpace.row][blankSpace.col],
                  copy.state[blankSpace.row+1][blankSpace.col]);
            children.push_back(copy);
        }
        
        //Swap left
        if( blankSpace.col == 1 || blankSpace.col == 2) {
            Puzzle8 copy( state, depth+1 );
            swap( copy.state[blankSpace.row][blankSpace.col],
                 copy.state[blankSpace.row][blankSpace.col-1]);
            children.push_back(copy);
        }
        
        //Swap right
        if( blankSpace.col == 0 || blankSpace.col == 1) {
            Puzzle8 copy( state, depth+1 );
            swap( copy.state[blankSpace.row][blankSpace.col],
                  copy.state[blankSpace.row][blankSpace.col+1]);
            children.push_back(copy);
        }
        
    }
};

void generalSearch( int method, Puzzle8& puzzle ){
    int maxQueueSize      = 0;
    int const UniformCost = 1;  //These 3 variables will be used to determine which method to use
    int const Misplaced   = 2;  //in a switch statement below
    int const Manhattan   = 3;  //
        
    queue<Puzzle8*>     unexploredQueue     = {};   //unexploredQueue and exploredQueue are for all search methods.
    vector<Puzzle8*>    exploredQueue       = {};   //
    
    vector<Puzzle8*>    allHeuristicStates  = {};   //allHeuristicStates is for misplaced and manhattan search.
                                                    //It contains all children states that are not pushed into the
                                                    //unexploredQueue yet
    
    unexploredQueue.push( &puzzle );    //Pushing the initial state into the unexploredQueue
    Puzzle8* currentState = unexploredQueue.front();    //currentState represents unexploredQueue.front() to make code a bit shorter
    
    //------------------------------------------------------------------------------------------------------------//
    //                                       Function checkDuplicate()                                            //
    //------------------------------------------------------------------------------------------------------------//
    //  This lambda function will be called by Mispalced Tiles & Manhattan Distance methods.
    //  This function will check each state inside allHeuristicStates with exploredQueue
    //  If the state is not a duplicate, then it will be pushed into unexploredQueue
    //  I extracted this block from each method to reduce code
    //
    auto checkDuplicate = [&](){
        //Sort states in allHeuristicStates container by their total cost in ascending order
        sort( allHeuristicStates.begin(), allHeuristicStates.end(), [](Puzzle8 *first, Puzzle8 *second ){
            return (first->heuristicCost + first->depth) < (second->heuristicCost + second->depth );
        });

        //If the state with the least heuristic cost has been explored before,
        //then we remove it, until we find the next state with the least heuristic cost that has not been explored.
        //allHeuristicStates can still contain duplicates.
        //I am only checking with the exploredQueue
        for( int size=0; size<exploredQueue.size(); size++ ){
            if( allHeuristicStates.front()->state == exploredQueue[size]->state ){
                allHeuristicStates.erase( allHeuristicStates.begin() );
                size=0;
            }
        }
        
        //Recording queue size
        if( maxQueueSize <= allHeuristicStates.size() ){
            maxQueueSize = allHeuristicStates.size();
        }
        
        //Push the state with the least cost into unexploredQueue
        unexploredQueue.push( allHeuristicStates.front() );
        allHeuristicStates.erase( allHeuristicStates.begin() );
    };
    //------------------------------------------------------------------------------------------------------------//
    //                                        END OF checkDuplicate()                                             //
    //------------------------------------------------------------------------------------------------------------//
    
    //Begin algorithm
    while( !unexploredQueue.empty() ){
        
        //Display current state
        cout <<"Current state is: \n";
        currentState->displayState();
        
        //When goal state is reached, the functino returns
        if( currentState->state == puzzle.goal ){
            printf("<-------------------------------------------------------------->\n\tGoal reached at %d. %lu states expanded.\n", currentState->depth, exploredQueue.size());
            printf("\tMaximum queue size was %d\n\tProgram ending...\n<--------------------------------------------------------------->", maxQueueSize);
            return;
        }
        
        //Expanding the current state
        currentState->expandState();

        //For each children state, we will perform different operation, depending on the choosen method passed in
        for( int x=0; x<currentState->children.size(); x++ ){
            
           /*--------------------------------------------------------------*
            *        The 3 search methods perform differently below        *
            *--------------------------------------------------------------*/
            
            switch( method ){
                
                //-------------------------------------//
                //     Uniform Cost Search Method      //
                //-------------------------------------//
                case UniformCost:{
                    if( exploredQueue.empty() ){
                        //First iteration exploredQueue will be empty, so we don't need to check for duplicate state
                        unexploredQueue.push( &currentState->children[x] );
                    }else{
                        //After the first iteration, we will begin to compare each children state to determine
                        //if it has been explored
                        for( int y=0; y<exploredQueue.size(); y++ ){
                            if( currentState->children[x].state == exploredQueue[y]->state ){
                                //If it has been explored, then we do not push into unexploredQueue
                                break;
                            }

                            if( y == exploredQueue.size() - 1 ){
                                //If it has not been explored, then we push into the unexploredQueue
                                unexploredQueue.push( &currentState->children[x] );
                            }
                        }
                    }
                    
                    if( maxQueueSize <= unexploredQueue.size() ){
                        maxQueueSize = unexploredQueue.size();
                    }
                } break;
                //--------------------------------------//
                //  END of Uniform Cost Search Method   //
                //--------------------------------------//
                
                
                //----------------------------------//
                //      Misplaced Tiles Method      //
                //----------------------------------//
                case Misplaced:{
                    //This nested for loops calculates the heuristic cost of each children state
                    for( int row=0; row<3; row++ ){
                        for( int col=0; col<3; col++ ){
                            if( currentState->children[x].state[row][col] != puzzle.goal[row][col] && currentState->children[x].state[row][col] != 0){
                                currentState->children[x].heuristicCost++;
                            }
                        }
                    }
                    //Pushing all children states into allHeuristicStates container
                    allHeuristicStates.push_back( &currentState->children[x] );
                    
                    //After we finish calculating the heuristic cost of each children state, we will check for duplicate.
                    //Then, we will choose the state that has the least heuristic cost to push into unexploredQueue.
                    //x is the index of the children we are currently exploring
                    if( x ==  currentState->children.size() - 1 ){
                        checkDuplicate();
                    }
                } break;
                //-----------------------------------//
                //   END of Misplaced Tiles Method   //
                //-----------------------------------//
                
                
                //----------------------------------//
                //    Manhattan Distance Method     //
                //----------------------------------//
                case Manhattan:{
                    //misplacedNumber will hold the number that is misplaced
                    int misplacedNumber;
                    
                    //This nested loop will scan each number in each state and update the heuristic cost accordingly
                    for( int row=0; row<3; row++ ){
                        for( int col=0; col<3; col++ ){
                            if( currentState->children[x].state[row][col] != puzzle.goal[row][col] && currentState->children[x].state[row][col] != 0){

                                //Found the misplaced number
                                misplacedNumber = currentState->children[x].state[row][col];
                                
                                //Calculating manhattan distance based on the number that is misplaced
                                if(       misplacedNumber == 1 ){   //Number 1 is misplaced
                                    currentState->children[x].heuristicCost += abs(row-0) + abs(col-0);
                                }else if( misplacedNumber == 2 ){   //Number 2 is misplaced
                                    currentState->children[x].heuristicCost += abs(row-0) + abs(col-1);
                                }else if( misplacedNumber == 3 ){   //Number 3 is misplaced
                                    currentState->children[x].heuristicCost += abs(row-0) + abs(col-2);
                                }else if( misplacedNumber == 4 ){   //Number 4 is misplaced
                                    currentState->children[x].heuristicCost += abs(row-1) + abs(col-0);
                                }else if( misplacedNumber == 5 ){   //Number 5 is misplaced
                                    currentState->children[x].heuristicCost += abs(row-1) + abs(col-1);
                                }else if( misplacedNumber == 6 ){   //Number 6 is misplaced
                                    currentState->children[x].heuristicCost += abs(row-1) + abs(col-2);
                                }else if( misplacedNumber == 7 ){   //Number 7 is misplaced
                                    currentState->children[x].heuristicCost += abs(row-2) + abs(col-0);
                                }else if( misplacedNumber == 8 ){   //Number 8 is misplaced
                                    currentState->children[x].heuristicCost += abs(row-2) + abs(col-1);
                                }
                            }
                        }
                    }
                    
                    //Pushing all children state into allHeuristicStates
                    allHeuristicStates.push_back( &currentState->children[x] );
                    
                    //When we finish calculating the heuristic cost of each children state,
                    //we will choose the state that has the least heuristic cost to push into the unexploredQueue.
                    //x is the index of the children we are currently exploring
                    if( x ==  currentState->children.size() - 1 ){
                        checkDuplicate();
                    }
                } break;
                //------------------------------------//
                //  END of Manhattan Distance Method  //
                //------------------------------------//
                
                default:{
                    cout<<"Something went wrong! Did you type something weird?\n";
                    return;
                }
            }//-----> END of switch statement
        }
        
        
        /*--------------------------------------------------------------*
         *     The 3 search methods perform the same again below        *
         *--------------------------------------------------------------*/
        
        exploredQueue.push_back( currentState );    //After we finished exploring the current state, we can put it inside exploredQueue.
        unexploredQueue.pop();                      //Then we pop it out of unexploredQueue.
        currentState = unexploredQueue.front();     //Updating the pointer to the next state to be explored
    }
    //If the unexploredQueue is empty, then no solotion was found
    cout <<"No solution found. Program ending...\n";
}

int main() {
    string initialState;
    int temp;
    int method;
    Puzzle8 puzzle;

    printf("Welcome to my program. Would you like to test a custom puzzle or the default puzzle?\n\t1) I want to test a custom puzzle.\n\t2) I want to test the default puzzle.\n");
    cin >> method;

    if( method == 1 ){
        printf("Separated by space, please enter numbers 1-8 and 0 which represents the blank space: ");
        cin.ignore();
        getline( cin, initialState );
        stringstream parse( initialState );

        //Initializing custom puzzle
        for( int row=0; row<3; row++ ){
            for( int col=0; col<3; col++ ){
                parse >> temp;
                puzzle.state[row][col] = temp;
            }
        }
    }else if (  method != 2 ){
        printf("Oops. I think you might have typed something weird. Please restart the program.\n ");
        exit(0);
    }

    printf("I have 3 methods for you to choose from: \n\t1) Uniform Cost Search\n\t2) Misplaced Tiles Search\n\t3) Manhattan Distance Search\nPlease press the corresponding number and then press enter to begin the algorithm:");
    cin >> method;

    //General search function
    generalSearch( method, puzzle );
}
