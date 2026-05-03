#include "fhe.h"
#include <cmath>

using namespace std;
using namespace seal;

std::shared_ptr<seal::SEALContext> setup_ckks(
    PublicKey &public_key,
    SecretKey &secret_key,
    RelinKeys &relin_keys,
    GaloisKeys &galois_keys,
    CKKSEncoder* &encoder,
    Encryptor* &encryptor,
    Evaluator* &evaluator,
    Decryptor* &decryptor)
{
    EncryptionParameters parms(scheme_type::ckks);
    size_t poly_modulus_degree = 8192;

    parms.set_poly_modulus_degree(poly_modulus_degree);
    parms.set_coeff_modulus(
        CoeffModulus::Create(poly_modulus_degree, {60, 40, 40, 60})
    );

    auto context = make_shared<SEALContext>(parms);

    KeyGenerator keygen(*context);
    secret_key = keygen.secret_key();
    keygen.create_public_key(public_key);
    keygen.create_relin_keys(relin_keys);
    keygen.create_galois_keys(galois_keys);

    encoder = new CKKSEncoder(*context);
    encryptor = new Encryptor(*context, public_key);
    evaluator = new Evaluator(*context);
    decryptor = new Decryptor(*context, secret_key);

    return context;
}

Ciphertext encrypt_data(vector<double> data,
                        CKKSEncoder* encoder,
                        Encryptor* encryptor)
{
    Plaintext plain;
    double scale = pow(2.0, 40);

    encoder->encode(data, scale, plain);

    Ciphertext encrypted;
    encryptor->encrypt(plain, encrypted);

    return encrypted;
}

Ciphertext linear_compute(Ciphertext &enc,
                          vector<double> weights,
                          CKKSEncoder* encoder,
                          Evaluator* evaluator,
                          GaloisKeys &galois_keys)
{
    // 1) encode weights at same scale
    Plaintext pt_w;
    encoder->encode(weights, enc.scale(), pt_w);

    // 2) element-wise multiply
    Ciphertext prod;
    evaluator->multiply_plain(enc, pt_w, prod);

    // 3) rescale to keep scale sane
    evaluator->rescale_to_next_inplace(prod);

    // 4) sum slots (tree reduction)
    Ciphertext acc = prod;
    for (int step = 1; step < (int)weights.size(); step <<= 1)
    {
        Ciphertext rotated;
        evaluator->rotate_vector(acc, step, galois_keys, rotated);
        evaluator->add_inplace(acc, rotated);
    }

    return acc;
}
