# Data Oriented Design Experiment
Some code to experiment with and benchmark "data oriented design" concepts. The code that experiments with data oriented design is written in C++, python is just used to benchmark and analyze the C++ program.

The python code is meant to compile/launch the C++ program repeatedly using different parameters and then record its performace. All the data is recorded in a pandas DataFrame.

# Instructions
Use `notebook.ipynb` (jupyter notebook) to experiment. This notebook imports the `expt` function from `dod.py`, which can be used to run the C++ program repeatedly using different parameters, record the runtimes, and then get back a DataFrame with all the results that you can then plot/analyze in your notebook.

# Dependencies
In order to run this code, you need MSVC compiler (Microsoft's C++ compiler) installed, this comes with visual studio installations. The MSVC compiler is used to dynamically compile the C++ code. You also need all the imports in `dod.py` (pandas, numpy, matplotlib).