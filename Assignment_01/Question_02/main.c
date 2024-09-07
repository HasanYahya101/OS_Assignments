#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>

const int FAILURE = 0;
const int SUCCESS = 1;

void initialize_random()
{
    // Seed the random number generator
    srand((unsigned int)time(NULL));
}

int random_chance(void)
{
    // Generate a random number between 0 and 99
    int rand_value = rand() % 100;
    // printf("Random no is:%d\n", rand_value);

    // Return FAILURE 30% of the time
    if (rand_value < 30)
    {
        return FAILURE;
    }

    // Return SUCCESS 70% of the time
    return SUCCESS;
}

int material_prep(void)
{
    printf("\n<---Material Preparation Stage--->\n");
    int attempt = 1;
    pid_t pid;
    while (1)
    {
        pid = fork();
        initialize_random(); // re seed to prevent same value

        if (pid == 0)
        {
            int result;

            printf("Preparing materials...\n");
            sleep(3);

            result = random_chance();

            if (result == SUCCESS)
            {
                printf("Material Stage Passed Successfully\n");
                exit(0);
            }
            else
            {
                exit(1);
            }
        }
        else
        {
            int status;
            pid_t id = waitpid(pid, &status, 0);
            if (id < 0)
            {
                printf("Wait Function failed\n");
                exit(1);
            }

            int exit_status = WEXITSTATUS(status);
            if (exit_status == 0)
            {
                break;
            }

            if (attempt < 3)
            {
                attempt++;
                printf("Restarting the material stage\n");
            }
            else
            {
                if (exit_status == 1) // Complete failure
                {
                    printf("Material Prep has failed Completly. Exiting the Production Line entirely\n");
                    exit(1);
                }
                break;
            }
        }
    }
    return attempt - 1;
}

int assembly(void)
{
    printf("\n<---Assembly Stage--->\n");
    int attempt = 1;
    pid_t pid;
    while (1)
    {
        pid = fork();
        initialize_random(); // re seed to prevent same value

        if (pid == 0)
        {
            int result;

            printf("Preparing assembly of materials...\n");
            sleep(3);

            result = random_chance();

            if (result == SUCCESS)
            {
                printf("Assembly Stage Passed Successfully\n");
                exit(0);
            }
            else
            {
                exit(1);
            }
        }
        else
        {
            int status;
            pid_t id = waitpid(pid, &status, 0);
            if (id < 0)
            {
                printf("Wait Function failed\n");
                exit(1);
            }

            int exit_status = WEXITSTATUS(status);
            if (exit_status == 0)
            {
                break;
            }

            if (attempt < 3)
            {
                attempt++;
                printf("Restarting the assembly stage\n");
            }
            else
            {
                if (exit_status == 1) // Complete failure
                {
                    printf("Assembly has failed Completly. Exiting the Production Line entirely\n");
                    exit(1);
                }
                break;
            }
        }
    }
    return attempt - 1;
}

int quality_check(void)
{
    printf("\n<---Quality Check Stage--->\n");
    int attempt = 1;
    pid_t pid;
    while (1)
    {
        pid = fork();
        initialize_random(); // re seed to prevent same value

        if (pid == 0)
        {
            int result;

            printf("Checking quality of materials...\n");
            sleep(3);

            result = random_chance();

            if (result == SUCCESS)
            {
                printf("Quality Stage Passed Successfully\n");
                exit(0);
            }
            else
            {
                exit(1);
            }
        }
        else
        {
            int status;
            pid_t id = waitpid(pid, &status, 0);
            if (id < 0)
            {
                printf("Wait Function failed\n");
                exit(1);
            }

            int exit_status = WEXITSTATUS(status);
            if (exit_status == 0)
            {
                break;
            }

            if (attempt < 3)
            {
                attempt++;
                printf("Restarting the quality stage\n");
            }
            else
            {
                if (exit_status == 1) // Complete failure
                {
                    printf("Qulaity check has failed Completly. Exiting the Production Line entirely\n");
                    exit(1);
                }
                break;
            }
        }
    }
    return attempt - 1;
}

int packaging(void)
{
    printf("\n<---Packaging Stage--->\n");
    int attempt = 1;
    pid_t pid;
    while (1)
    {
        pid = fork();
        initialize_random(); // re seed to prevent same value

        if (pid == 0)
        {
            int result;

            printf("Packaging materials...\n");
            sleep(3);

            result = random_chance();

            if (result == SUCCESS)
            {
                printf("Packaging Stage Passed Successfully\n");
                exit(0);
            }
            else
            {
                exit(1);
            }
        }
        else
        {
            int status;
            pid_t id = waitpid(pid, &status, 0);
            if (id < 0)
            {
                printf("Wait Function failed\n");
                exit(1);
            }

            int exit_status = WEXITSTATUS(status);
            if (exit_status == 0)
            {
                break;
            }

            if (attempt < 3)
            {
                attempt++;
                printf("Restarting the packaging stage\n");
            }
            else
            {
                if (exit_status == 1) // Complete failure
                {
                    printf("Packaging has failed Completly. Exiting the Production Line entirely\n");
                    exit(1);
                }
                break;
            }
        }
    }
    return attempt - 1;
}

int main()
{
    initialize_random();

    int m_failure = material_prep();
    int a_failure = assembly();
    int q_failure = quality_check();
    int p_failure = packaging();

    printf("\n\n");
    printf("Summary Report:\n");
    printf("Material Prep Failures: %d\n", m_failure);
    printf("Assembly Failures: %d\n", a_failure);
    printf("Quality Check Failures: %d\n", q_failure);
    printf("Packaging Failures: %d\n", p_failure);

    return 0;
}