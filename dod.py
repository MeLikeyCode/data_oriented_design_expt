import matplotlib.pyplot as plt
import numpy as np
import subprocess
import re
import pandas as pd
import os
import seaborn as sns

def compile_static(num_components):
    """Compile dod.cpp statically with the given number of components.
    
    Compiled executable is called dod.exe and placed in the CWD.
    """
    # compile the code with the given number of components
    subprocess.run(["cl", f"/DUSE_STATIC={num_components}", "/EHsc", "/O2", "dod.cpp"], check=True)


def compile_non_static():
    """Compile dod.cpp without USE_STATIC defined (thus num_components can be passed as a command line arg to it).
    
    Compiled executable is called dod.exe and placed in the CWD.
    """
    # compile the code with no static
    subprocess.run(["cl", "/EHsc", "/O2", "dod.cpp"], check=True)


def run(num_points, num_components):
    """Run dod.exe with the given number of points and optionally components. If components is None, then assumes dod.exe
    was compiled with USE_STATIC defined and thus will use its own number of components.
    
    Returns a tuple of (soa_time, aos_time) where soa_time is the time in microseconds it took the structure of arrays version
    to complete and aos_time is the time in microseconds it took the array of structures version to complete.
    """

    if num_components == None:
        result = subprocess.run(["dod.exe", str(num_points)], check=True,stdout=subprocess.PIPE)
    else:
        result = subprocess.run(["dod.exe", str(num_points), str(num_components)], check=True,stdout=subprocess.PIPE)
    result = result.stdout.decode('utf-8')

    # result looks like this
    # soa: 734us
    # aos: 2563us

    # parse the result
    soa_time = int(re.search('soa: (\d+)us', result).group(1))
    aos_time = int(re.search('aos: (\d+)us', result).group(1))

    return soa_time, aos_time


def expt(points, components):
    """Run the experiment with the given combination of points and components, return a DataFrame with the results.
    
    Args:
        points (list): list of number of points to use
        components (list): list of number of components to use
    """

    df = pd.DataFrame(columns=["num_points", "num_components", "soa_time", "aos_time","static"])

    # do non static
    compile_non_static()
    for c in components:
        for n in points:
            soa_t, aos_t = run(n, c)
            df = pd.concat([df, pd.DataFrame({"num_points": [n], "num_components": [c], "soa_time": [soa_t], "aos_time": [aos_t],"static":[False]})], ignore_index=True)
            

    # do static
    for c in components:
        compile_static(c)
        for n in points:
            soa_t, aos_t = run(n, None)
            df = pd.concat([df, pd.DataFrame({"num_points": [n], "num_components": [c], "soa_time": [soa_t], "aos_time": [aos_t],"static":[True]})], ignore_index=True)

    # replace 0 with 1 to avoid divide by 0
    df["aos_time"] = df["aos_time"].replace(0, 1)
    df["soa_time"] = df["soa_time"].replace(0, 1)

    # add new column for aos/soa ratio
    df["aos/soa"] = df["aos_time"] / df["soa_time"]

    # save the data to a csv file
    df.to_csv("last_expt.csv")
    return df