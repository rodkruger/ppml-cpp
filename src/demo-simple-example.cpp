#include "openfhe.h"

#include <iostream>

using namespace lbcrypto;

int main() {

    // Step 1: Set up the BFV context
    CCParams<CryptoContextBFVRNS> parameters;
    parameters.SetPlaintextModulus(65537);
    parameters.SetMultiplicativeDepth(2);
    parameters.SetSecurityLevel(HEStd_128_classic);

    // Generate the crypto context
    CryptoContext<DCRTPoly> cryptoContext = GenCryptoContext(parameters);

    // Enable necessary algorithms (encryption, decryption, etc.)
    cryptoContext->Enable(PKE);
    cryptoContext->Enable(KEYSWITCH);
    cryptoContext->Enable(LEVELEDSHE);

    // Step 2: Key generation
    auto keys = cryptoContext->KeyGen();

    // Generate relinearization keys
    cryptoContext->EvalMultKeyGen(keys.secretKey);

    // Generate keys for rotation (optional, but necessary for some schemes)
    cryptoContext->EvalAtIndexKeyGen(keys.secretKey, {1, -1});

    // Step 3: Encoding the plaintexts (the numbers to sum)
    int num1 = 5;
    int num2 = 10;

    Plaintext plaintext1 = cryptoContext->MakePackedPlaintext({num1});
    Plaintext plaintext2 = cryptoContext->MakePackedPlaintext({num2});

    std::cout << "Plaintext 1: " << num1 << std::endl;
    std::cout << "Plaintext 2: " << num2 << std::endl;

    // Step 4: Encrypt the plaintexts
    auto ciphertext1 = cryptoContext->Encrypt(keys.publicKey, plaintext1);
    auto ciphertext2 = cryptoContext->Encrypt(keys.publicKey, plaintext2);

    // Step 5: Perform homomorphic addition
    auto ciphertextSum = cryptoContext->EvalAdd(ciphertext1, ciphertext2);

    // Step 6: Decrypt the result
    Plaintext plaintextResult;
    cryptoContext->Decrypt(keys.secretKey, ciphertextSum, &plaintextResult);

    // Step 7: Display the result
    plaintextResult->SetLength(1);  // Ensures the correct length
    std::cout << "Encrypted Sum: " << plaintextResult << std::endl;

    return 0;
}
