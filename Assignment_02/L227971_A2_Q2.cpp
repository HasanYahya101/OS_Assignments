#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
using namespace std;

string getline(int fd)
{
    char buffer;
    string line = "";

    while (read(fd, &buffer, 1) > 0)
    {
        if (buffer == '\n')
        {
            break; // Stop reading when newline is found
        }
        line += buffer; // Accumulate characters until newline
    }

    return line;
}

bool isdigit(char c)
{
    return c >= '0' && c <= '9';
}

bool is_nums_only(string str)
{
    for (int i = 0; i < str.length(); i++)
    {
        if (!isdigit(str[i]))
        {
            return false;
        }
    }
    return true;
}

int main(int argc, char *argv[])
{
    if (argc != 4) // 3 arguments
    {
        cerr << "Error: Invalid number of arguments.\n";
        return 1;
    }

    string input_file = argv[1];
    string output_file = argv[2];
    string error_file = argv[3];

    int fd_input = open(input_file.c_str(), O_RDONLY);
    if (fd_input == -1)
    {
        cerr << "Error: Cannot open input file.\n";
        return 1;
    }

    int fd_output = open(output_file.c_str(), O_WRONLY | O_CREAT, 0666);
    if (fd_output == -1)
    {
        cerr << "Error: Cannot open output file.\n";
        return 1;
    }

    int fd_error = open(error_file.c_str(), O_WRONLY | O_CREAT, 0666);
    if (fd_error == -1)
    {
        cerr << "Error: Cannot open error file.\n";
        return 1;
    }
    cout << "Redirecting output to " << output_file << endl;
    cerr << "Redirecting error to " << error_file << endl;

    int stdout_fd = dup(1);
    int stderr_fd = dup(2);

    dup2(fd_output, 1); // redirect
    dup2(fd_error, 2);  // redirect

    // now cout will write to output file and cerr will write to error file, you can also use write
    string line;
    string str = "";
    while ((line = getline(fd_input)) != "")
    {
        if (is_nums_only(line))
        {
            str = "Read number: " + line + "\n";
            cout << str;
            // write(1, str.c_str(), str.length());
            if (line.length() == 1 && line[0] == '0') // ie only 0
            {
                str = "Error: division by zero encountered\n";
                cerr << str;
                // write(2, str.c_str(), str.length());
                cout << str;
                // write(1, str.c_str(), str.length());
            }
            else
            {
                str = "Processed number: " + line + "\n";
                cout << str;
                // write(1, str.c_str(), str.length());
            }
        }
        else
        {
            str = "Error: Invalid number format for '" + line + "'\n";
            cerr << str;
            // write(2, str.c_str(), str.length());
        }
    }

    // remove redirection
    dup2(stdout_fd, 1);
    dup2(stderr_fd, 2);

    cout << "Output redirected to stdout\n";
    cerr << "Error redirected to stderr\n";

    close(fd_input);
    close(fd_output);
    close(fd_error);

    return 0;
}