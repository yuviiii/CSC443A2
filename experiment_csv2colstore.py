import sys
import shutil
import subprocess
import os
import csv
import glob
import matplotlib.pyplot as plt
import numpy as np

def csv2colstore(csvFile, dirName, pageSize):
    """
    Execute ./csv2colstore and return how long it took to process
    """
    result = subprocess.run(
        [
            os.path.join(os.path.dirname(os.path.realpath(__file__)), './csv2colstore'),
            csvFile,
            dirName,
            str(pageSize),
        ],
        stdout=subprocess.PIPE,
    )
    return bytes.decode(result.stdout).split('\n')[2].split(' ')[1]


def main():

    if len(sys.argv) != 3:
        print('Usage: experiment_csv2colstore.py <csvFile> <dirName>')
        sys.exit(1)
    csvFile = sys.argv[1]
    
    dirName = sys.argv[2]
    # if not os.path.exists(dirName):
    #     os.mkdir(dirName)
    # else:
    #     shutil.rmtree(dirName)
    
    outputFile = sys.argv[2]
    totalSize = 2e6
    pageSizes = [
        1 * 2 ** 10,   # 1 KB
        2 * 2 ** 10,   # 2 KB
        4 * 2 ** 10,   # 4 KB
        8 * 2 ** 10,   # 8 KB
        16 * 2 ** 10,   # 16 KB
        32 * 2 ** 10,   # 32 KB
        64 * 2 ** 10,   # 64 KB
        128 * 2 ** 10,  # 128 KB
        256 * 2 ** 10,  # 256 KB
        512 * 2 ** 10,  # 512 KB
        1 * 2 ** 20,  # 1 MB
        2 * 2 ** 20,  # 2 MB
    ]
    
    sizeList = []
    timeTookList = []
    aveRateList = []
    timeAve = 0
    
    csvwriter = csv.DictWriter(sys.stdout, fieldnames=('page_size', 'time_took'))
    csvwriter.writeheader()
    for size in pageSizes:
        aveRate = 0
        timeAve = 0
        timeTookList[:] = []
        for i in range(1):
            timeTook = csv2colstore(
                csvFile,
                dirName,
                size,
            )
            csvwriter.writerow({
                'page_size': size,
                'time_took': int(timeTook),
            })

            timeTookList.append(int(timeTook))
        
        timeAve = sum(timeTookList)/1
        aveRate = totalSize/timeAve
        sizeList.append(int(size))
        aveRateList.append(aveRate)
        
    plt.scatter(sizeList, aveRateList)
    plt.plot(sizeList, aveRateList)
    plt.xlabel("Page Size (Bytes)")
    plt.ylabel("csv2colstore Data Rates (records/s)")
    plt.xscale('log', basex=2)

    plt.ylim(0, 500)
    # plt.ylim(0, 8e6)  for usb write
    plt.title('csv to Colstore ::: ',fontweight='bold')
    plt.show()

if __name__ == '__main__':
    main()