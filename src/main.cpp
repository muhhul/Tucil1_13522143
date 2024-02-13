#include <iostream>
#include <cstdio>
#include <filesystem>
#include <string>
#include <vector>
#include <sstream>
#include <chrono>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <random>

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#define GetCurrentDir _getcwd
#define ChangeDir _chdir
#define FileSeparator '\\'
#define clearCLI "cls"
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#define ChangeDir chdir
#define FileSeparator '/'
#define clearCLI "clear"
#endif

using namespace std;

// struct
struct Answer
{
    int reward, duration, *arrRewardSequences, amountSequences;
    string buffer;
    vector<string> arrKordinat, arrToken;
    vector<vector<string>> matrix;
};

// Fungsi untuk memindahkan isi file ke dalam vektor yang bertipe string
vector<string> fileToVector(string fileName)
{
    vector<string> lines;
    char currentDir[FILENAME_MAX];
    if (!GetCurrentDir(currentDir, sizeof(currentDir)))
    {
        cout << "Gagal mendapatkan path\n";
    }

    string pathToFile = "../test";

    if (ChangeDir(pathToFile.c_str()) != 0)
    {
        cout << "Gagal mengubah path ke test: " << pathToFile << '\n';
    }

    filesystem::path fullPath = filesystem::absolute(filesystem::path(fileName));

    FILE *inputFile = fopen(fullPath.string().c_str(), "rt");

    if (!inputFile)
    {
        cout << "Gagal membuka file: " << fullPath << '\n';
    }

    char buffer[100];
    while (fgets(buffer, sizeof(buffer), inputFile))
    {
        buffer[strcspn(buffer, "\n")] = '\0';
        lines.push_back(buffer);
    }
    fclose(inputFile);

    return lines;
}

void saveFile(Answer answer)
{
    cout << "Apakah ingin menyimpan solusi? (y/n)" << endl;
    string inputUser;
    cin >> inputUser;

    if (inputUser == "y" || inputUser == "Y")
    {
        cout << "Masukkan nama file yang ingin kamu simpan: ";
        cin >> inputUser;
        inputUser = inputUser + ".txt";
        vector<string> lines;
        char currentDir[FILENAME_MAX];
        if (!GetCurrentDir(currentDir, sizeof(currentDir)))
        {
            cout << "Gagal mendapatkan path\n";
        }

        string pathToFile = "../test";

        if (ChangeDir(pathToFile.c_str()) != 0)
        {
            cout << "Gagal mengubah path ke test: " << pathToFile << '\n';
        }

        filesystem::path fullPath = filesystem::absolute(filesystem::path(inputUser));

        ofstream outputFile(fullPath);
        if (outputFile.is_open())
        {
            outputFile << "Matriks yang digunakan" << endl;
            int rows = answer.matrix.size();
            int cols = 0;
            if (rows > 0)
            {
                cols = answer.matrix[0].size();
            }
            for (size_t i = 0; i < rows; ++i)
            {
                for (size_t j = 0; j < cols; ++j)
                {
                    outputFile << answer.matrix[i][j] << " ";
                }
                outputFile << endl;
            }
            outputFile << endl;

            outputFile << "Sekuens yang digunakan" << endl;
            for (int i = 0; i < answer.amountSequences; i++)
            {
                outputFile << answer.arrToken[i] << endl;
                outputFile << answer.arrRewardSequences[i] << endl;
            }
            outputFile << endl;

            outputFile << "Total bobot hadiah : " << answer.reward << endl;
            outputFile << "Buffer : " << answer.buffer << endl;
            int j = 0;
            for (int i = 0; i < answer.arrKordinat.size(); i++)
            {
                int row = answer.arrKordinat[i][0] - '0' + 1;
                int col = answer.arrKordinat[i][1] - '0' + 1;
                outputFile << answer.buffer[j] << answer.buffer[j + 1] << " Terletak pada : " << col << ", " << row << endl;
                j = j + 3;
            }
            outputFile << endl;
            outputFile << "Duarsi yang dibutuhkan : " << answer.duration << " ms" << endl;
            outputFile.close();
            cout << "File berhasil disimpan di: " << fullPath << '\n';
        }
        else
        {
            cout << "Gagal membuka file untuk ditulis." << endl;
        }
    }
}

