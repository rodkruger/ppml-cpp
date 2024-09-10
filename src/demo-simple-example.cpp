#include "openfhe.h"

#include <iostream>

using namespace lbcrypto;

// Scale factor for fixed-point arithmetic
const double SCALE_FACTOR = 1e3;

// Parcialmente homomórficos ...
//
// RSA --> O mais duro (assimétrico)
// AES --> Mais utilizado no mundo (simétrico) -->
// BV
// ...

// Sistemas de criptografia completamente homomórfica
//
// 1. BFV
// 2. BGV
// 3. CKKS
// 4. TFHE

void e_addition()
{
    // Step 1: Set up the BFV context
    CCParams<CryptoContextBFVRNS> parameters;
    parameters.SetPlaintextModulus(65537);  // Prime number!
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

    // Client
    // Step 4: Encrypt the plaintexts
    auto ciphertext1 = cryptoContext->Encrypt(keys.publicKey, plaintext1);
    auto ciphertext2 = cryptoContext->Encrypt(keys.publicKey, plaintext2);

    // keys.publicKey --> Chave pública
    // keys.secretKey --> Criptografada da chave privada;

    // Provider
    // Step 5: Perform homomorphic addition
    auto ciphertextSum = cryptoContext->EvalAdd(ciphertext1, ciphertext2);

    // Client
    // Step 6: Decrypt the result
    Plaintext plaintextResult;
    cryptoContext->Decrypt(keys.secretKey, ciphertextSum, &plaintextResult);

    // Step 7: Display the result
    plaintextResult->SetLength(1);  // Ensures the correct length
    std::cout << "Encrypted Sum: " << plaintextResult << std::endl;
}

void e_subtraction()
{
    // Step 1: Set up the BFV context
    CCParams<CryptoContextBFVRNS> parameters;
    parameters.SetPlaintextModulus(65537);   // Set plaintext modulus
    parameters.SetMultiplicativeDepth(2);    // We only need depth 2 for subtraction
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

    // Step 3: Encode and encrypt the plaintexts (the numbers to subtract)
    int64_t num1 = 15;
    int64_t num2 = 10;

    Plaintext plaintext1 = cryptoContext->MakePackedPlaintext({num1});
    Plaintext plaintext2 = cryptoContext->MakePackedPlaintext({num2});

    std::cout << "Plaintext 1: " << num1 << std::endl;
    std::cout << "Plaintext 2: " << num2 << std::endl;

    // Step 4: Encrypt the plaintexts
    auto ciphertext1 = cryptoContext->Encrypt(keys.publicKey, plaintext1);
    auto ciphertext2 = cryptoContext->Encrypt(keys.publicKey, plaintext2);

    // Step 5: Perform homomorphic subtraction
    auto ciphertextDifference = cryptoContext->EvalSub(ciphertext1, ciphertext2);

    // Step 6: Decrypt the result
    Plaintext plaintextResult;
    cryptoContext->Decrypt(keys.secretKey, ciphertextDifference, &plaintextResult);

    // Step 7: Display the result
    plaintextResult->SetLength(1);  // Ensure the correct length
    std::cout << "Encrypted Subtraction Result: " << plaintextResult->GetPackedValue()[0] << std::endl;
}

void e_multiplication()
{
    // Step 1: Set up the BFV context
    CCParams<CryptoContextBFVRNS> parameters;
    parameters.SetPlaintextModulus(65537);   // Set plaintext modulus
    parameters.SetMultiplicativeDepth(2);    // Depth 2 is sufficient for multiplication
    parameters.SetSecurityLevel(HEStd_128_classic);

    // Generate the crypto context
    CryptoContext<DCRTPoly> cryptoContext = GenCryptoContext(parameters);

    // Enable necessary algorithms (encryption, decryption, etc.)
    cryptoContext->Enable(PKE);
    cryptoContext->Enable(KEYSWITCH);
    cryptoContext->Enable(LEVELEDSHE);

    // Step 2: Key generation
    auto keys = cryptoContext->KeyGen();

    // Generate relinearization keys to allow homomorphic multiplication
    cryptoContext->EvalMultKeyGen(keys.secretKey);

    // Step 3: Encode and encrypt the plaintexts (the numbers to multiply)
    int64_t num1 = 6;
    int64_t num2 = 7;

    Plaintext plaintext1 = cryptoContext->MakePackedPlaintext({num1});
    Plaintext plaintext2 = cryptoContext->MakePackedPlaintext({num2});

    std::cout << "Plaintext 1: " << num1 << std::endl;
    std::cout << "Plaintext 2: " << num2 << std::endl;

    // Step 4: Encrypt the plaintexts
    auto ciphertext1 = cryptoContext->Encrypt(keys.publicKey, plaintext1);
    auto ciphertext2 = cryptoContext->Encrypt(keys.publicKey, plaintext2);

    // Step 5: Perform homomorphic multiplication
    auto ciphertextProduct = cryptoContext->EvalMult(ciphertext1, ciphertext2);

    // Step 6: Decrypt the result
    Plaintext plaintextResult;
    cryptoContext->Decrypt(keys.secretKey, ciphertextProduct, &plaintextResult);

    // Step 7: Display the result
    plaintextResult->SetLength(1);  // Ensure the correct length
    std::cout << "Encrypted Multiplication Result: " << plaintextResult->GetPackedValue()[0] << std::endl;
}

