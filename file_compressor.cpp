#include <bits/stdc++.h>
using namespace std;

struct Node
{
    char ch;
    int freq;
    Node *left, *right;
    Node(char c, int f, Node *l = nullptr, Node *r = nullptr)
        : ch(c), freq(f), left(l), right(r) {}
};

struct Compare
{
    bool operator()(Node *a, Node *b)
    {
        return a->freq > b->freq;
    }
};

void build_codes(Node *root, const string &prefix,
                 unordered_map<char, string> &codes)
{
    if (!root)
        return;
    if (!root->left && !root->right)
    {
        codes[root->ch] = prefix;
    }
    build_codes(root->left, prefix + '0', codes);
    build_codes(root->right, prefix + '1', codes);
}

void delete_tree(Node *root)
{
    if (!root)
        return;
    delete_tree(root->left);
    delete_tree(root->right);
    delete root;
}

int compress(const string &inFilename, const string &outFilename)
{
    ifstream in(inFilename, ios::binary);
    if (!in)
    {
        cerr << "Error opening input\n";
        exit(1);
    }
    string data((istreambuf_iterator<char>(in)), {});
    in.close();

    unordered_map<char, int> freq;
    for (char c : data)
        freq[c]++;

    priority_queue<Node *, vector<Node *>, Compare> pq;
    for (auto &p : freq)
        pq.push(new Node(p.first, p.second));

    while (pq.size() > 1)
    {
        Node *a = pq.top();
        pq.pop();
        Node *b = pq.top();
        pq.pop();
        pq.push(new Node('\0', a->freq + b->freq, a, b));
    }
    Node *root = pq.top();

    unordered_map<char, string> codes;
    build_codes(root, "", codes);

    ofstream out(outFilename, ios::binary);
    if (!out)
    {
        cerr << "Error opening output\n";
        exit(1);
    }

    for (auto &p : codes)
    {
        out << int((unsigned char)p.first)
            << ':' << p.second << '\n';
    }
    out << "===\n";
    int val = out.tellp();
    string bitstr;
    for (char c : data)
        bitstr += codes[c];

    uint8_t buffer = 0;
    int bitCount = 0;
    for (char b : bitstr)
    {
        buffer = (buffer << 1) | (b - '0');
        bitCount++;
        if (bitCount == 8)
        {
            out << buffer;
            buffer = 0;
            bitCount = 0;
        }
    }
    if (bitCount)
    {
        buffer <<= (8 - bitCount);
        out << buffer;
    }

    delete_tree(root);
    out.close();
    cout << "Compressed to " << outFilename << "\n";
    return val;
}

void decompress(const string &inFilename, const string &outFilename)
{
    ifstream in(inFilename, ios::binary);
    if (!in)
    {
        cerr << "Error opening input\n";
        exit(1);
    }

    unordered_map<string, char> rev;
    string line;
    while (getline(in, line))
    {
        if (line == "===")
            break;
        auto pos = line.find(':');
        int ci = stoi(line.substr(0, pos));
        string code = line.substr(pos + 1);
        rev[code] = char(ci);
    }

    string bitstr;
    char byte;
    while (in.get(byte))
    {
        uint8_t b = static_cast<uint8_t>(byte);
        for (int i = 7; i >= 0; --i)
            bitstr += ((b >> i) & 1) ? '1' : '0';
    }
    in.close();

    ofstream out(outFilename, ios::binary);
    if (!out)
    {
        cerr << "Error opening output\n";
        exit(1);
    }

    string cur;
    for (char bit : bitstr)
    {
        cur.push_back(bit);
        auto it = rev.find(cur);
        if (it != rev.end())
        {
            out << it->second;
            cur.clear();
        }
    }
    out.close();
    cout << "Decompressed to " << outFilename << "\n";
}
struct Symbol
{
    char ch;
    int freq;
    string code;
    Symbol(char c, int f) : ch(c), freq(f), code("") {}
};

