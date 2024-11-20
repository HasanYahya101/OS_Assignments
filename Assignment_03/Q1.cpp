#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <math.h>
using namespace std;

struct shared_thread_data
{
    string shared_data;
    int *arr;
    float mean;
    float median;
    float standard_deviation;
    int arr_size;

    shared_thread_data() : shared_data(""), arr(nullptr), mean(0),
                           median(0), standard_deviation(0), arr_size(0) {}
};

string remove_duplicate_spaces(string data)
{
    // remove more then one spaces in a sequence
    string data_in_a_line_without_duplicate_spaces = "";
    for (int i = 0; i < data.length(); i++)
    {
        if (data[i] == ' ')
        {
            if (data[i + 1] != ' ')
            {
                data_in_a_line_without_duplicate_spaces += data[i];
            }
        }
        else
        {
            data_in_a_line_without_duplicate_spaces += data[i];
        }
    }
    return data_in_a_line_without_duplicate_spaces;
}

int total_numbers(string data)
{
    int count = 0;
    for (int i = 0; i < data.length(); i++)
    {
        if (data[i] == ' ')
        {
            count++;
        }
    }
    return count;
}

void *loading(void *arg)
{
    shared_thread_data *shared_data = (shared_thread_data *)arg;
    cout << "Data Loading Thread started" << endl;
    string input_file;
    cout << "Enter the input file name: ";
    cin >> input_file;

    while (input_file.find(".txt") == string::npos) // .txt not in name
    {
        cout << "Please enter a valid file name: ";
        cin >> input_file;
    }

    int fd = open(input_file.c_str(), O_RDONLY);

    if (fd == -1)
    {
        cout << "Error in opening file" << endl;
        pthread_exit(NULL);
    }

    string data_in_a_line = "";

    char data[1];

    while (read(fd, &data, 1) > 0)
    {
        if (data[0] == '\n')
        {
            data[0] = ' ';
        }
        data_in_a_line += data[0];
    }

    data_in_a_line = remove_duplicate_spaces(data_in_a_line);

    shared_data->shared_data = data_in_a_line;

    close(fd);

    cout << "Data Loading Thread finished" << endl;
    pthread_exit(NULL);
}

void *cleaning(void *arg)
{
    shared_thread_data *data = (shared_thread_data *)arg;
    cout << "Data Cleaning Thread started" << endl;
    string temp_data = data->shared_data;
    string to_remove = "-9999999";

    while (temp_data.find(to_remove) != string::npos)
    {
        temp_data.replace(temp_data.find(to_remove), to_remove.length(), "");
    }

    temp_data = remove_duplicate_spaces(temp_data);

    data->shared_data = temp_data;

    int n = total_numbers(temp_data) + 1;
    data->arr = new int[n];

    string temp_data_str = temp_data;
    int j = 0;

    size_t pos = 0;
    string token;
    while ((pos = temp_data_str.find(' ')) != string::npos)
    {
        token = temp_data_str.substr(0, pos);
        data->arr[j++] = stoi(token);
        temp_data_str.erase(0, pos + 1);
    }
    if (!temp_data_str.empty())
    {
        data->arr[j++] = stoi(temp_data_str);
    }

    // bubble sort
    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            if (data->arr[i] > data->arr[j])
            {
                int temp = data->arr[i];
                data->arr[i] = data->arr[j];
                data->arr[j] = temp;
            }
        }
    }

    // remove duplicates
    int *temp_arr = new int[n];
    int k = 0;
    temp_arr[k++] = data->arr[0];

    for (int i = 1; i < n; i++)
    {
        if (data->arr[i] != data->arr[i - 1])
        {
            temp_arr[k++] = data->arr[i];
        }
    }

    // resize array
    delete[] data->arr;
    data->arr = new int[k];
    for (int i = 0; i < k; i++)
    {
        data->arr[i] = temp_arr[i];
    }
    delete[] temp_arr;

    data->arr_size = k;

    cout << "Data Cleaning Thread finished" << endl;
    pthread_exit(NULL);
}

