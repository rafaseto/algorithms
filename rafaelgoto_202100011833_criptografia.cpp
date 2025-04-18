#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <thread>

using namespace std;

static const bool ios_sync_off = [](){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    return true;
}();

inline int myMin(int a, int b) { return (a < b) ? a : b; }

// ===== IMPLEMENTAÇÃO DE BIGINT (operações com números grandes) =====
struct BigInt {
    // Número representado em little‑endian: word0 é o LSB.
    unsigned int words[130];
    int size; // número de palavras utilizadas (mínimo 1)
};

inline void initBigInt(BigInt &a) {
    memset(a.words, 0, sizeof(a.words));
    a.size = 1;
}

inline void fromHex(BigInt &a, const char *hex) {
    initBigInt(a);
    int len = (int)strlen(hex);
    a.size = (len + 7) / 8;
    for (int i = 0; i < a.size; i++)
        a.words[i] = 0;
    int pos = len;
    for (int i = 0; i < a.size; i++) {
        int start = (pos - 8 < 0) ? 0 : pos - 8;
        int chunkSize = pos - start;
        char buf[9];
        memcpy(buf, hex + start, chunkSize);
        buf[chunkSize] = '\0';
        a.words[i] = (unsigned int)strtoul(buf, nullptr, 16);
        pos = start;
    }
    while(a.size > 1 && a.words[a.size - 1] == 0)
        a.size--;
}

string toHex(const BigInt &a, int expectedHex) {
    int wordsNeeded = (expectedHex + 7) / 8;
    string result;
    result.reserve(wordsNeeded * 8);
    for (int i = wordsNeeded - 1; i >= 0; i--) {
        char buf[9];
        int width = (i == wordsNeeded - 1 && (expectedHex % 8 != 0)) ? (expectedHex % 8) : 8;
        unsigned int w = (i < a.size ? a.words[i] : 0);
        sprintf(buf, "%0*X", width, w);
        result += buf;
    }
    return result;
}

inline int compareBigInt(const BigInt &a, const BigInt &b) {
    if(a.size > b.size) return 1;
    if(a.size < b.size) return -1;
    for (int i = a.size - 1; i >= 0; i--) {
        if(a.words[i] > b.words[i]) return 1;
        if(a.words[i] < b.words[i]) return -1;
    }
    return 0;
}

inline void addBigInt(BigInt &res, const BigInt &a, const BigInt &b) {
    int maxSize = (a.size > b.size) ? a.size : b.size;
    unsigned long long carry = 0;
    for (int i = 0; i < maxSize; i++){
        unsigned long long sum = carry;
        if(i < a.size)
            sum += a.words[i];
        if(i < b.size)
            sum += b.words[i];
        res.words[i] = (unsigned int)(sum & 0xFFFFFFFFUL);
        carry = sum >> 32;
    }
    res.size = maxSize;
    if(carry) {
        res.words[res.size] = (unsigned int)carry;
        res.size++;
    }
}

inline void subtractBigInt(BigInt &res, const BigInt &a, const BigInt &b) {
    unsigned long long borrow = 0;
    res.size = a.size;
    for(int i = 0; i < a.size; i++){
        unsigned long long sub = (unsigned long long)a.words[i] - ((i < b.size) ? b.words[i] : 0) - borrow;
        res.words[i] = (unsigned int)(sub & 0xFFFFFFFFUL);
        borrow = (a.words[i] < ((i < b.size) ? b.words[i] : 0) + borrow) ? 1 : 0;
    }
    while(res.size > 1 && res.words[res.size - 1] == 0)
        res.size--;
}