int split(vector<Symbol> &symbols, int start, int end)
{
    int totalSum = 0;
    for (int i = start; i <= end; ++i)
        totalSum += symbols[i].freq;

    int halfSum = 0, splitIndex = start;
    for (int i = start; i <= end; ++i)
    {
        halfSum += symbols[i].freq;
        if (halfSum >= totalSum / 2)
        {
            splitIndex = i;
            break;
        }
    }
    return splitIndex;
}
void build_codes_shanon(vector<Symbol> &symbols, int start, int end)
{
    if (start >= end)
        return;

    int splitIndex = split(symbols, start, end);

    for (int i = start; i <= splitIndex; ++i)
        symbols[i].code += '0';
    for (int i = splitIndex + 1; i <= end; ++i)
        symbols[i].code += '1';

    build_codes_shanon(symbols, start, splitIndex);
    build_codes_shanon(symbols, splitIndex + 1, end);
}
int compress_shanon(const string &inFilename, const string &outFilename)
{
    ifstream in(inFilename, ios::binary);
    if (!in)
    {
        cerr << "Error opening input\n";
        exit(1);
    }

    string data((istreambuf_iterator<char>(in)), {});
    in.close();

    unordered_map<char, int> freq;
    for (char c : data)
        freq[c]++;

    vector<Symbol> symbols;
    for (auto &p : freq)
        symbols.push_back(Symbol(p.first, p.second));

    sort(symbols.begin(), symbols.end(), [](const Symbol &a, const Symbol &b)
         { return a.freq > b.freq; });

    build_codes_shanon(symbols, 0, symbols.size() - 1);

    unordered_map<char, string> codes;
    for (const auto &sym : symbols)
        codes[sym.ch] = sym.code;

    ofstream out(outFilename, ios::binary);
    if (!out)
    {
        cerr << "Error opening output\n";
        exit(1);
    }

    for (const auto &sym : symbols)
    {
        out << int((unsigned char)sym.ch) << ':' << sym.code << '\n';
    }
    out << "===\n";
    int val = out.tellp();
    string bitstr;
    for (char c : data)
        bitstr += codes[c];

    uint8_t buffer = 0;
    int bitCount = 0;
    for (char b : bitstr)
    {
        buffer = (buffer << 1) | (b - '0');
        bitCount++;
        if (bitCount == 8)
        {
            out << buffer;
            buffer = 0;
            bitCount = 0;
        }
    }
    if (bitCount)
    {
        buffer <<= (8 - bitCount);
        out << buffer;
    }

    out.close();
    cout << "Compressed to " << outFilename << "\n";
    return val;
}
void decompress_shanon(const string &inFilename, const string &outFilename)
{
    ifstream in(inFilename, ios::binary);
    if (!in)
    {
        cerr << "Error opening input\n";
        exit(1);
    }

    unordered_map<string, char> rev;
    string line;
    while (getline(in, line))
    {
        if (line == "===")
            break;
        auto pos = line.find(':');
        int ci = stoi(line.substr(0, pos));
        string code = line.substr(pos + 1);
        rev[code] = char(ci);
    }

    string bitstr;
    char byte;
    while (in.get(byte))
    {
        uint8_t b = static_cast<uint8_t>(byte);
        for (int i = 7; i >= 0; --i)
            bitstr += ((b >> i) & 1) ? '1' : '0';
    }
    in.close();

    ofstream out(outFilename, ios::binary);
    if (!out)
    {
        cerr << "Error opening output\n";
        exit(1);
    }

    string cur;
    for (char bit : bitstr)
    {
        cur.push_back(bit);
        auto it = rev.find(cur);
        if (it != rev.end())
        {
            out << it->second;
            cur.clear();
        }
    }

    out.close();
    cout << "Decompressed to " << outFilename << "\n";
}

int getFileSize(const string &filename)
{
    ifstream file(filename, ios::binary | ios::ate);
    if (!file)
    {
        cerr << "Error opening file: " << filename << endl;
        return 0;
    }
    return file.tellg();
}
void displayMenu()
{
    cout << "========================================" << endl;
    cout << "         FILE COMPRESSION TOOL          " << endl;
    cout << "========================================" << endl;
    cout << "1. Compression" << endl;
    cout << "2. Decompression" << endl;
    cout << "0. Exit" << endl;
    cout << "========================================" << endl;
    cout << "Enter your choice: ";
}

void displayCompressionMenu()
{
    cout << "\n----------------------------------------" << endl;
    cout << "         COMPRESSION OPTIONS            " << endl;
    cout << "----------------------------------------" << endl;
    cout << "1. Huffman Compression" << endl;
    cout << "2. Shannon-Fano Compression" << endl;
    cout << "----------------------------------------" << endl;
    cout << "Enter your choice: ";
}

void displayDecompressionMenu()
{
    cout << "\n----------------------------------------" << endl;
    cout << "        DECOMPRESSION OPTIONS           " << endl;
    cout << "----------------------------------------" << endl;
    cout << "1. Huffman Decompression" << endl;
    cout << "2. Shannon-Fano Decompression" << endl;
    cout << "----------------------------------------" << endl;
    cout << "Enter your choice: ";
}

int main()
{
    int t = 1;
    while (t)
    {
        displayMenu();
        int ch;
        cin >> ch;

        if (ch == 0)
        {
            cout << "\nExiting... Thank you for using the tool!" << endl;
            break;
        }

        else if (ch == 1)
        {
            string in, out;
            cout << "\nEnter input file name: ";
            cin >> in;
            cout << "Enter output file name: ";
            cin >> out;

            displayCompressionMenu();
            int a;
            cin >> a;

            if (a != 1 && a != 2)
            {
                cout << "Invalid choice. Returning to main menu.\n"
                     << endl;
                continue;
            }

            int val;
            if (a == 1)
                val = compress(in, out);
            else
                val = compress_shanon(in, out);

            int compressedSize = getFileSize(out);
            int originalSize = getFileSize(in);

            cout << "\n========================================" << endl;
            cout << "           COMPRESSION RESULTS          " << endl;
            cout << "----------------------------------------" << endl;
            cout << "Original Size      : " << originalSize << " bytes" << endl;
            cout << "Compressed Size    : " << compressedSize << " bytes" << endl;
            cout << fixed << setprecision(2);
            cout << "Compression Ratio  : "
                 << (float)originalSize / compressedSize << endl;
            cout << "========================================\n"
                 << endl;
        }

        else if (ch == 2)
        {
            string in, out;
            cout << "\nEnter input file name: ";
            cin >> in;
            cout << "Enter output file name: ";
            cin >> out;

            displayDecompressionMenu();
            int a;
            cin >> a;

            if (a == 1)
                decompress(in, out);
            else if (a == 2)
                decompress_shanon(in, out);
            else
                cout << "Invalid choice. Returning to main menu.\n"
                     << endl;
        }

        else
        {
            cout << "Invalid choice. Please try again.\n"
                 << endl;
        }

        cout << "\n========================================\n\n";
    }

    return 0;
}