void *analysing(void *arg)
{
    shared_thread_data *data = (shared_thread_data *)arg;
    cout << "Data Analysing Thread started" << endl;
    int n = data->arr_size;

    int sum = 0;
    for (int i = 0; i < n; i++)
    {
        sum += data->arr[i];
    }
    data->mean = sum / (float)n;

    if (n % 2 == 0)
    {
        data->median = (data->arr[n / 2 - 1] + data->arr[n / 2]) / 2;
    }
    else
    {
        data->median = (float)data->arr[n / 2];
    }

    int sum_of_squares = 0;
    for (int i = 0; i < n; i++)
    {
        sum_of_squares += pow(data->arr[i] - data->mean, 2);
    }

    data->standard_deviation = sqrt(sum_of_squares / n);
    cout << "Data Analysing Thread finished" << endl;
    pthread_exit(NULL);
}

void *reporting(void *arg)
{
    shared_thread_data *data = (shared_thread_data *)arg;
    cout << "Data Reporting Thread started" << endl;
    string file = "report.txt";

    int fd = open(file.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);

    if (fd == -1)
    {
        cout << "Error in opening file" << endl;
        pthread_exit(NULL);
    }

    string first_line = "{'Cleaned Data': [\n";
    write(fd, first_line.c_str(), first_line.length());

    for (int i = 0; i < data->arr_size - 1; i++)
    {
        string data_to_write = to_string(data->arr[i]) + ",\n";
        write(fd, data_to_write.c_str(), data_to_write.length());
    }

    string last_data = to_string(data->arr[data->arr_size - 1]) + "\n],\n";
    write(fd, last_data.c_str(), last_data.length());

    string report_data = "'Mean': " + to_string(data->mean) + ",\n'Median': " +
                         to_string(data->median) + ",\n'Standard Deviation': " +
                         to_string(data->standard_deviation);

    write(fd, report_data.c_str(), report_data.length());

    string last_line = "\n}";
    write(fd, last_line.c_str(), last_line.length());

    close(fd);

    cout << "Data Reporting Thread finished" << endl;
    pthread_exit(NULL);
}

int main()
{
    pthread_t data_loading;
    pthread_t data_cleaning;
    pthread_t data_analysing;
    pthread_t data_reporting;

    shared_thread_data *shared_data = new shared_thread_data();

    pthread_create(&data_loading, NULL, loading, (void *)shared_data);
    pthread_join(data_loading, NULL);

    if (shared_data->shared_data == "")
    {
        cout << "Program terminated" << endl;
        delete shared_data;
        exit(1);
    }
    cout << "Data after loading: " << shared_data->shared_data << endl;

    pthread_create(&data_cleaning, NULL, cleaning, (void *)shared_data);
    pthread_join(data_cleaning, NULL);

    if (shared_data->shared_data == "" ||
        shared_data->shared_data.find("-9999999") != string::npos ||
        shared_data->arr == nullptr)
    {
        cout << "Error cleaning data! Program terminated" << endl;
        delete shared_data;
        exit(1);
    }

    cout << "Data after cleaning: ";
    for (int i = 0; i < shared_data->arr_size; i++)
    {
        cout << shared_data->arr[i] << " ";
    }
    cout << endl;

    pthread_create(&data_analysing, NULL, analysing, (void *)shared_data);
    pthread_join(data_analysing, NULL);

    cout << "Mean: " << shared_data->mean << endl;
    cout << "Median: " << shared_data->median << endl;
    cout << "Standard Deviation: " << shared_data->standard_deviation << endl;

    pthread_create(&data_reporting, NULL, reporting, (void *)shared_data);
    pthread_join(data_reporting, NULL);

    // clean
    delete[] shared_data->arr;
    delete shared_data;

    cout << "Program finished successfully" << endl;
    cout << "Exiting..." << endl;
    return 0;
}