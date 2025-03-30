# Barycenter Calculation Project

This project implements a barycenter calculation algorithm using recursive updates and batch processing. The core algorithm is written in C++.

## Prerequisites

To compile and run the project, you need to have a C++ compiler installed on your system. This project has been tested on Ubuntu-based systems and uses the `g++` compiler.

Ensure that you have the following software installed:

- **g++** (C++ compiler)

You can install `g++` with the following command:

```bash
sudo apt update
sudo apt install g++
```

## Steps to Reproduce

Follow these steps to compile and run the barycenter calculation program:

### 1. Clone the Repository (or Create the File Manually)

You can either clone this repository if you have it hosted on a version control platform (e.g., GitHub) or create a `main.cpp` file with the provided C++ code.

### 2. Install `g++` (If not already installed)

If you don't have `g++` installed, run the following command to install it:

```bash
sudo apt update
sudo apt install g++
```

### 3. Compile the C++ Code

Use `g++` to compile the C++ code. Make sure the code is in a file called `main.cpp`. Run the following command in the terminal:

```bash
g++ -std=c++17 main.cpp -o barycenter
```

- `-std=c++17`: Ensures that C++17 standard features are enabled.
- `-o barycenter`: Specifies the output binary file name (`barycenter`).

### 4. Run the Program

After successfully compiling the code, you can run the program with the following command:

```bash
./barycenter
```

### 5. Expected Output

Upon running the program, you should see output similar to the following:

```
Oracle evaluation for first element: 2.0
Barycenter: 0.1234 0.5678 
Final Barycenter (Recursive): 1.2346 3.4567 7.8901
```

### 6. Modify Parameters (Optional)

Feel free to modify the `main.cpp` file to adjust parameters like the number of iterations, `nu`, `lambda`, `sigma`, `zeta`, etc., to experiment with different configurations.

## Troubleshooting

If you encounter any issues during compilation or execution, make sure that you have followed the steps correctly. Common issues include:

- Missing `g++` installation (install using `sudo apt install g++`)
- Typo or error in the C++ code (ensure `main.cpp` is correct)

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