inline void multiplyBigInt(BigInt &res, const BigInt &a, const BigInt &b) {
    unsigned long long temp[130] = {0};
    int resSize = a.size + b.size;
    for (int i = 0; i < a.size; i++){
        unsigned long long carry = 0;
        for (int j = 0; j < b.size; j++){
            unsigned long long mul = (unsigned long long)a.words[i] * b.words[j] + temp[i+j] + carry;
            temp[i+j] = (unsigned int)(mul & 0xFFFFFFFFUL);
            carry = mul >> 32;
        }
        temp[i + b.size] = carry;
    }
    res.size = resSize;
    while (res.size > 1 && temp[res.size - 1] == 0)
        res.size--;
    for (int i = 0; i < res.size; i++)
        res.words[i] = (unsigned int)temp[i];
}

inline int getBit(const BigInt &a, int pos) {
    int word = pos / 32;
    int bit = pos % 32;
    if(word >= a.size) return 0;
    return (a.words[word] >> bit) & 1;
}

inline void shiftLeftOne(BigInt &a) {
    unsigned int carry = 0;
    for (int i = 0; i < a.size; i++){
        unsigned long long shifted = ((unsigned long long)a.words[i] << 1) | carry;
        a.words[i] = (unsigned int)(shifted & 0xFFFFFFFFUL);
        carry = (unsigned int)(shifted >> 32);
    }
    if(carry){
        a.words[a.size] = carry;
        a.size++;
    }
}

inline void shiftRightOne(BigInt &a) {
    unsigned int carry = 0;
    for (int i = a.size - 1; i >= 0; i--){
        unsigned int newCarry = a.words[i] & 1;
        a.words[i] = (a.words[i] >> 1) | (carry << 31);
        carry = newCarry;
    }
    if(a.size > 1 && a.words[a.size - 1] == 0)
        a.size--;
}

inline bool isZero(const BigInt &a) {
    return (a.size == 1 && a.words[0] == 0);
}

void modBigInt(const BigInt &a, const BigInt &d, BigInt &r) {
    BigInt rem;
    initBigInt(rem);
    int bitLen = a.size * 32;
    while(bitLen > 0 && getBit(a, bitLen - 1) == 0)
        bitLen--;
    for (int i = bitLen - 1; i >= 0; i--) {
        shiftLeftOne(rem);
        if(getBit(a, i))
            rem.words[0] |= 1;
        if(compareBigInt(rem, d) >= 0) {
            BigInt temp;
            subtractBigInt(temp, rem, d);
            rem = temp;
        }
    }
    r = rem;
}

void modMultiplyBigInt(BigInt &res, const BigInt &a, const BigInt &b, const BigInt &mod) {
    BigInt result, A, B;
    initBigInt(result);
    A = a; modBigInt(A, mod, A);
    B = b;
    int totalBits = B.size * 32;
    for (int i = 0; i < totalBits; i++) {
        if(getBit(B, 0)) {
            BigInt temp;
            addBigInt(temp, result, A);
            if(compareBigInt(temp, mod) >= 0) {
                BigInt temp2;
                subtractBigInt(temp2, temp, mod);
                result = temp2;
            } else {
                result = temp;
            }
        }
        {
            BigInt temp;
            addBigInt(temp, A, A);
            if(compareBigInt(temp, mod) >= 0) {
                BigInt temp2;
                subtractBigInt(temp2, temp, mod);
                A = temp2;
            } else {
                A = temp;
            }
        }
        shiftRightOne(B);
        if(isZero(B))
            break;
    }
    res = result;
}

void modExpBigInt(BigInt &res, const BigInt &base, const BigInt &exponent, const BigInt &mod) {
    BigInt result;
    initBigInt(result);
    result.words[0] = 1;
    BigInt baseCopy = base;
    modBigInt(baseCopy, mod, baseCopy);
    int expBits = exponent.size * 32;
    int bitLen = expBits;
    while(bitLen > 0 && getBit(exponent, bitLen - 1) == 0)
        bitLen--;
    for (int i = bitLen - 1; i >= 0; i--) {
        BigInt temp;
        modMultiplyBigInt(temp, result, result, mod);
        result = temp;
        if(getBit(exponent, i)) {
            modMultiplyBigInt(temp, result, baseCopy, mod);
            result = temp;
        }
    }
    res = result;
}

