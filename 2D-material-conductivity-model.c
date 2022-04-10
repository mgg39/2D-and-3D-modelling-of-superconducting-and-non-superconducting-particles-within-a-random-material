#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "stdbool.h"

typedef struct particle
{
    //initialized to empty type
    int type;
    bool cluster;

} particle;

typedef struct coordinates
{
    int x;
    int y;

} coordinate;

void InitializeArray(particle** arr_particle, int lx, int ly)
{
    for (int x = 0; x < lx; x++)
    {
        for (int y = 0; y < ly; y++)
        {
            arr_particle[x][y].cluster = false;
            arr_particle[x][y].type = 0;
        }
    }
}

void InputRandomCoordinates(particle** arr_particle, int lx, int ly, int N, int fraction)
{
    int numb_part = 0;

    while (numb_part < N)
    {
        int xp = (rand() % lx);
        int yp = (rand() % ly);

        if (arr_particle[xp][yp].type == 0)
        {
            if ((double)numb_part < (double)N * (double)fraction / 100)
            {
                arr_particle[xp][yp].type = -1;
            }
            else
            {
                arr_particle[xp][yp].type = 1;
            }
            numb_part++;
        }

    }
}

/*void PrintArray(particle** arr_input, int lx, int ly)
{
    //printf("  0  1  2  3  4  5  6  7\n");
    for (int y = 0; y < ly; y++)
    {
        //printf("%d", y);
        for (int x = 0; x < lx; x++)
        {
            if (arr_input[x][y].type == -1) { printf(" * "); }
            if (arr_input[x][y].type == 0) { printf("   "); }
            if (arr_input[x][y].type == 1) { printf(" + "); }
        }
        printf("\n");
    }
}*/

coordinate GetStart(particle** arr_particles, int lx, int ly)
{
    while (true)
    {
        int rand_x = rand() % lx;
        int rand_y = rand() % ly;

        if (arr_particles[rand_x][rand_y].type != 0)
        {
            coordinate result = { rand_x , rand_y };

            return result;
        }
    }
}

void InitializeRoster(coordinate* roster, int N)
{
    for (int index = 0; index < N; index++)
    {
        roster[index].x = -1;
        roster[index].y = -1;
    }
}

/*void PrintRoster(coordinate* roster, int N)
{
    for (int index = 0; index < N; index++)
    {
        printf("X = %d Y = %d \n", roster[index].x, roster[index].y);
    }
}*/

bool should_they_connect(particle** arr_particle, coordinate* roster, int seed, int d, int j, int lx, int ly)
{
    int x = roster[seed].x;
    int y = roster[seed].y;

    if (x + d < 0 || x + d >= lx || y + j < 0 || y + j >= ly)
    {
        return false;
    }
    //we now know the grid space exists, so we can ask questions about the particle

    if (arr_particle[x + d][y + j].type == 0 || arr_particle[x + d][y + j].cluster == true)
    {
        return false;
    }

    if (arr_particle[x][y].type == 1 || arr_particle[x + d][y + j].type == 1)
    {
        if (d != 0 && j != 0)
        {
            return false;
        }
    }
    //if it does not "pass" any of the fail conditions, the particle is indeed in the cluster and as such the function returns true
    return true;
}

void AnalyzeRoster(particle** arr_particle, coordinate* roster, int index, int seed, int lx, int ly)
{
    //!(roster[index].x == roster[seed].x && roster[index].y == roster[seed].y)
    while (index != seed)
    {
        for (int d = -1; d <= 1; d++)
        {
            for (int j = -1; j <= 1; j++)
            {
                if (j != 0 || d != 0)
                { //by this point every neighbour (2d) is being looked at
                    if (should_they_connect(arr_particle, roster, seed, d, j, lx, ly))
                    {
                        arr_particle[roster[seed].x + d][roster[seed].y + j].cluster = true;

                        roster[index].x = roster[seed].x + d;
                        roster[index].y = roster[seed].y + j;

                        //printf("x, y = %d, %d ", roster[index].x, roster[index].y);
                        //printf("index = %d ", index);
                        //printf("seed = %d \n", seed);

                        index++;
                    }
                }
            }

        }
        //arr_particle[roster[seed].x][roster[seed].y].checked = true;
        seed++;
    }
}

void PrintGrid(particle** arr_particle, int lx, int ly, coordinate first)
{
    for (int y = 0; y < ly; y++)
    {
        for (int x = 0; x < lx; x++)
        {
            if (arr_particle[x][y].type != 0)
            {
                if (arr_particle[x][y].cluster == true)
                {
                    if (x == first.x && y == first.y) { printf(" 0 "); }
                    else { printf(" 1 "); }
                }
                if (arr_particle[x][y].cluster == false) { printf("-1 "); }
            }
            else { printf(" X "); }
        }
        printf("\n");
    }
}

bool can_current_flow(particle** arr_particle, int lx, int ly)
{
    for (int x1 = 0; x1 < lx; x1++)
    {
        //checking top line
        if (arr_particle[x1][0].cluster)
        {
            for (int x2 = 0; x2 < lx; x2++)
            {
                //checking bottom line
                if (arr_particle[x2][ly - 1].cluster)
                {
                    return true;
                }
            }
            //so it doesn't iterate through the bottom line if top line doesn't have anything
            return false;
        }
    }
    return false;
}



int main()
{
    int lx, ly, N, n_grids, fraction;

    printf("Enter number of rows (Ly): ");
    scanf("%d", &ly);

    printf("Enter number of columns (Lx): ");
    scanf("%d", &lx);

    printf("Enter number of particles randomly distributed through the grid (N): ");
    scanf("%d", &N);

    printf("Enter how many grids you would like to print: ");
    scanf("%d", &n_grids);

    printf("Enter the percentage of particles you would like to be superconductors: ");
    scanf("%d", &fraction);

    srand(time(NULL) + lx);

    //printf("\n");
    int fraction_grids_with_connecting_path = 0;

    for (int i = 0; i < n_grids; i++)
    {
       // printf("\n");

        if (ly * lx <= N)
        {
            printf("Too many particles for this grid size.");
            exit(-1);
        }

        particle** arr_particle;
        arr_particle = malloc(lx * sizeof(*arr_particle));
        for (int arr = 0; arr < lx; arr++)
        {
            arr_particle[arr] = malloc(ly * sizeof(particle));
        }

        InitializeArray(arr_particle, lx, ly);

        InputRandomCoordinates(arr_particle, lx, ly, N, fraction);

        //PrintArray(arr_particle, lx, ly);

        coordinate first;
        first = GetStart(arr_particle, lx, ly);

        coordinate* roster;
        roster = malloc(N * sizeof(coordinate));

        InitializeRoster(roster, N);

        int index = 0;
        int seed = 0;

        //setting the initial zero point to valid
        roster[index] = first;
        arr_particle[roster[index].x][roster[index].y].cluster = true;
        index++;

        //PrintRoster(roster, N);

        AnalyzeRoster(arr_particle, roster, index, seed, lx, ly);

        //PrintRoster(roster, N);
        //PrintAnalyzedRoster( roster, N, arr_particle);

        //PrintGrid(arr_particle, lx, ly, first);

        if (can_current_flow(arr_particle, lx, ly))
        {
            fraction_grids_with_connecting_path++;
        }

        //printf("\n");

        for (int i = 0; i < lx; i++)
        {
            free(arr_particle[i]);
        }
        free(arr_particle);
        free(roster);
    }

    printf("Fraction of grids for which you there is a connecting path: %d / %d", fraction_grids_with_connecting_path, n_grids);

    return 0;
}