bool cekKordinat(const string &newKordinat, const vector<string> &listKordinat)
{
    bool cek = false;
    for (const string &kordinat : listKordinat)
    {
        if (kordinat == newKordinat)
        {
            cek = true;
            break;
        }
    }
    return cek;
}

int getReward(int *arrReward, vector<string> arrToken, string buffer)
{
    int reward = -1;
    for (int k = 0; k < arrToken.size(); k++)
    {
        int count = 0;
        size_t lengthToken = arrToken[k].length();
        size_t lengthBuffer = buffer.length();
        if (lengthBuffer >= lengthToken)
        {
            for (size_t i = 0; i <= lengthBuffer - lengthToken; i++)
            {
                bool match = true;
                for (size_t j = 0; j < lengthToken; j++)
                {
                    if (buffer[i + j] != arrToken[k][j])
                    {
                        match = false;
                        break;
                    }
                }
                if (match)
                {
                    count = 1;
                }
            }
        }
        reward = reward + arrReward[k] * count;
    }

    if (reward > 0)
    {
        return reward + 1;
    }
    return reward;
}

vector<vector<string>> createRandomMatrix(int col, int row, string token, int amountToken)
{
    vector<string> arrToken;
    vector<vector<string>> matrix;
    for (int i = 0; i < amountToken * 3; i++)
    {
        string temp1(1, token[i]);
        string temp2(1, token[i + 1]);
        string temp = temp1 + temp2;
        arrToken.push_back(temp);
        i++;
        i++;
    }

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distribution(0, arrToken.size() - 1);
    matrix.resize(row, std::vector<std::string>(col));
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < col; j++)
        {
            int random = distribution(gen);
            matrix[i][j] = arrToken[random];
        }
    }

    return matrix;
}

vector<string> createRandomSequens(int maxSequences, int amountSequences, string token, int amountToken)
{
    vector<string> arrToken;
    vector<vector<string>> matrix;
    for (int i = 0; i < amountToken * 3; i++)
    {
        string temp1(1, token[i]);
        string temp2(1, token[i + 1]);
        string temp = temp1 + temp2;
        arrToken.push_back(temp);
        i++;
        i++;
    }

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distribution(2, maxSequences);
    uniform_int_distribution<> distribution2(0, arrToken.size() - 1);
    vector<string> arrSequens;
    for (int i = 0; i < amountSequences; i++)
    {
        string temp = "";
        int random = distribution(gen);
        for (int j = 0; j < random; j++)
        {
            int random = distribution2(gen);
            temp = temp + arrToken[random];
            temp = temp + " ";
        }
        arrSequens.push_back(temp);
    }

    return arrSequens;
}

void createRandomReward(int amountSequences, int arrRewardSequences[])
{
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distribution(10, 50);
    for (int i = 0; i < amountSequences; i++)
    {
        int random = distribution(gen);
        arrRewardSequences[i] = random;
    }
}

