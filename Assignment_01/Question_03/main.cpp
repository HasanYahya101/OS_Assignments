#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <fstream>
using namespace std;

int find(string given, string to_find, int index = 0)
{
    if (index > given.length() - 1)
    {
        return -1;
    }

    if (given.length() < to_find.length())
    {
        return -1;
    }

    if (given.length() - index < to_find.length())
    {
        return -1;
    }

    char first = to_find[0];
    for (int i = index; i < given.length(); i++)
    {
        if (given[i] == first)
        {
            bool flag = true;
            int k = 0;
            for (int j = i; j < to_find.length(); j++)
            {
                if (given[j] != to_find[k])
                {
                    flag = false;
                    break;
                }
                else
                {
                    k++;
                }
            }
            if (flag == true)
            {
                return i;
            }
        }
    }

    return -1;
}

bool exists_in(char given, string field)
{
    for (int i = 0; i < field.length(); i++)
    {
        if (field[i] == given)
        {
            return true;
        }
    }
    return false;
}

int find_first_not_of(string given, string not_of, int index = 0)
{
    if (index > given.length() - 1)
    {
        return -1;
    }

    for (int i = index; i < given.length(); i++)
    {
        if (!exists_in(given[i], not_of))
        {
            return i;
        }
    }

    return -1;
}

void redact(string filename, string to_filename)
{
    pid_t pid = fork();

    if (pid == -1)
    {
        cout << "Error creating a child process" << endl;
        exit(1);
    }

    if (pid == 0) // child
    {
        ifstream input(filename);

        if (!input)
        {
            cout << "Error opening Input file" << endl;
            exit(1);
        }

        ofstream output;
        output.open(to_filename, ios::trunc);

        if (!output)
        {
            cout << "Error opening output file" << endl;
            input.close();
            exit(1);
        }

        string line;
        while (!input.eof())
        {
            getline(input, line);

            while (true)
            {
                // int start = line.find("SSN: ");
                // int end = line.find_first_not_of("0123456789-", start + 5);
                int start = find(line, "SSN: ");
                int end = find_first_not_of(line, "0123456789-", start + 5);

                if (start == -1 || end == string::npos) // error
                {
                    break;
                }

                string final = "";
                for (int i = 0; i < start; i++)
                {
                    final += line[i];
                }
                final += "[REDACTED]";
                for (int i = end; i < line.length(); i++)
                {
                    final += line[i];
                }

                line = final;
            }

            output << line << '\n';
        }

        input.close();
        output.close();

        exit(0);
    }
    else
    {
        int status;
        pid_t id = waitpid(pid, &status, 0);
        int exit_status = WEXITSTATUS(status);

        if (exit_status == 1)
        {
            exit(1); // exit the whole program with an error
        }
    }
    return;
}

void decrypt(string filename, string to_filename)
{
    pid_t pid = fork();

    if (pid == -1)
    {
        cout << "Error creating a child process" << endl;
        exit(1);
    }

    if (pid == 0) // child
    {
        ifstream input(filename);

        if (!input)
        {
            cout << "Error opening Input file" << endl;
            exit(1);
        }

        ofstream output;
        output.open(to_filename, ios::trunc);

        if (!output)
        {
            cout << "Error opening output file" << endl;
            input.close();
            exit(1);
        }

        string line;
        while (!input.eof())
        {
            getline(input, line);
            for (int i = 0; i < line.length(); i++)
            {
                if (line[i] == ' ')
                {
                    continue;
                }
                line[i] = line[i] - 1;
            }
            output << line << '\n';
        }

        input.close();
        output.close();

        exit(0);
    }
    else // parent
    {
        int status;
        pid_t id = waitpid(pid, &status, 0);
        int exit_status = WEXITSTATUS(status);

        if (exit_status == 1)
        {
            exit(1); // exit the whole program with an error
        }
    }
    return;
}

int main(int argc, char **argv)
{
    string file1;
    string file2;
    if (argc == 3)
    {
        file1 = argv[1];
        file2 = argv[2];
        // cout << file1 << file2 << endl;
    }
    else
    {
        cout << "Please Enter an input file name: ";
        cin >> file1;
        cout << "Please Enter an output file name: ";
        cin >> file2;
    }

    char command = 'a';

    cout << "Which command do u want to run? Decrypt or Redact (Enter d or r): ";
    cin >> command;

    while (command != 'd' && command != 'r')
    {
        cout << "Error! Please enter a valid value (d or r): ";
        cin >> command;
    }

    if (command == 'd')
    {
        decrypt(file1, file2);
    }
    else if (command == 'r')
    {
        redact(file1, file2);
    }
    else
    {
        cout << "Please enter a valid value" << endl;
        exit(1);
    }

    return 0;
}