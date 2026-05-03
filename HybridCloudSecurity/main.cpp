#include <iostream>
#include <vector>
#include <chrono>
#include "seal/seal.h"
#include "fhe.h"
#include "enclave.h"

using namespace std;
using namespace seal;
using namespace std::chrono;

int main(int argc, char* argv[])
{
    int N = 3; // default size
    if (argc > 1) {
        N = atoi(argv[1]);
        if (N > 4096) N = 4096; // max slots for 8192 poly modulus
    }

    PublicKey pk;
    SecretKey sk;
    RelinKeys relin;
    GaloisKeys galois;

    CKKSEncoder* encoder = nullptr;
    Encryptor* encryptor = nullptr;
    Evaluator* evaluator = nullptr;
    Decryptor* decryptor = nullptr;

    auto setup_start = high_resolution_clock::now();
    // context is kept alive by the shared_ptr
    auto context = setup_ckks(pk, sk, relin, galois,
                              encoder, encryptor, evaluator, decryptor);
    auto setup_end = high_resolution_clock::now();
    cout << "Setup Time: " << duration_cast<milliseconds>(setup_end - setup_start).count() << " ms" << endl;

    vector<double> data(N, 1.0); // fill with 1.0
    data.resize(encoder->slot_count(), 0.0);
    vector<double> weights(N, 0.5); // fill with 0.5

    // Encrypt
    auto enc_start = high_resolution_clock::now();
    auto enc = encrypt_data(data, encoder, encryptor);
    auto enc_end = high_resolution_clock::now();
    cout << "Encryption Time: " << duration_cast<milliseconds>(enc_end - enc_start).count() << " ms" << endl;

    // Cloud computation
    auto comp_start = high_resolution_clock::now();
    auto linear = linear_compute(enc, weights, encoder, evaluator, galois);
    auto comp_end = high_resolution_clock::now();
    cout << "Cloud Computation Time: " << duration_cast<milliseconds>(comp_end - comp_start).count() << " ms" << endl;

    // Send to "Enclave"
    auto dec_start = high_resolution_clock::now();
    Plaintext plain;
    decryptor->decrypt(linear, plain);

    vector<double> decoded;
    encoder->decode(plain, decoded);
    auto dec_end = high_resolution_clock::now();
    cout << "Decryption Time: " << duration_cast<milliseconds>(dec_end - dec_start).count() << " ms" << endl;

    cout << "pre-sigmoid: " << decoded[0] << endl;

    // Non-linear
    auto sigmoid_start = high_resolution_clock::now();
    auto result = sigmoid(decoded);
    auto sigmoid_end = high_resolution_clock::now();
    cout << "Enclave (Sigmoid) Time: " << duration_cast<milliseconds>(sigmoid_end - sigmoid_start).count() << " ms" << endl;

    cout << "\nFinal Output (Sigmoid Result):" << endl;
    int K = 1; // single dot product result
    for (int i = 0; i < K; i++)
        cout << result[i] << endl;

    delete encoder;
    delete encryptor;
    delete evaluator;
    delete decryptor;

    return 0;
}