// ===== IMPLEMENTAÇÃO DO AES (ECB) =====

static const unsigned char sbox[256] = {
  0x63,0x7c,0x77,0x7b,0xf2,0x6b,0x6f,0xc5,0x30,0x01,0x67,0x2b,0xfe,0xd7,0xab,0x76,
  0xca,0x82,0xc9,0x7d,0xfa,0x59,0x47,0xf0,0xad,0xd4,0xa2,0xaf,0x9c,0xa4,0x72,0xc0,
  0xb7,0xfd,0x93,0x26,0x36,0x3f,0xf7,0xcc,0x34,0xa5,0xe5,0xf1,0x71,0xd8,0x31,0x15,
  0x04,0xc7,0x23,0xc3,0x18,0x96,0x05,0x9a,0x07,0x12,0x80,0xe2,0xeb,0x27,0xb2,0x75,
  0x09,0x83,0x2c,0x1a,0x1b,0x6e,0x5a,0xa0,0x52,0x3b,0xd6,0xb3,0x29,0xe3,0x2f,0x84,
  0x53,0xd1,0x00,0xed,0x20,0xfc,0xb1,0x5b,0x6a,0xcb,0xbe,0x39,0x4a,0x4c,0x58,0xcf,
  0xd0,0xef,0xaa,0xfb,0x43,0x4d,0x33,0x85,0x45,0xf9,0x02,0x7f,0x50,0x3c,0x9f,0xa8,
  0x51,0xa3,0x40,0x8f,0x92,0x9d,0x38,0xf5,0xbc,0xb6,0xda,0x21,0x10,0xff,0xf3,0xd2,
  0xcd,0x0c,0x13,0xec,0x5f,0x97,0x44,0x17,0xc4,0xa7,0x7e,0x3d,0x64,0x5d,0x19,0x73,
  0x60,0x81,0x4f,0xdc,0x22,0x2a,0x90,0x88,0x46,0xee,0xb8,0x14,0xde,0x5e,0x0b,0xdb,
  0xe0,0x32,0x3a,0x0a,0x49,0x06,0x24,0x5c,0xc2,0xd3,0xac,0x62,0x91,0x95,0xe4,0x79,
  0xe7,0xc8,0x37,0x6d,0x8d,0xd5,0x4e,0xa9,0x6c,0x56,0xf4,0xea,0x65,0x7a,0xae,0x08,
  0xba,0x78,0x25,0x2e,0x1c,0xa6,0xb4,0xc6,0xe8,0xdd,0x74,0x1f,0x4b,0xbd,0x8b,0x8a,
  0x70,0x3e,0xb5,0x66,0x48,0x03,0xf6,0x0e,0x61,0x35,0x57,0xb9,0x86,0xc1,0x1d,0x9e,
  0xe1,0xf8,0x98,0x11,0x69,0xd9,0x8e,0x94,0x9b,0x1e,0x87,0xe9,0xce,0x55,0x28,0xdf,
  0x8c,0xa1,0x89,0x0d,0xbf,0xe6,0x42,0x68,0x41,0x99,0x2d,0x0f,0xb0,0x54,0xbb,0x16
};

