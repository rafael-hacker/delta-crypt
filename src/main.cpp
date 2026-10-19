#include <sodium.h>
#include <iostream>
#include <CLI/CLI.hpp>
#include <sodium/crypto_pwhash.h>
#include <sodium/crypto_secretstream_xchacha20poly1305.h>
#include <string>
#include <fstream>

int constexpr CHUNK = 4096;

int main(int argc, char* argv[]){
   bool encrypt, decrypt = false; 
    // ______________CLI11_________________
    
    CLI::App app("delta-crypt");
    std::string file{};
    std::string output_filename{};
    std::string password{};
    // ---- flags---
    app.add_flag("-e, --encrypt", encrypt, "encrypt the file");
    app.add_flag("-d, --decrypt", decrypt, "decrypt the file");
    // ----options----
    app.add_option("-f, --file", file , "provide the path or filename");
    app.add_option("-p, --password", password, "enter the password");
    app.add_option("-o, --output", output_filename, "provide the output filename");

    CLI11_PARSE(app, argc, argv);
     
    if (file.empty()){
        std::cerr << "no file was specified" << '\n' << "\tUsage: delta-crypt -f <filename/path> -p <password> \n";
        return 1; 
    }
    
    if (password.empty()){
        std::cerr << "ERROR: a password is required" << '\n' << "\tUsage: delta-crypt -f <filename/path> -p <password> \n";
        return 1;
    }
    // __________libsodium_______________
    if (encrypt){
        std::ifstream fin(file, std::ios::binary);
        std::ofstream out(file+".enc", std::ios::binary);
        if (!fin.is_open() || !out.is_open()){
            std::cerr << "ERROR: FAILED to open " << file <<'\n';
            return 1;
        }

        if (sodium_init() < 0){
            std::cerr << "ERROR: the library couldn't be Initialized; it is not safe to use" << '\n';
            return 1;
        }
        // ----- variables ------- 
        unsigned char salt[crypto_pwhash_SALTBYTES];
        unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES];
        unsigned char header[crypto_secretstream_xchacha20poly1305_HEADERBYTES];
        crypto_secretstream_xchacha20poly1305_state st;
    
        randombytes_buf(salt, sizeof salt); // generate salt
        if(crypto_pwhash(key, sizeof(key), password.c_str(), password.length(), salt, crypto_pwhash_OPSLIMIT_MODERATE, crypto_pwhash_MEMLIMIT_MODERATE, crypto_pwhash_ALG_DEFAULT) != 0){
            std::cerr << "Failed to transform password !" << std::endl;
            fin.close();
            out.close();
            return 1;
        } 
    
        crypto_secretstream_xchacha20poly1305_init_push(&st, header, key); // 
        out.write(reinterpret_cast<const char*>(salt), sizeof(salt));
        out.write(reinterpret_cast<const char*>(header), sizeof(header));
        unsigned char in[CHUNK];
        unsigned char buf[4096+crypto_secretstream_xchacha20poly1305_ABYTES];
        size_t n ; unsigned long long clen; int eof;
    
        do {
            fin.read(reinterpret_cast<char*>(in), CHUNK);
            n = fin.gcount();
            eof = fin.eof();
            unsigned char tag = eof ? crypto_secretstream_xchacha20poly1305_TAG_FINAL : 0;
            crypto_secretstream_xchacha20poly1305_push(&st, buf, &clen, in, n, nullptr, 0, tag);
            out.write(reinterpret_cast<char*>(buf),clen);
        }while (!eof);

        fin.close();
        out.close();
    } else if (decrypt){
        std::ifstream fin(file, std::ios::binary);
        std::ofstream out(output_filename, std::ios::binary);
        // ----- variables ------- 
        unsigned char salt[crypto_pwhash_SALTBYTES];
        unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES];
        unsigned char header[crypto_secretstream_xchacha20poly1305_HEADERBYTES];
        crypto_secretstream_xchacha20poly1305_state st;
        
        fin.read(reinterpret_cast<char*>(salt), sizeof(salt));
        fin.read(reinterpret_cast<char*>(header), sizeof(header));
        if (!fin){std::cerr << "file too small to be encrypted; check if you provided the correct one" << '\n'; return 1;}
        if (crypto_pwhash(key, sizeof(key), password.c_str(), password.length(), salt, crypto_pwhash_OPSLIMIT_MODERATE, crypto_pwhash_MEMLIMIT_MODERATE, crypto_pwhash_ALG_DEFAULT) != 0){
            std::cerr << "Failed to transform password";
            fin.close();
            out.close();
            return 1;
        }
        if (crypto_secretstream_xchacha20poly1305_init_pull(&st, header, key) != 0){
            std::cerr << "invalid header !" << '\n';
            fin.close();
            out.close();
            return 1;
        }
        
        // -------- loop ------------
        unsigned char in[CHUNK + crypto_secretstream_xchacha20poly1305_ABYTES];
        unsigned char buf[CHUNK];
        unsigned long long mlen;
        unsigned char tag;
        bool end = false;
        while (true){
            fin.read(reinterpret_cast<char*>(in), sizeof(in));
            std::streamsize n = fin.gcount();
            if (n == 0){break;}
            if (crypto_secretstream_xchacha20poly1305_pull(&st, buf, &mlen, &tag, in, n, nullptr, 0) != 0){
                std::cerr << "file or password was adulterated" << '\n';
                break;
            }
            out.write(reinterpret_cast<char*>(buf), mlen);
            if(tag == crypto_secretstream_xchacha20poly1305_TAG_FINAL){
                end = true;
                out.close();
                fin.close();
                break;
            }
        }

        sodium_memzero(key, sizeof key);
        if (!end){
            fin.close();
            out.close();
            return 1;
        }
    }
    return 0;

}
