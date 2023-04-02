//Country Guesser
//A combination of Wordle and the online.seterra.com Europe Country Guesser!

#include <iostream>
#include <fstream>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

using namespace std;

void file_check(FILE* file_to_check)
{
    if (file_to_check == NULL)
    {
        printf("Error! Could not open file\n");
        exit(-1);
    }
}

struct scoreboard
{
  char *name;
  char *country;
  int guesses;
  int hints[5];
};

struct country_parameters
{
    char part[5];
    char neighbours[12][25];
    int nb_of_neighbours;
};

void wait_for_enter()
{
    //The "Press Enter" prompt
    cout << "\n\nPress enter to return";
    fflush(stdin);
    for(;;)
    {
        getchar();
        system("cls");
        break;
    }
}

int verify_guess(const char answer[], const char guess[],struct country_parameters *param_list,int key)
{
    int close_guess = 0;

    //if letters don't match/length is not equal,check if the guess is close
    if(strlen(answer) == strlen(guess))
    {
        for(int i = 0; i < strlen(answer); i++)
        {
            if (tolower(answer[i]) != tolower(guess[i])){
                close_guess = 1;
            }
        }
    }
    else{
        close_guess = 1;
    }

    //if the guess is right
    if (close_guess == 0){
        return 1;
    }
    //else,if the guess isn't right, we check if it's close
    else{
        //for user convenience
        char temp_guess[25];
        strcpy(temp_guess,guess);
        for(int i = 0;i< strlen(temp_guess);i++){
            temp_guess[i] = tolower(temp_guess[i]);
        }
        temp_guess[0] = toupper(temp_guess[0]);

        //checking if the guess is close to the answer
        for(int i = 0;i<= param_list[key].nb_of_neighbours;i++)
        {
            if(strcmp(param_list[key].neighbours[i],temp_guess) == 0)
            {
                cout << "Close,try again! \n";
                break;
            }
        }
        return 0;
    }
}

void hint_select(char hint_nb,const char answer[],struct country_parameters *param_list,int key)
{
    char nr[15];
    srand(time(NULL));

    //random country for h4
    int temp = rand()%param_list[key].nb_of_neighbours;

    //random letter for h3
    int randomness = rand()%47;

    //select which hint to display
    switch(hint_nb)
    {
    //length of name
    case '1':
        if (strlen(answer) == 7){
            strcpy(nr,"equal to 7");
        }
        else if (strlen(answer) < 7){
            strcpy(nr,"less than 7");
        }
        else{
            strcpy(nr,"larger than 7");
        }
        cout << "The length of the country's name is " << nr << endl;
        break;
    //Part of Europe
    case '2':
        cout << "The country is in the " << param_list[key].part <<"sern part of Europe\n";
        break;
    //Starting letter + random letter
    case '3':
        //a condition so that h3 doesn't always show: "right letter - wrong letter"
        if (randomness <23)
            cout << "The country's name starts with is: "<< answer[0] << " " << answer[0] << " "<< param_list[randomness].neighbours[0][0] << endl;
        else
            cout << "The country's name starts with is: " << param_list[randomness].neighbours[0][0] << " " <<answer[0] << endl;
        break;
    case '4': //Random neighbour
        cout << "A random country close to the correct one is " << param_list[key].neighbours[temp] << endl;
        break;
    }
}

void display_scores()
{
    FILE* scores = fopen("Scores.txt","r");
    file_check(scores);

    int line_length = 80;
    char line[line_length];

    while(fgets(line, line_length, scores)){
        cout << line;
    }
}

void save_score(char answer[], int hints[], int nb_guesses)
{
    system("cls");
    fflush(stdin);

    FILE* score_check = fopen("Scores.txt","a");

    file_check(score_check);

    ofstream scores;
    scores.open ("Scores.txt");

    char name[16];
    int empty_name = 0;

    //Reading the name
    while(empty_name == 0)
    {
        cout << "Enter your name (max 15 characters): ";
        fgets(name,15,stdin);
        name[strcspn(name, "\n")] = 0; //remove \n
        //checking for an empty name
        if (strlen(name) != 0){
            empty_name = 1;
        }
        else{
            cout << "Please input a valid name\n";
        }
    }

    struct scoreboard p1 = {name,answer,nb_guesses};
    for (int i = 1;i <= 4 ; i++){
        p1.hints[i] = hints[i];
      }

    //printing
    scores << "\n";

    scores << p1.country << " - " << p1.name << " - ";
    cout << p1.country << " - " << p1.name << " - ";

    for (int i = 1; i <= 4 ; i++)
    {
        if(p1.hints[i] != 0)
        {
            scores << i << " ";
            cout << i << " " ;
        }
    }

    scores << "- " << p1.guesses;
    cout << "- guesses: " << p1.guesses;

    scores.close();

    wait_for_enter();
}