static const unsigned char invSbox[256] = {
  0x52,0x09,0x6A,0xD5,0x30,0x36,0xA5,0x38,0xBF,0x40,0xA3,0x9E,0x81,0xF3,0xD7,0xFB,
  0x7C,0xE3,0x39,0x82,0x9B,0x2F,0xFF,0x87,0x34,0x8E,0x43,0x44,0xC4,0xDE,0xE9,0xCB,
  0x54,0x7B,0x94,0x32,0xA6,0xC2,0x23,0x3D,0xEE,0x4C,0x95,0x0B,0x42,0xFA,0xC3,0x4E,
  0x08,0x2E,0xA1,0x66,0x28,0xD9,0x24,0xB2,0x76,0x5B,0xA2,0x49,0x6D,0x8B,0xD1,0x25,
  0x72,0xF8,0xF6,0x64,0x86,0x68,0x98,0x16,0xD4,0xA4,0x5C,0xCC,0x5D,0x65,0xB6,0x92,
  0x6C,0x70,0x48,0x50,0xFD,0xED,0xB9,0xDA,0x5E,0x15,0x46,0x57,0xA7,0x8D,0x9D,0x84,
  0x90,0xD8,0xAB,0x00,0x8C,0xBC,0xD3,0x0A,0xF7,0xE4,0x58,0x05,0xB8,0xB3,0x45,0x06,
  0xD0,0x2C,0x1E,0x8F,0xCA,0x3F,0x0F,0x02,0xC1,0xAF,0xBD,0x03,0x01,0x13,0x8A,0x6B,
  0x3A,0x91,0x11,0x41,0x4F,0x67,0xDC,0xEA,0x97,0xF2,0xCF,0xCE,0xF0,0xB4,0xE6,0x73,
  0x96,0xAC,0x74,0x22,0xE7,0xAD,0x35,0x85,0xE2,0xF9,0x37,0xE8,0x1C,0x75,0xDF,0x6E,
  0x47,0xF1,0x1A,0x71,0x1D,0x29,0xC5,0x89,0x6F,0xB7,0x62,0x0E,0xAA,0x18,0xBE,0x1B,
  0xFC,0x56,0x3E,0x4B,0xC6,0xD2,0x79,0x20,0x9A,0xDB,0xC0,0xFE,0x78,0xCD,0x5A,0xF4,
  0x1F,0xDD,0xA8,0x33,0x88,0x07,0xC7,0x31,0xB1,0x12,0x10,0x59,0x27,0x80,0xEC,0x5F,
  0x60,0x51,0x7F,0xA9,0x19,0xB5,0x4A,0x0D,0x2D,0xE5,0x7A,0x9F,0x93,0xC9,0x9C,0xEF,
  0xA0,0xE0,0x3B,0x4D,0xAE,0x2A,0xF5,0xB0,0xC8,0xEB,0xBB,0x3C,0x83,0x53,0x99,0x61,
  0x17,0x2B,0x04,0x7E,0xBA,0x77,0xD6,0x26,0xE1,0x69,0x14,0x63,0x55,0x21,0x0C,0x7D
};

static const unsigned char Rcon[11] = {
  0x00,0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x1B,0x36
};

inline void rotWord(unsigned char word[4]) {
    unsigned char temp = word[0];
    word[0] = word[1];
    word[1] = word[2];
    word[2] = word[3];
    word[3] = temp;
}

inline void subWord(unsigned char word[4]) {
    for (int i = 0; i < 4; i++)
        word[i] = sbox[word[i]];
}

void KeyExpansion(const unsigned char key[], int keyLen, unsigned char roundKeys[], int &nr) {
    int Nk = keyLen / 4;
    nr = (keyLen == 16) ? 10 : (keyLen == 24 ? 12 : 14);
    const int Nb = 4;
    const int totalWords = Nb * (nr + 1);
    unsigned char temp[4];
    // Cópia direta da chave inicial
    for (int i = 0; i < Nk; i++) {
        roundKeys[4*i + 0] = key[4*i + 0];
        roundKeys[4*i + 1] = key[4*i + 1];
        roundKeys[4*i + 2] = key[4*i + 2];
        roundKeys[4*i + 3] = key[4*i + 3];
    }
    for (int i = Nk; i < totalWords; i++) {
        memcpy(temp, roundKeys + 4 * (i - 1), 4);
        if (i % Nk == 0) {
            rotWord(temp);
            subWord(temp);
            temp[0] ^= Rcon[i/Nk];
        } else if (Nk > 6 && (i % Nk) == 4) {
            subWord(temp);
        }
        for (int j = 0; j < 4; j++)
            roundKeys[4*i + j] = roundKeys[4*(i - Nk) + j] ^ temp[j];
    }
}