Answer getAnswer(int col, int row, int buffSize, const vector<vector<string>> &matriks, int *arrReward, vector<string> arrToken)
{
    Answer answer;
    answer.reward = -1;
    string tempBuffer;
    vector<string> tempKordinat;

    for (int i = 0; i < col; i++)
    {
        tempBuffer = matriks[0][i];
        string tempString = "0" + to_string(i);
        tempKordinat.push_back(tempString);
        bool cek = false;
        for (int j = 0; j < row; j++)
        {
            string tempBuffer1 = tempBuffer + " " + matriks[j][i];
            tempString = to_string(j) + to_string(i);
            cek = cekKordinat(tempString, tempKordinat);
            if (!cek)
            {
                int reward = getReward(arrReward, arrToken, tempBuffer1);
                tempKordinat.push_back(tempString);
                if (reward > answer.reward)
                {
                    answer.reward = reward;
                    answer.buffer = tempBuffer1;
                    answer.arrKordinat = tempKordinat;
                }
                if (buffSize > 2)
                {
                    for (int k = 0; k < col; k++)
                    {
                        string tempBuffer2 = tempBuffer1 + " " + matriks[j][k];
                        tempString = to_string(j) + to_string(k);
                        cek = cekKordinat(tempString, tempKordinat);
                        if (!cek)
                        {
                            int reward = getReward(arrReward, arrToken, tempBuffer2);
                            tempKordinat.push_back(tempString);
                            if (reward > answer.reward)
                            {
                                answer.reward = reward;
                                answer.buffer = tempBuffer2;
                                answer.arrKordinat = tempKordinat;
                            }
                            if (buffSize > 3)
                            {
                                for (int l = 0; l < row; l++)
                                {
                                    string tempBuffer3 = tempBuffer2 + " " + matriks[l][k];
                                    tempString = to_string(l) + to_string(k);
                                    cek = cekKordinat(tempString, tempKordinat);
                                    if (!cek)
                                    {
                                        int reward = getReward(arrReward, arrToken, tempBuffer3);
                                        tempKordinat.push_back(tempString);
                                        if (reward > answer.reward)
                                        {
                                            answer.reward = reward;
                                            answer.buffer = tempBuffer3;
                                            answer.arrKordinat = tempKordinat;
                                        }
                                        if (buffSize > 4)
                                        {
                                            for (int m = 0; m < col; m++)
                                            {
                                                string tempBuffer4 = tempBuffer3 + " " + matriks[l][m];
                                                tempString = to_string(l) + to_string(m);
                                                cek = cekKordinat(tempString, tempKordinat);
                                                if (!cek)
                                                {
                                                    int reward = getReward(arrReward, arrToken, tempBuffer4);
                                                    tempKordinat.push_back(tempString);
                                                    if (reward > answer.reward)
                                                    {
                                                        answer.reward = reward;
                                                        answer.buffer = tempBuffer4;
                                                        answer.arrKordinat = tempKordinat;
                                                    }
                                                    if (buffSize > 5)
                                                    {
                                                        for (int n = 0; n < row; n++)
                                                        {
                                                            string tempBuffer5 = tempBuffer4 + " " + matriks[n][m];
                                                            tempString = to_string(n) + to_string(m);
                                                            cek = cekKordinat(tempString, tempKordinat);
                                                            if (!cek)
                                                            {
                                                                int reward = getReward(arrReward, arrToken, tempBuffer5);
                                                                tempKordinat.push_back(tempString);
                                                                if (reward > answer.reward)
                                                                {
                                                                    answer.reward = reward;
                                                                    answer.buffer = tempBuffer5;
                                                                    answer.arrKordinat = tempKordinat;
                                                                }
                                                                if (buffSize > 6)
                                                                {
                                                                    for (int o = 0; o < col; o++)
                                                                    {
                                                                        string tempBuffer6 = tempBuffer5 + " " + matriks[n][o];
                                                                        tempString = to_string(n) + to_string(o);
                                                                        cek = cekKordinat(tempString, tempKordinat);
                                                                        if (!cek)
                                                                        {
                                                                            int reward = getReward(arrReward, arrToken, tempBuffer6);
                                                                            tempKordinat.push_back(tempString);
                                                                            if (reward > answer.reward)
                                                                            {
                                                                                answer.reward = reward;
                                                                                answer.buffer = tempBuffer6;
                                                                                answer.arrKordinat = tempKordinat;
                                                                            }
                                                                            if (buffSize > 7)
                                                                            {
                                                                                for (int p = 0; p < row; p++)
                                                                                {
                                                                                    string tempBuffer7 = tempBuffer6 + " " + matriks[p][o];
                                                                                    tempString = to_string(p) + to_string(o);
                                                                                    cek = cekKordinat(tempString, tempKordinat);
                                                                                    if (!cek)
                                                                                    {
                                                                                        int reward = getReward(arrReward, arrToken, tempBuffer7);
                                                                                        tempKordinat.push_back(tempString);
                                                                                        if (reward > answer.reward)
                                                                                        {
                                                                                            answer.reward = reward;
                                                                                            answer.buffer = tempBuffer7;
                                                                                            answer.arrKordinat = tempKordinat;
                                                                                        }
                                                                                        if (buffSize > 8)
                                                                                        {
                                                                                            for (int q = 0; q < col; q++)
                                                                                            {
                                                                                                string tempBuffer8 = tempBuffer7 + " " + matriks[p][q];
                                                                                                tempString = to_string(p) + to_string(q);
                                                                                                cek = cekKordinat(tempString, tempKordinat);
                                                                                                if (!cek)
                                                                                                {
                                                                                                    int reward = getReward(arrReward, arrToken, tempBuffer8);
                                                                                                    tempKordinat.push_back(tempString);
                                                                                                    if (reward > answer.reward)
                                                                                                    {
                                                                                                        answer.reward = reward;
                                                                                                        answer.buffer = tempBuffer8;
                                                                                                        answer.arrKordinat = tempKordinat;
                                                                                                    }
                                                                                                    if (buffSize > 9)
                                                                                                    {
                                                                                                        for (int r = 0; r < row; r++)
                                                                                                        {
                                                                                                            string tempBuffer9 = tempBuffer8 + " " + matriks[r][q];
                                                                                                            tempString = to_string(r) + to_string(q);
                                                                                                            cek = cekKordinat(tempString, tempKordinat);
                                                                                                            if (!cek)
                                                                                                            {
                                                                                                                int reward = getReward(arrReward, arrToken, tempBuffer9);
                                                                                                                tempKordinat.push_back(tempString);
                                                                                                                if (reward > answer.reward)
                                                                                                                {
                                                                                                                    answer.reward = reward;
                                                                                                                    answer.buffer = tempBuffer9;
                                                                                                                    answer.arrKordinat = tempKordinat;
                                                                                                                }
                                                                                                                tempKordinat.pop_back();
                                                                                                            }
                                                                                                        }
                                                                                                    }
                                                                                                    tempKordinat.pop_back();
                                                                                                }
                                                                                            }
                                                                                        }
                                                                                        tempKordinat.pop_back();
                                                                                    }
                                                                                }
                                                                            }
                                                                            tempKordinat.pop_back();
                                                                        }
                                                                    }
                                                                }
                                                                tempKordinat.pop_back();
                                                            }
                                                        }
                                                    }
                                                    tempKordinat.pop_back();
                                                }
                                            }
                                        }
                                        tempKordinat.pop_back();
                                    }
                                }
                            }
                            tempKordinat.pop_back();
                        }
                    }
                }
                tempKordinat.pop_back();
            }
        }
        tempKordinat.clear();
    }
    return answer;
}