// Function to approximate division: a / b ≈ a * (1 / b)
double reciprocal(int64_t divisor) {
    // Simple reciprocal function for integer divisor
    if (divisor != 0) {
        return 1.0 / divisor;
    } else {
        throw std::invalid_argument("Division by zero is not allowed.");
    }
}

void e_division()
{
    // Step 1: Set up the BFV context
    CCParams<CryptoContextBFVRNS> parameters;
    parameters.SetPlaintextModulus(65537);   // Set plaintext modulus
    parameters.SetMultiplicativeDepth(2);    // We need at least depth 2 for multiplication
    parameters.SetSecurityLevel(HEStd_128_classic);

    // Generate the crypto context
    CryptoContext<DCRTPoly> cryptoContext = GenCryptoContext(parameters);

    // Enable necessary algorithms (encryption, decryption, etc.)
    cryptoContext->Enable(PKE);
    cryptoContext->Enable(KEYSWITCH);
    cryptoContext->Enable(LEVELEDSHE);

    // Step 2: Key generation
    auto keys = cryptoContext->KeyGen();

    // Generate relinearization keys for multiplication
    cryptoContext->EvalMultKeyGen(keys.secretKey);

    // Step 3: Encode and encrypt the plaintexts
    int64_t num1 = 15;  // Numerator
    int64_t num2 = 5;   // Denominator

    // Compute reciprocal of num2 in plaintext
    double reciprocalNum2 = reciprocal(num2);

    // Encode plaintexts
    Plaintext plaintext1 = cryptoContext->MakePackedPlaintext({num1});
    Plaintext plaintextReciprocal2 = cryptoContext->MakePackedPlaintext({static_cast<int64_t>(reciprocalNum2 * 65536)}); // Scaled reciprocal for fixed-point approximation

    std::cout << "Plaintext 1 (numerator): " << num1 << std::endl;
    std::cout << "Plaintext reciprocal of 2 (denominator): " << reciprocalNum2 << std::endl;

    // Step 4: Encrypt the plaintexts
    auto ciphertext1 = cryptoContext->Encrypt(keys.publicKey, plaintext1);
    auto ciphertextReciprocal2 = cryptoContext->Encrypt(keys.publicKey, plaintextReciprocal2);

    // Step 5: Perform homomorphic multiplication (to approximate division)
    auto ciphertextResult = cryptoContext->EvalMult(ciphertext1, ciphertextReciprocal2);

    // Step 6: Decrypt the result
    Plaintext plaintextResult;
    cryptoContext->Decrypt(keys.secretKey, ciphertextResult, &plaintextResult);

    // Step 7: Display the result (scaled back down)
    plaintextResult->SetLength(1);  // Ensure the correct length
    std::cout << "Encrypted Division Result (approximation): " << plaintextResult->GetPackedValue()[0] / 65536.0 << std::endl;
}

// Function to perform homomorphic exponentiation (a^b)
Ciphertext<DCRTPoly> EncryptedPow(CryptoContext<DCRTPoly> cc, Ciphertext<DCRTPoly> base, int exponent) {
    auto result = base;

    // Homomorphic exponentiation by multiplying the base by itself 'exponent - 1' times
    for (int i = 1; i < exponent; i++) {
        result = cc->EvalMult(result, base);
    }

    return result;
}

void e_power()
{
    // Step 1: Set up the BFV context
    CCParams<CryptoContextBFVRNS> parameters;
    parameters.SetPlaintextModulus(65537);   // Set plaintext modulus
    parameters.SetMultiplicativeDepth(5);    // Set sufficient depth for powering (depends on the exponent)
    parameters.SetSecurityLevel(HEStd_128_classic);

    // Generate the crypto context
    CryptoContext<DCRTPoly> cryptoContext = GenCryptoContext(parameters);

    // Enable necessary algorithms (encryption, decryption, etc.)
    cryptoContext->Enable(PKE);
    cryptoContext->Enable(KEYSWITCH);
    cryptoContext->Enable(LEVELEDSHE);

    // Step 2: Key generation
    auto keys = cryptoContext->KeyGen();

    // Generate relinearization keys to allow homomorphic multiplication
    cryptoContext->EvalMultKeyGen(keys.secretKey);

    // Step 3: Encode and encrypt the plaintext (the base number and exponent)
    int64_t baseNum = 3;  // Base number
    int exponent = 4;     // Exponent

    Plaintext plaintextBase = cryptoContext->MakePackedPlaintext({baseNum});

    std::cout << "Plaintext base: " << baseNum << std::endl;
    std::cout << "Exponent: " << exponent << std::endl;

    // Step 4: Encrypt the plaintext (the base number)
    auto ciphertextBase = cryptoContext->Encrypt(keys.publicKey, plaintextBase);

    // Step 5: Perform homomorphic exponentiation (base^exponent)
    auto ciphertextResult = EncryptedPow(cryptoContext, ciphertextBase, exponent);

    // Step 6: Decrypt the result
    Plaintext plaintextResult;
    cryptoContext->Decrypt(keys.secretKey, ciphertextResult, &plaintextResult);

    // Step 7: Display the result
    plaintextResult->SetLength(1);  // Ensure the correct length
    std::cout << "Encrypted Power Result (base^exponent): " << plaintextResult->GetPackedValue()[0] << std::endl;
}