inline void SubBytes(unsigned char state[16]) {
    for (int i = 0; i < 16; i++)
        state[i] = sbox[state[i]];
}

inline void InvSubBytes(unsigned char state[16]) {
    for (int i = 0; i < 16; i++)
        state[i] = invSbox[state[i]];
}

inline void ShiftRows(unsigned char state[16]) {
    unsigned char temp[16];
    memcpy(temp, state, 16);
    state[1]  = temp[5];
    state[5]  = temp[9];
    state[9]  = temp[13];
    state[13] = temp[1];
    
    state[2]  = temp[10];
    state[6]  = temp[14];
    state[10] = temp[2];
    state[14] = temp[6];
    
    state[3]  = temp[15];
    state[7]  = temp[3];
    state[11] = temp[7];
    state[15] = temp[11];
}

inline void InvShiftRows(unsigned char state[16]) {
    unsigned char temp[16];
    memcpy(temp, state, 16);
    state[1]  = temp[13];
    state[5]  = temp[1];
    state[9]  = temp[5];
    state[13] = temp[9];
    
    state[2]  = temp[10];
    state[6]  = temp[14];
    state[10] = temp[2];
    state[14] = temp[6];
    
    state[3]  = temp[7];
    state[7]  = temp[11];
    state[11] = temp[15];
    state[15] = temp[3];
}

inline unsigned char xtime(unsigned char x) {
    return ((x << 1) ^ ((x >> 7) & 1 ? 0x1b : 0));
}

inline unsigned char multiplyGF(unsigned char a, unsigned char b) {
    unsigned char res = 0;
    for (int i = 0; i < 8; i++) {
        if (b & 1)
            res ^= a;
        unsigned char hi_bit = a & 0x80;
        a <<= 1;
        if(hi_bit)
            a ^= 0x1b;
        b >>= 1;
    }
    return res;
}

inline void MixColumns(unsigned char state[16]) {
    for (int c = 0; c < 4; c++){
        int col = 4 * c;
        unsigned char a0 = state[col+0], a1 = state[col+1],
                      a2 = state[col+2], a3 = state[col+3];
        state[col+0] = xtime(a0) ^ (a1 ^ xtime(a1)) ^ a2 ^ a3;
        state[col+1] = a0 ^ xtime(a1) ^ (a2 ^ xtime(a2)) ^ a3;
        state[col+2] = a0 ^ a1 ^ xtime(a2) ^ (a3 ^ xtime(a3));
        state[col+3] = (a0 ^ xtime(a0)) ^ a1 ^ a2 ^ xtime(a3);
    }
}

inline void InvMixColumns(unsigned char state[16]) {
    for (int c = 0; c < 4; c++){
        int col = 4 * c;
        unsigned char a0 = state[col+0], a1 = state[col+1],
                      a2 = state[col+2], a3 = state[col+3];
        state[col+0] = multiplyGF(a0,0x0e) ^ multiplyGF(a1,0x0b) ^ multiplyGF(a2,0x0d) ^ multiplyGF(a3,0x09);
        state[col+1] = multiplyGF(a0,0x09) ^ multiplyGF(a1,0x0e) ^ multiplyGF(a2,0x0b) ^ multiplyGF(a3,0x0d);
        state[col+2] = multiplyGF(a0,0x0d) ^ multiplyGF(a1,0x09) ^ multiplyGF(a2,0x0e) ^ multiplyGF(a3,0x0b);
        state[col+3] = multiplyGF(a0,0x0b) ^ multiplyGF(a1,0x0d) ^ multiplyGF(a2,0x09) ^ multiplyGF(a3,0x0e);
    }
}

inline void AddRoundKey(unsigned char state[16], const unsigned char roundKey[16]) {
    for (int i = 0; i < 16; i++)
        state[i] ^= roundKey[i];
}

