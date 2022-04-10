#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "stdbool.h"

typedef struct particle
{
    int type;
    bool cluster;

} particle;

typedef struct coordinates
{
    int x;
    int y;
    int z;

} coordinate;

void InitializeArray(particle*** arr_particle, int lx, int ly, int lz)
{
    for (int x = 0; x < lx; x++)
    {
        for (int y = 0; y < ly; y++)
        {
            for (int z = 0; z < lz; z++)
            {
                arr_particle[x][y][z].cluster = false;
                arr_particle[x][y][z].type = 0;
            }
        }
    }
}


void InputRandomCoordinates(particle*** arr_particle, int lx, int ly, int lz, int N, int fraction)
{
    int numb_part = 0;

    while (numb_part < N)
    {
        int xp = (rand() % lx);
        int yp = (rand() % ly);
        int zp = (rand() % lz);

        if (arr_particle[xp][yp][zp].type == 0)
        {
            if ((double)numb_part < (double)N * (double)fraction / 100)
            {
                arr_particle[xp][yp][zp].type = -1;
            }
            else
            {
                arr_particle[xp][yp][zp].type = 1;
            }
            numb_part++;
        }

    }
}

coordinate GetStart(particle*** arr_particles, int lx, int ly, int lz)
{
    while (true)
    {
        int rand_x = rand() % lx;
        int rand_y = rand() % ly;
        int rand_z = rand() % lz;

        if (arr_particles[rand_x][rand_y][rand_z].type != 0)
        {
            coordinate result = { rand_x , rand_y, rand_z };

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
        roster[index].z = -1;
    }
}

void PrintRoster(coordinate* roster, int N)
{
    for (int index = 0; index < N; index++)
    {
        printf("X = %d Y = %d Z = %d \n", roster[index].x, roster[index].y, roster[index].z);
    }
}

bool should_they_connect(particle*** arr_particle, coordinate* roster, int seed, int d, int j, int k, int lx, int ly, int lz)
{
    int x = roster[seed].x;
    int y = roster[seed].y;
    int z = roster[seed].z;

    if (x + d < 0 || x + d >= lx || y + j < 0 || y + j >= ly || z + k < 0 || z + k >= lz)
    {
        return false;
    }
    //we now know the grid space exists, so we can ask questions about the particle

    if (arr_particle[x + d][y + j][z + k].type == 0 || arr_particle[x + d][y + j][z + k].cluster == true)
    {
        return false;
    }

    if (arr_particle[x][y][z].type == 1 || arr_particle[x + d][y + j][z + k].type == 1)
    {
        if ((d != 0 && j != 0)||(k!=0 && j!=0)||(k!=0 && d!=0))
        {
            return false;
        }
    }
    //if it does not "pass" any of the fail conditions, the particle is indeed in the cluster and as such the function returns true
    return true;
}

void AnalyzeRoster(particle*** arr_particle, coordinate* roster, int index, int seed, int lx, int ly, int lz)
{
    while (index != seed)
    {
        for (int d = -1; d <= 1; d++)
        {
            for (int j = -1; j <= 1; j++)
            {
                for (int k = -1; k <= 1; k++)
                {
                    if (j != 0 || k != 0 || d != 0)
                    { //by this point every neighbour (3d) is being looked at
                        if (should_they_connect(arr_particle, roster, seed, d, j, k, lx, ly, lz))
                        {
                            arr_particle[roster[seed].x + d][roster[seed].y + j][roster[seed].z + k].cluster = true;

                            roster[index].x = roster[seed].x + d;
                            roster[index].y = roster[seed].y + j;
                            roster[index].z = roster[seed].z + k;

                            index++;
                        }
                    }
                }
            }

        }
        seed++;
    }
}

bool can_current_flow(particle*** arr_particle, int lx, int ly, int lz)
{
    for (int y = 0; y < ly; y++)
    {
        for (int x = 0; x < lx; x++)
        {
            //checking top line
            if (arr_particle[x][y][0].cluster)
            {
                for (int y1 = 0; y1 < ly; y1++)
                {
                    for (int x1 = 0; x1 < ly; x1++)
                    {
                        //checking bottom line
                        if (arr_particle[x1][y1][lz - 1].cluster)
                        {
                            return true;
                        }
                    }
                }
                //so it doesn't iterate through the bottom plane if top line doesn't have anything
                return false;
            }
        }
    }
    return false;
}


int main()
{
    int lx, ly, lz, N, n_grids, fraction;

    printf("Enter Ly: ");
    scanf("%d", &ly);

    printf("Enter Lx: ");
    scanf("%d", &lx);

    printf("Enter Lz: ");
    scanf("%d", &lz);

    printf("Enter N:  ");
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

        if (ly * lx * lz <= N)
        {
            printf("Too many particles for this grid size.");
            exit(-1);
        }
        
        particle*** arr_particle;
        arr_particle = malloc(lx * sizeof(**arr_particle));
        for (int iter_x = 0; iter_x < lx; iter_x++)
        {
            arr_particle[iter_x] = malloc(ly * sizeof(*arr_particle));
            for (int iter_y = 0; iter_y < ly; iter_y++)
            {
                arr_particle[iter_x][iter_y] = malloc(lz * sizeof(particle));
            }
        }

        InitializeArray(arr_particle, lx, ly, lz);

        InputRandomCoordinates(arr_particle, lx, ly, lz, N, fraction);
        
        coordinate first;
        first = GetStart(arr_particle, lx, ly, lz);

        coordinate* roster;
        roster = malloc(N * sizeof(coordinate));

        InitializeRoster(roster, N);
   
        int index = 0;
        int seed = 0;

        //setting the initial zero point to valid
        roster[index] = first;
        arr_particle[roster[index].x][roster[index].y][roster[index].z].cluster = true;
        index++;

        AnalyzeRoster(arr_particle, roster, index, seed, lx, ly, lz);
        
        if (can_current_flow(arr_particle, lx, ly, lz))
        {
            fraction_grids_with_connecting_path++;
        }

        //PrintRoster(roster, N);

        for (int x = 0; x < lx; x++)
        {
            for (int y = 0; y < ly; y++)
            {
                free(arr_particle[x][y]);
            }
            free(arr_particle[x]);
        }
        free(arr_particle);

        free(roster);
    }

    printf("Fraction of grids for which you there is a connecting path: %d / %d", fraction_grids_with_connecting_path, n_grids);

    return 0;
}