void sub_menu(char answer[], int hints[], int nb_guesses)
{
    cout << "\nPress 1 to go to the main menu\nPress 2 to save your score\n";
    int exit = 0;
    char choice;
    while (exit != 1)
    {
        scanf("%s",&choice);
        switch (choice)
        {
            //Menu
            case '1':
                exit = 1;
                break;
            //Save score
            case '2':
                fflush(stdout);
                save_score(answer,hints,nb_guesses);
                exit = 1;
                break;
            //Invalid Input
            default:
                cout << "Please select a valid option\n";
                break;
        }
    }
}

void parameter_assignment(const char reader[],struct country_parameters *param_list,const int j)
{
    int neighbour_count = -1;
    int name_position = 0;
    int line_found_before = 0;

    //essentially a modified strtok cuz I forgot it existed while coding this :))
    for(int k = 0; k<strlen(reader)-1; k++)
    {
        if (k < 4) //first 4 letters are always East/West
            param_list[j].part[k] = reader[k];
        if (reader[k] == '-')
        {
            line_found_before = 1;
        }
        else
        {
            if (line_found_before == 1)
            {
                neighbour_count++;
                name_position = 0;
                line_found_before = 0;
            }
            param_list[j].neighbours[neighbour_count][name_position] = reader[k];
            name_position++;
        }
    }
    param_list[j].nb_of_neighbours = neighbour_count;
}

void game()
{
    fflush(stdin); //clean the input

    //Parameter initialization for storing the countries
    int country_count = 47,i=1,j=0; //i = number of the line in the file
    char country_list[47][25]; //47 countries with names no longer than 25 characters
    struct country_parameters param_list[80];
    char reader[130];
    FILE* countries = fopen("Countries.txt","r");

    file_check(countries);

    //Read the countries and parameters from file
    while (fgets(reader, 130, countries))
    {
      if(i%2 != 0)
      {
          reader[strcspn(reader, "\n")] = 0;
          strcpy(country_list[j],reader);
      }
      else
      {
        parameter_assignment(reader,param_list,j);
        j++;
      }
      i++; //i used for reading: odd lines = countries, even lines = params.
    }

    //Choose a random answer
    char answer[25];

    srand(time(NULL));
    int key = rand()%country_count;
    strcpy(answer,country_list[key]); //randomly chosen country

    //check the guesses
    int correct_guess = 0,nb_of_guesses=0;
    int hints[5]={0,0,0,0};
    char guess[25];

    while(correct_guess != 1)
    {
        cout << "Input a country:";
        fgets(guess,25,stdin);
        guess[strcspn(guess, "\n")] = 0;

        //Selection of the hints
        if((tolower(guess[0])) == 'h' && strlen(guess)<=4)
        {
            hint_select(guess[1],answer,param_list,key);
            if (hints[guess[1]-'0'] == 0) //counting what hints were used
                hints[guess[1]-'0']++;
            continue;
        }

        nb_of_guesses++;
        correct_guess = verify_guess(answer,guess,param_list,key);

        if (correct_guess != 1){
            cout << "Wrong Country\n";
        }
        else{
            cout << "\nYou guessed the country!!\nNumber of guesses:  " << nb_of_guesses;
        }
    }

    //Sub-menu after winning
    sub_menu(answer,hints,nb_of_guesses);
    fclose(countries);
}

void help_display()
{
    system("cls");
    cout << "Guess the European country!\n";
    cout << "(recommended to play with the Seterra map of Europe: online.seterra.com/pdf/europe-countries-labeled.pdf)\n\n";
    cout << "Try to guess the random country. You will be told if your guess is close enough on the map to the correct country.\n\n";
    cout << "Typing h1,h2,h3 or h4 will show you some Hints:\n";
    cout << "h1 - Number of letters the country's name has\n";
    cout << "h2 - The part of Europe the country is in:\n";
    cout << "    a) Eastern Europe - border: Germany-Austria-Italy + Northen countries\n";
    cout << "    b) Western Europe - border: Poland-Czech Republic-Slovakia-Hungary-Slovenia\n";
    cout << "h3 - The first letter of the country's name + a random dummy letter\n";
    cout << "h4 - A random country close to the answer\n\n";
    cout << "h1/h2 - small hints , h3 - large hint , h4 - makes the game quite simple \nCombine hints to decrease difficulty or just try to blindly guess!";
}

void main_menu()
{
    int exit = 0;
    int choice;
    while (exit != 1)
    {
        cout << "Country guesser!\n1. Play the game\n2. Scores\n3. Help\n4. Exit\n";
        scanf("%d",&choice);
        switch (choice)
        {
            //1. Play the game
            case 1:
                system("cls"); //clear the console
                game();
                system("cls");
                break;
            //2. Display the scores
            case 2:
                system("cls");
                display_scores();
                wait_for_enter();
                break;
            //3. Help
            case 3:
                help_display();
                wait_for_enter();
                break;
            //4. Quit
            case 4:
                cout << "\nThank you for playing\n";
                exit = 1;
                break;
            //Invalid input
            default:
                cout << "Please select a valid option\n";
                exit = 1;
                break;
        }
    }
}

int main()
{
    main_menu();
    return 0;
}