void AES_Encrypt_Block(const unsigned char in[16], unsigned char out[16],
                         const unsigned char roundKeys[], int nr) {
    unsigned char state[16];
    memcpy(state, in, 16);
    AddRoundKey(state, roundKeys);
    for (int round = 1; round < nr; round++){
        SubBytes(state);
        ShiftRows(state);
        MixColumns(state);
        AddRoundKey(state, roundKeys + round*16);
    }
    SubBytes(state);
    ShiftRows(state);
    AddRoundKey(state, roundKeys + nr*16);
    memcpy(out, state, 16);
}

void AES_Decrypt_Block(const unsigned char in[16], unsigned char out[16],
                         const unsigned char roundKeys[], int nr) {
    unsigned char state[16];
    memcpy(state, in, 16);
    AddRoundKey(state, roundKeys + nr*16);
    for (int round = nr - 1; round >= 1; round--){
        InvShiftRows(state);
        InvSubBytes(state);
        AddRoundKey(state, roundKeys + round*16);
        InvMixColumns(state);
    }
    InvShiftRows(state);
    InvSubBytes(state);
    AddRoundKey(state, roundKeys);
    memcpy(out, state, 16);
}

// ===== FUNÇÃO MAIN =====
int main(int argc, char* argv[]) {
    if(argc != 3) {
        cerr << "Uso: " << argv[0] << " entrada.txt saida.txt" << endl;
        return 1;
    }
    
    ifstream fin(argv[1]);
    if(!fin.is_open()){
        cerr << "Erro ao abrir o arquivo de entrada." << endl;
        return 1;
    }
    ofstream fout(argv[2]);
    if(!fout.is_open()){
        cerr << "Erro ao abrir o arquivo de saida." << endl;
        return 1;
    }
    
    int n;
    fin >> n;
    string dummy;
    getline(fin, dummy); // descarta o resto da linha
    
    // Chave AES global
    unsigned char aesKey[32];
    int aesKeyLen = 0; // em bytes
    bool keySet = false;
    unsigned char roundKeys[240];
    int nr = 0;
    
    for (int i = 0; i < n; i++){
        string line;
        getline(fin, line);
        if(line.empty())
            continue;
        istringstream iss(line);
        string command;
        iss >> command;
        
        if(command == "dh") {
            // Comando Diffie‑Hellman
            string a_str, b_str, g_str, p_str;
            iss >> a_str >> b_str >> g_str >> p_str;
            BigInt A, B, G, P;
            fromHex(A, a_str.c_str());
            fromHex(B, b_str.c_str());
            fromHex(G, g_str.c_str());
            fromHex(P, p_str.c_str());
            
            BigInt B_pub;
            modExpBigInt(B_pub, G, B, P);
            
            BigInt s_bn;
            modExpBigInt(s_bn, B_pub, A, P);
            
            int keyHexDigits = (int)a_str.size();
            aesKeyLen = keyHexDigits / 2;
            string s_hex = toHex(s_bn, keyHexDigits);
            for (int j = 0; j < aesKeyLen; j++){
                string byteStr = s_hex.substr(j*2, 2);
                aesKey[j] = (unsigned char) strtoul(byteStr.c_str(), nullptr, 16);
            }
            keySet = true;
            KeyExpansion(aesKey, aesKeyLen, roundKeys, nr);
            fout << "s=" << s_hex << "\n";
        }
        else if(command == "d") {
            // Comando de descriptografia
            string cipherHex;
            iss >> cipherHex;
            if (cipherHex.size() % 32 != 0) {
                int zeros = 32 - (int)(cipherHex.size() % 32);
                cipherHex = string(zeros, '0') + cipherHex;
            }
            int cipherLen = (int)cipherHex.size() / 2;
            unsigned char *cipherBytes = new unsigned char[cipherLen];
            for (int j = 0; j < cipherLen; j++){
                cipherBytes[j] = (unsigned char) strtoul(cipherHex.substr(j*2, 2).c_str(), nullptr, 16);
            }
            unsigned char *plainBytes = new unsigned char[cipherLen];
            
            // Paralelização da descriptografia
            unsigned int threadCount = thread::hardware_concurrency();
            if(threadCount == 0)
                threadCount = 4; // valor default
            int blockCount = cipherLen / 16;
            int blocksPerThread = (blockCount + threadCount - 1) / threadCount;
            
            thread* threads = new thread[threadCount];
            for (unsigned int t = 0; t < threadCount; t++){
                int startBlock = t * blocksPerThread;
                int endBlock = myMin(startBlock + blocksPerThread, blockCount);
                // Capturamos por referência os arrays e variáveis imutáveis necessárias
                threads[t] = thread([startBlock, endBlock, cipherBytes, plainBytes, nr, roundKeys]() {
                    for (int b = startBlock; b < endBlock; b++){
                        int offset = b * 16;
                        AES_Decrypt_Block(cipherBytes + offset, plainBytes + offset, roundKeys, nr);
                    }
                });
            }
            for (unsigned int t = 0; t < threadCount; t++){
                threads[t].join();
            }
            delete[] threads;
            
            // Monta string de saída usando buffer para evitar muitas concatenações
            int outSize = cipherLen * 2;
            char* plainHexBuffer = new char[outSize + 1];
            for (int j = 0; j < cipherLen; j++){
                sprintf(plainHexBuffer + j*2, "%02X", plainBytes[j]);
            }
            plainHexBuffer[outSize] = '\0';
            fout << "m=" << plainHexBuffer << "\n";
            
            delete[] cipherBytes;
            delete[] plainBytes;
            delete[] plainHexBuffer;
        }
        else if(command == "e") {
            // Comando de encriptação
            string plainHex;
            iss >> plainHex;
            if (plainHex.size() % 32 != 0) {
                int zeros = 32 - (int)(plainHex.size() % 32);
                plainHex = string(zeros, '0') + plainHex;
            }
            int plainLen = (int)plainHex.size() / 2;
            unsigned char *plainBytes = new unsigned char[plainLen];
            for (int j = 0; j < plainLen; j++){
                plainBytes[j] = (unsigned char) strtoul(plainHex.substr(j*2, 2).c_str(), nullptr, 16);
            }
            unsigned char *cipherBytes = new unsigned char[plainLen];
            
            // Paralelização da encriptação:
            unsigned int threadCount = thread::hardware_concurrency();
            if(threadCount == 0)
                threadCount = 4;
            int blockCount = plainLen / 16;
            int blocksPerThread = (blockCount + threadCount - 1) / threadCount;
            
            thread* threads = new thread[threadCount];
            for (unsigned int t = 0; t < threadCount; t++){
                int startBlock = t * blocksPerThread;
                int endBlock = myMin(startBlock + blocksPerThread, blockCount);
                threads[t] = thread([startBlock, endBlock, plainBytes, cipherBytes, nr, roundKeys]() {
                    for (int b = startBlock; b < endBlock; b++){
                        int offset = b * 16;
                        AES_Encrypt_Block(plainBytes + offset, cipherBytes + offset, roundKeys, nr);
                    }
                });
            }
            for (unsigned int t = 0; t < threadCount; t++){
                threads[t].join();
            }
            delete[] threads;
            
            int outSize = plainLen * 2;
            char* cipherHexBuffer = new char[outSize + 1];
            for (int j = 0; j < plainLen; j++){
                sprintf(cipherHexBuffer + j*2, "%02X", cipherBytes[j]);
            }
            cipherHexBuffer[outSize] = '\0';
            fout << "c=" << cipherHexBuffer << "\n";
            
            delete[] plainBytes;
            delete[] cipherBytes;
            delete[] cipherHexBuffer;
        }
    }
    
    fin.close();
    fout.close();
    return 0;
}
