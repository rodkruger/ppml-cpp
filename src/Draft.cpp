#include <iostream>
#include "context.h"
#include "client.h"

using namespace hermesml;

void EvalChebyshevFunction() {
    std::cout << "# Generate crypto context" << std::endl;

    auto ckksCtx = HEContextFactory().ckksHeContext();
    auto ckksClient = Client(ckksCtx);
    auto cc = ckksCtx.GetCc();
    auto publicKey = ckksCtx.GetPublicKey();
    auto privateKey = ckksCtx.GetPrivateKey();

    std::cout << "      Ring dimension: " << cc->GetRingDimension() << std::endl;
    std::cout << "      Modulus: " << cc->GetModulus() << std::endl;
    std::cout << "      Multiplicative depth: " << ckksCtx.GetMultiplicativeDepth() << std::endl;

    //-----------------------------------------------------------------------------------------------------------------

    uint32_t polyDegree = 10;
    double lowerBound = 0;
    double upperBound = 2;

    std::vector<std::complex<double> > input{2.0};
    Plaintext plaintext = cc->MakeCKKSPackedPlaintext(input);
    auto ciphertext = cc->Encrypt(publicKey, plaintext);

    auto result = cc->EvalChebyshevFunction([](double x) -> double { return std::sqrt(x); },
                                            ciphertext,
                                            lowerBound,
                                            upperBound,
                                            polyDegree);

    Plaintext plaintextDec;
    cc->Decrypt(privateKey, result, &plaintextDec);

    std::vector<std::complex<double> > finalResult = plaintextDec->GetCKKSPackedValue();
    std::cout << "Actual output\n\t" << finalResult << std::endl << std::endl;
}

#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>
#include <iostream>
#include <vector>
#include <chrono> // For measuring time

const int BIT_SIZE = 64; // 64-bit integer addition

// Function to encrypt a 64-bit integer
std::vector<LweSample *> encrypt_integer(uint64_t number, const TFheGateBootstrappingSecretKeySet *key,
                                         const TFheGateBootstrappingParameterSet *params) {
    std::vector<LweSample *> encrypted_bits(BIT_SIZE);
    for (int i = 0; i < BIT_SIZE; i++) {
        encrypted_bits[i] = new_gate_bootstrapping_ciphertext(params);
        int bit = (number >> i) & 1; // Extract i-th bit
        bootsSymEncrypt(encrypted_bits[i], bit, key);
    }
    return encrypted_bits;
}

// Function to decrypt a 64-bit integer
uint64_t decrypt_integer(const std::vector<LweSample *> &encrypted_bits, const TFheGateBootstrappingSecretKeySet *key) {
    uint64_t number = 0;
    for (int i = 0; i < BIT_SIZE; i++) {
        int bit = bootsSymDecrypt(encrypted_bits[i], key);
        number |= ((uint64_t) bit << i);
    }
    return number;
}

// Function to perform homomorphic 64-bit addition
std::vector<LweSample *> homomorphic_addition(const std::vector<LweSample *> &a, const std::vector<LweSample *> &b,
                                              const TFheGateBootstrappingCloudKeySet *cloud_key,
                                              const TFheGateBootstrappingParameterSet *params) {
    std::vector<LweSample *> sum_bits(BIT_SIZE);
    LweSample *carry = new_gate_bootstrapping_ciphertext(params);
    bootsCONSTANT(carry, 0, cloud_key); // Initial carry is 0

    for (int i = 0; i < BIT_SIZE; i++) {
        // Allocate memory for the sum bit
        sum_bits[i] = new_gate_bootstrapping_ciphertext(params);

        // Temporary variables
        LweSample *temp_sum = new_gate_bootstrapping_ciphertext(params);
        LweSample *temp_carry = new_gate_bootstrapping_ciphertext(params);
        LweSample *temp_and1 = new_gate_bootstrapping_ciphertext(params);
        LweSample *temp_and2 = new_gate_bootstrapping_ciphertext(params);

        // Full adder logic
        bootsXOR(temp_sum, a[i], b[i], cloud_key);
        bootsXOR(sum_bits[i], temp_sum, carry, cloud_key);

        bootsAND(temp_and1, a[i], b[i], cloud_key);
        bootsAND(temp_and2, carry, temp_sum, cloud_key);
        bootsOR(temp_carry, temp_and1, temp_and2, cloud_key);

        // Update carry
        bootsCOPY(carry, temp_carry, cloud_key);

        // Clean up temporary variables
        delete_gate_bootstrapping_ciphertext(temp_sum);
        delete_gate_bootstrapping_ciphertext(temp_carry);
        delete_gate_bootstrapping_ciphertext(temp_and1);
        delete_gate_bootstrapping_ciphertext(temp_and2);
    }

    delete_gate_bootstrapping_ciphertext(carry);
    return sum_bits;
}

void TfheArithmetic() {
    const int minimum_lambda = 110;

    // Generate TFHE parameters and keys
    TFheGateBootstrappingParameterSet *params = new_default_gate_bootstrapping_parameters(minimum_lambda);
    TFheGateBootstrappingSecretKeySet *key = new_random_gate_bootstrapping_secret_keyset(params);
    const TFheGateBootstrappingCloudKeySet *cloud_key = &key->cloud;

    std::cout << "Key generation completed." << std::endl;

    // Input values (64-bit integers)
    uint64_t num1 = 1000;
    uint64_t num2 = 1000;

    // Measure encryption time
    auto start_enc = std::chrono::high_resolution_clock::now();
    std::vector<LweSample *> encrypted_num1 = encrypt_integer(num1, key, params);
    std::vector<LweSample *> encrypted_num2 = encrypt_integer(num2, key, params);
    auto end_enc = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> encryption_time = end_enc - start_enc;

    std::cout << "Encryption completed in " << encryption_time.count() << " seconds." << std::endl;

    // Measure homomorphic addition time
    auto start_add = std::chrono::high_resolution_clock::now();
    std::vector<LweSample *> encrypted_sum = homomorphic_addition(encrypted_num1, encrypted_num2, cloud_key, params);
    auto end_add = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> addition_time = end_add - start_add;

    std::cout << "Homomorphic addition completed in " << addition_time.count() << " seconds." << std::endl;

    // Measure decryption time
    auto start_dec = std::chrono::high_resolution_clock::now();
    uint64_t result = decrypt_integer(encrypted_sum, key);
    auto end_dec = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> decryption_time = end_dec - start_dec;

    std::cout << "Decryption completed in " << decryption_time.count() << " seconds." << std::endl;

    std::cout << "Decrypted Result: " << result << std::endl;

    // Free memory
    for (int i = 0; i < BIT_SIZE; i++) {
        delete_gate_bootstrapping_ciphertext(encrypted_num1[i]);
        delete_gate_bootstrapping_ciphertext(encrypted_num2[i]);
        delete_gate_bootstrapping_ciphertext(encrypted_sum[i]);
    }
    delete_gate_bootstrapping_secret_keyset(key);
    delete_gate_bootstrapping_parameters(params);
}

int main() {
    // Put trash-incredible-code here
}
