#pragma once
#include <vector>
#include <memory>
#include "seal/seal.h"

std::shared_ptr<seal::SEALContext> setup_ckks(
    seal::PublicKey &public_key,
    seal::SecretKey &secret_key,
    seal::RelinKeys &relin_keys,
    seal::GaloisKeys &galois_keys,
    seal::CKKSEncoder* &encoder,
    seal::Encryptor* &encryptor,
    seal::Evaluator* &evaluator,
    seal::Decryptor* &decryptor);

seal::Ciphertext encrypt_data(std::vector<double> data,
                              seal::CKKSEncoder* encoder,
                              seal::Encryptor* encryptor);

seal::Ciphertext linear_compute(seal::Ciphertext &enc,
                                std::vector<double> weights,
                                seal::CKKSEncoder* encoder,
                                seal::Evaluator* evaluator,
                                seal::GaloisKeys &galois_keys);