int main()
{

    // Variabel
    int bufferSize, matCol, matRow, amountToken, amountSequences, maxSequences, arrRewardSequences[100];
    string token;
    vector<vector<string>> matrix(100, vector<string>(100));
    vector<string> arrToken;

    // Menu dan memilih input permainan
    cout << "Selamat datang di Tucil 1\n";
    cout << "Silahkan pilih input yang ingin kamu masukkan\n";
    cout << "1. Masukkan lewat file.txt\n";
    cout << "2. Masukkan lewat CLI\n";

    int inputUser;
    cout << ">> ";
    cin >> inputUser;

    bool inputUserCheck = false;
    while (!inputUserCheck)
    {
        if (inputUser == 1)
        {
            inputUserCheck = true;
            cout << "Masukkan nama file yang ingin kamu muat: ";
            string inputUserString;
            cin >> inputUserString;
            inputUserString = inputUserString + ".txt";
            vector<string> dataFromFile = fileToVector(inputUserString);

            istringstream issBufferSize(dataFromFile[0]);
            issBufferSize >> bufferSize;

            istringstream issMatrix(dataFromFile[1]);
            issMatrix >> matCol >> matRow;

            matrix.resize(matRow, std::vector<std::string>(matCol));
            for (int i = 0; i < matRow; i++)
            {
                int k = 0;
                for (int j = 0; j < matCol * 3 - 1; j++)
                {
                    char temp1 = dataFromFile[2 + i][j];
                    char temp2 = dataFromFile[2 + i][j + 1];
                    string temp3(1, temp1);
                    string temp4(1, temp2);
                    string temp = temp3 + temp4;
                    matrix[i][k] = temp;
                    j++;
                    j++;
                    k++;
                }
            }

            istringstream issAmSeq(dataFromFile[matRow + 2]);
            issAmSeq >> amountSequences;

            for (int i = 0; i < amountSequences; i++)
            {
                arrToken.push_back(dataFromFile[matRow + 3 + i * 2]);
            }

            for (int i = 1; i <= amountSequences; i++)
            {
                istringstream issReward(dataFromFile[matRow + 2 + i * 2]);
                issReward >> arrRewardSequences[i - 1];
            }
            system(clearCLI);
        }
        else if (inputUser == 2)
        {
            inputUserCheck = true;

            cout << "Masukkan jumlah token unik : ";
            cin >> amountToken;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            cout << "Masukkan token : ";
            getline(cin, token);
            cout << "Masukkan ukuran buffer : ";
            cin >> bufferSize;
            cout << "Masukkan ukuran matriks : \n";
            cout << "Ukuran kolom : ";
            cin >> matCol;
            cout << "Ukuran baris : ";
            cin >> matRow;
            cout << "Masukkan jumlah sekuens : ";
            cin >> amountSequences;
            cout << "Masukkan ukuran maksimal sekuens : ";
            cin >> maxSequences;

            matrix.resize(matRow, std::vector<std::string>(matCol));
            system(clearCLI);
            matrix = createRandomMatrix(matCol, matRow, token, amountToken);
            arrToken = createRandomSequens(maxSequences, amountSequences, token, amountToken);
            createRandomReward(amountSequences, arrRewardSequences);
        }
        else
        {
            system(clearCLI);
            cout << "Masukkan kamu salah, silahkan masukkan lagi\n";
            cout << "1. Masukkan lewat file.txt\n";
            cout << "2. Masukkan lewat CLI\n";
            cout << ">> ";
            cin >> inputUser;
        }
    }

    Answer answer;
    auto start_time = chrono::high_resolution_clock::now();
    answer = getAnswer(matCol, matRow, bufferSize, matrix, arrRewardSequences, arrToken);
    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);
    answer.duration = static_cast<int>(duration.count());
    answer.matrix = matrix;
    answer.amountSequences = amountSequences;
    answer.arrToken = arrToken;
    answer.arrRewardSequences = arrRewardSequences;

    cout << "Matriks yang digunakan" << endl;
    int rows = answer.matrix.size();
    int cols = 0;
    if (rows > 0)
    {
        cols = answer.matrix[0].size();
    }
    for (size_t i = 0; i < rows; ++i)
    {
        for (size_t j = 0; j < cols; ++j)
        {
            cout << answer.matrix[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;

    cout << "Sekuens yang digunakan" << endl;
    for (int i = 0; i < answer.amountSequences; i++)
    {
        cout << answer.arrToken[i] << endl;
        cout << answer.arrRewardSequences[i] << endl;
    }
    cout << endl;

    if (answer.reward == -1)
    {
        cout << "Tidak ditemukan sekuens yang memenuhi" << endl;
    }
    else
    {
        cout << "Total bobot hadiah : " << answer.reward << endl;
        cout << "Buffer : " << answer.buffer << endl;
        int j = 0;
        for (int i = 0; i < answer.arrKordinat.size(); i++)
        {
            int row = answer.arrKordinat[i][0] - '0' + 1;
            int col = answer.arrKordinat[i][1] - '0' + 1;
            cout << answer.buffer[j] << answer.buffer[j + 1] << " Terletak pada : " << col << ", " << row << endl;
            j = j + 3;
        }
        cout << endl;
        cout << "Duarsi yang dibutuhkan : " << duration.count() << " ms" << endl;
    }

    saveFile(answer);
    return 0;
}