// Polynomial approximation of reciprocal using Newton-Raphson method
// Approximates 1/x for x > 0
Ciphertext<DCRTPoly> HomomorphicReciprocal(Ciphertext<DCRTPoly> cipher_x, CryptoContext<DCRTPoly> cc, KeyPair<DCRTPoly> keys, int iterations = 10) {
    
    std::vector<int64_t> two_t = {static_cast<int64_t>(2 * SCALE_FACTOR)};

    // Start with an initial guess (encrypted value, say 0.1 scaled by SCALE_FACTOR)
    std::vector<int64_t> initial_guess_vector = {static_cast<int64_t>(0.1 * SCALE_FACTOR)};
    Plaintext initial_guess_pt = cc->MakePackedPlaintext(initial_guess_vector);
    auto initial_guess = cc->Encrypt(keys.publicKey, initial_guess_pt);
    
    auto guess = initial_guess;
    
    // Iteratively improve the guess for 1/x using Newton-Raphson method
    for (int i = 0; i < iterations; i++) {
        // Compute (2 - x * guess)
        auto term1 = cc->EvalMult(cipher_x, guess);
        auto term2 = cc->EvalSub(cc->MakePackedPlaintext(two_t), term1);
        
        // Update guess to guess * (2 - x * guess)
        guess = cc->EvalMult(guess, term2);
    }
    
    return guess;
}

// Polynomial approximation of square root using Newton-Raphson method
// Approximates sqrt(x) for x > 0
// The formula: f(x) = 0.5 * (guess + n / guess)
Ciphertext<DCRTPoly> HomomorphicSqrt(Ciphertext<DCRTPoly> cipher_x, CryptoContext<DCRTPoly> cc, KeyPair<DCRTPoly> keys, int iterations = 10) {
    
    // Start with an initial guess of 1 (encrypted)
    Plaintext plaintext_input = cc->MakePackedPlaintext({1});
    auto one = cc->Encrypt(keys.publicKey, plaintext_input);
    auto guess = one;
    
    // Newton-Raphson iteration to approximate square root
    for (int i = 0; i < iterations; i++) {
        // Compute reciprocal of guess
        auto reciprocal_guess = HomomorphicReciprocal(guess, cc, keys, iterations);
        
        // Compute (x / guess)
        auto div_term = cc->EvalMult(cipher_x, reciprocal_guess);
        
        // Compute (guess + x / guess)
        auto sum_term = cc->EvalAdd(guess, div_term);
        
        // Update guess to 0.5 * (guess + x / guess)
        guess = cc->EvalMult(sum_term, cc->MakePackedPlaintext({static_cast<int64_t>(0.5 * SCALE_FACTOR)}));
    }
    
    return guess;
}

void e_sqrt()
{
    // Step 1: Set up the BFV context
    CCParams<CryptoContextBFVRNS> parameters;
    parameters.SetPlaintextModulus(65537);   // Set plaintext modulus
    parameters.SetMultiplicativeDepth(5);    // Depth for multiple multiplications in Newton's method
    parameters.SetSecurityLevel(HEStd_128_classic);

    // Generate the crypto context
    CryptoContext<DCRTPoly> cryptoContext = GenCryptoContext(parameters);

    // Enable necessary algorithms (encryption, decryption, etc.)
    cryptoContext->Enable(PKE);
    cryptoContext->Enable(KEYSWITCH);
    cryptoContext->Enable(LEVELEDSHE);

    // Step 2: Key generation
    KeyPair<DCRTPoly> keys = cryptoContext->KeyGen();

    // Generate relinearization keys to allow homomorphic multiplication
    cryptoContext->EvalMultKeyGen(keys.secretKey);

    // Step 2: Encrypt input value (we'll compute sqrt(9), for example)
    std::vector<int64_t> input_vector = {9};
    Plaintext plaintext_input = cryptoContext->MakePackedPlaintext(input_vector);
    auto ciphertext_input = cryptoContext->Encrypt(keys.publicKey, plaintext_input);

    // Step 3: Compute the square root using Newton-Raphson approximation
    int iterations = 10; // Number of iterations for approximation
    auto cipher_result = HomomorphicSqrt(ciphertext_input, cryptoContext, keys, iterations);

    // Step 4: Decrypt result and display it
    Plaintext result;
    cryptoContext->Decrypt(keys.secretKey, cipher_result, &result);
    
    std::cout << "Approximated sqrt(9) = " << result->GetPackedValue()[0] << std::endl;
}

int main()
{
    e_addition();

    e_subtraction();

    e_multiplication();

    e_division();

    e_power();

    e_sqrt();

    return 0;
}
